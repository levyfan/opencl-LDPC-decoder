#include <sstream>
#include "clLDPCdec.h"

clLDPCdec::clLDPCdec()
{
	context = NULL;
	queue = NULL;
	program = NULL;

	clm_rowsta = NULL;
	clm_colsta = NULL;
	clm_itlver = NULL;
	clm_chkind = NULL;
	clm_xram = NULL;
	clm_sout = NULL;
	clm_check = NULL;
	clm_llr = NULL;

	vnd = NULL;
	cnd = NULL;
	memset = NULL;

	M = 0;
	N = 0;
	L = 0;
	check = NULL;
}

clLDPCdec* clLDPCdec::create(
	cl_platform_id platform_id,
	cl_device_id device_id,
	int* rowsta,
	int* colsta,
	int* itlver,
	int* chkind,
	size_t M, size_t N, size_t L,
	int rowMax, int colMax, float alpha,
	char* kernelSource,
	void* buf)
{
	clLDPCdec* hdec = NULL;
	if (buf == NULL) {
		hdec = new clLDPCdec();
	} else {
		hdec = new(buf) clLDPCdec();
	}
	
	hdec->M = M;
	hdec->N = N;
	hdec->L = L;
	hdec->check = new unsigned char[M];

	cl_int error;
	// Create a context
	hdec->context = clCreateContext(0, 1, &device_id, NULL, NULL, &error);
	if (!hdec->context){ throw clException(error, "clCreateContext"); } 

	// Create a command queue
	hdec->queue = clCreateCommandQueue(hdec->context, device_id, 0, &error);
	if (!hdec->queue) { throw clException(error, "clCreateCommandQueue"); }

	// Create the compute program from the source buffer
	hdec->program = clCreateProgramWithSource(hdec->context, 1, (const char **) &kernelSource, NULL, &error);
	if (!hdec->program) { throw clException(error, "clCreateProgramWithSource"); }

	// Build the program executable
	std::ostringstream build_options;
	build_options << "-D ROW_MAX=" << rowMax << " -D COL_MAX=" << colMax << " -D ALPHA=" << alpha;
	error = clBuildProgram(hdec->program, 0, NULL, build_options.str().c_str(), NULL, NULL);
	if (error != CL_SUCCESS) {
		const size_t LENGTH = 2048;
		char buffer[LENGTH];
		clGetProgramBuildInfo(hdec->program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
		throw clException(error, std::string(buffer));
	}

	// Create the compute kernel in the program we wish to run
	hdec->vnd = clCreateKernel(hdec->program, "vnd", &error);
	if (!hdec->vnd || error != CL_SUCCESS) { throw clException(error, "clCreateKernel vnd"); }
	hdec->cnd = clCreateKernel(hdec->program, "cnd", &error);
	if (!hdec->cnd || error != CL_SUCCESS) { throw clException(error, "clCreateKernel cnd"); }
	hdec->memset = clCreateKernel(hdec->program, "memset", &error);
	if (!hdec->memset || error != CL_SUCCESS) { throw clException(error, "clCreateKernel memset"); }

	/*
	hdec->_checksum = clCreateKernel(hdec->program, "checksum", &error);
     */

	// Create the input and output arrays in device memory for our calculation
	hdec->clm_rowsta = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * (M+1), rowsta, NULL);
	hdec->clm_colsta = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * (N+1), colsta, NULL);
	hdec->clm_itlver = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * L, itlver, NULL);
	hdec->clm_chkind = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * L, chkind, NULL);
	hdec->clm_xram = clCreateBuffer(hdec->context, CL_MEM_READ_WRITE, sizeof(float) * L, NULL, NULL);
	hdec->clm_sout = clCreateBuffer(hdec->context, CL_MEM_READ_WRITE, sizeof(float) * N, NULL, NULL);
	hdec->clm_check = clCreateBuffer(hdec->context, CL_MEM_WRITE_ONLY, sizeof(unsigned char) * M, NULL, NULL);
	hdec->clm_llr = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY, sizeof(float) * N, NULL, NULL);
	if (!hdec->clm_rowsta || !hdec->clm_colsta || !hdec->clm_itlver || !hdec->clm_chkind || !hdec->clm_xram || !hdec->clm_sout || !hdec->clm_check) { throw clException(error, "clCreateBuffer"); }

	/*
	hdec->_clm_fail = clCreateBuffer(hdec->context, CL_MEM_WRITE_ONLY, sizeof(unsigned char), NULL, NULL);
     */

	// Set the arguments to our compute kernel
	error  = clSetKernelArg(hdec->vnd, 0, sizeof(cl_mem), &hdec->clm_colsta);
	error |= clSetKernelArg(hdec->vnd, 1, sizeof(cl_mem), &hdec->clm_xram);
	error |= clSetKernelArg(hdec->vnd, 2, sizeof(cl_mem), &hdec->clm_sout);
	error |= clSetKernelArg(hdec->vnd, 3, sizeof(cl_mem), &hdec->clm_llr);
	error |= clSetKernelArg(hdec->cnd, 0, sizeof(cl_mem), &hdec->clm_rowsta);
	error |= clSetKernelArg(hdec->cnd, 1, sizeof(cl_mem), &hdec->clm_itlver);
	error |= clSetKernelArg(hdec->cnd, 2, sizeof(cl_mem), &hdec->clm_chkind);
	error |= clSetKernelArg(hdec->cnd, 3, sizeof(cl_mem), &hdec->clm_xram);
	error |= clSetKernelArg(hdec->cnd, 4, sizeof(cl_mem), &hdec->clm_sout);
	error |= clSetKernelArg(hdec->cnd, 5, sizeof(cl_mem), &hdec->clm_check);
	error |= clSetKernelArg(hdec->memset, 0, sizeof(cl_mem), &hdec->clm_xram);
	if (error != CL_SUCCESS) { throw clException(error, "clSetKernelArg"); }

	/*
	clSetKernelArg(hdec->_checksum, 0, sizeof(cl_mem), &hdec->clm_rowsta);
	clSetKernelArg(hdec->_checksum, 1, sizeof(cl_mem), &hdec->clm_chkind);
	clSetKernelArg(hdec->_checksum, 2, sizeof(cl_mem), &hdec->clm_sout);
	clSetKernelArg(hdec->_checksum, 3, sizeof(cl_mem), &hdec->clm_check);
	clSetKernelArg(hdec->_checksum, 4, sizeof(cl_mem), &hdec->_clm_fail);
     */

	return hdec;
}

clLDPCdec::~clLDPCdec(void)
{
	if (check != NULL) {
		delete[] check;
	}

	clReleaseMemObject(clm_rowsta);
	clReleaseMemObject(clm_colsta);
	clReleaseMemObject(clm_itlver);
	clReleaseMemObject(clm_chkind);
	clReleaseMemObject(clm_xram);
	clReleaseMemObject(clm_sout);
	clReleaseMemObject(clm_check);
	clReleaseMemObject(clm_llr);
	clReleaseProgram(program);
	clReleaseKernel(vnd);
	clReleaseKernel(cnd);
	clReleaseKernel(memset);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
}

unsigned int clLDPCdec::decode(float* llr, size_t maxIter, float* sout)
{
	unsigned int iter;
	unsigned char fail = 1;

	clEnqueueWriteBuffer(queue, clm_llr, CL_FALSE, 0, sizeof(float) * N, llr, 0, NULL, NULL);
	clEnqueueNDRangeKernel(queue, memset, 1, NULL, &L, NULL, 0, NULL, NULL);

	for (iter = 0; iter < maxIter && fail; iter++) {
		fail = 0;
		clEnqueueNDRangeKernel(queue, vnd, 1, NULL, &N, NULL, 0, NULL, NULL);
		clEnqueueNDRangeKernel(queue,cnd, 1, NULL, &M, NULL, 0, NULL, NULL);
		
		clEnqueueReadBuffer(queue, clm_check, CL_TRUE, 0, sizeof(unsigned char) * M, check, 0, NULL, NULL);
		#pragma omp parallel for shared(fail)
		for (size_t i=0; i<M; i++) {
			if (check[i] != 0) {
				fail = 1;
			}
		}

		// clEnqueueNDRangeKernel(queue, _checksum, 1, NULL, &M, NULL, 0, NULL, NULL);
		// clEnqueueReadBuffer(queue, _clm_fail, CL_TRUE, 0, sizeof(unsigned char), &fail, 0, NULL, NULL);
	}

	clEnqueueReadBuffer(queue, clm_sout, CL_TRUE, 0, sizeof(float) * N, sout, 0, NULL, NULL);
	return iter;
}