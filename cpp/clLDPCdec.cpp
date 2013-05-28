/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2013  FAN. Liwen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/
#include <sstream>
#include <map>
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
	cl_device_id device_id,
	unsigned int* rowsta,
	unsigned int* colsta,
	unsigned int* itlver,
	unsigned int* chkind,
	size_t M, size_t N, size_t L,
	unsigned int rowMax, unsigned int colMax,
	char* kernelSource,
	float alpha,
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
		clGetProgramBuildInfo(hdec->program, device_id, CL_PROGRAM_BUILD_LOG, LENGTH, buffer, NULL);
		throw clException(error, std::string(buffer));
	}

	// Create the compute kernel in the program we wish to run
	hdec->vnd = clCreateKernel(hdec->program, "vnd", &error);
	if (!hdec->vnd || error != CL_SUCCESS) { throw clException(error, "clCreateKernel vnd"); }
	hdec->cnd = clCreateKernel(hdec->program, "cnd", &error);
	if (!hdec->cnd || error != CL_SUCCESS) { throw clException(error, "clCreateKernel cnd"); }
	hdec->memset = clCreateKernel(hdec->program, "memset", &error);
	if (!hdec->memset || error != CL_SUCCESS) { throw clException(error, "clCreateKernel memset"); }

	// Create the input and output arrays in device memory for our calculation
	hdec->clm_rowsta = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * (M+1), rowsta, NULL);
	hdec->clm_colsta = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * (N+1), colsta, NULL);
	hdec->clm_itlver = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * L, itlver, NULL);
	hdec->clm_chkind = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * L, chkind, NULL);
	hdec->clm_xram = clCreateBuffer(hdec->context, CL_MEM_READ_WRITE, sizeof(float) * L, NULL, NULL);
	hdec->clm_sout = clCreateBuffer(hdec->context, CL_MEM_READ_WRITE, sizeof(float) * N, NULL, NULL);
	hdec->clm_check = clCreateBuffer(hdec->context, CL_MEM_WRITE_ONLY, sizeof(unsigned char) * M, NULL, NULL);
	hdec->clm_llr = clCreateBuffer(hdec->context, CL_MEM_READ_ONLY, sizeof(float) * N, NULL, NULL);
	if (!hdec->clm_rowsta || !hdec->clm_colsta || !hdec->clm_itlver || !hdec->clm_chkind || !hdec->clm_xram || !hdec->clm_sout || !hdec->clm_check) { throw clException(error, "clCreateBuffer"); }

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

	return hdec;
}

clLDPCdec* clLDPCdec::create(
	cl_device_id device_id,
	unsigned char* H,
	size_t M, size_t N,
	char* kernelSource,
	float alpha,
	void* buf)
{
	unsigned int* rowsta = new unsigned int[M+1];
	::memset(rowsta, 0, (M+1)*sizeof(unsigned int));
	unsigned int* colsta = new unsigned int[N+1];
	::memset(colsta, 0, (N+1)*sizeof(unsigned int));

	std::map<unsigned int, unsigned int> tree;
	unsigned int L = 0;
	for (size_t n=0; n<N; n++) {
		for (size_t m=0; m<M; m++) {
			unsigned int values = m*N+n;
			if (H[values]) {
				tree[values] = L;
				L++;
				rowsta[m+1]++;
				colsta[n+1]++;
			}
		}
	}
	unsigned int rowMax = 0;
	unsigned int colMax = 0;
	for (size_t m=0; m<M; m++) {
		rowMax = (rowsta[m+1] > rowMax) ? rowsta[m+1] : rowMax;
		rowsta[m+1] += rowsta[m];
	}
	for (size_t n=0; n<N; n++) {
		colMax = (colsta[n+1] > colMax) ? colsta[n+1] : colMax;
		colsta[n+1] += colsta[n];
	}

	unsigned int* itlver = new unsigned int[L];
	unsigned int* chkind = new unsigned int[L];
	size_t index = 0;
	for (std::map<unsigned int, unsigned int>::iterator ptr=tree.begin(); ptr!=tree.end(); ptr++) {
		chkind[index] = ptr->first % N;
		itlver[index] = ptr->second;
		index++;
	}
	
	clLDPCdec* hdec = create(device_id, rowsta, colsta, itlver, chkind, M, N, L, rowMax, colMax, kernelSource, alpha, buf);

	delete[] chkind;
	delete[] itlver;
	delete[] colsta;
	delete[] rowsta;
	return hdec;
}

template<class T> clLDPCdec* clLDPCdec::create(
	cl_device_id device_id,
	T* Ir, T* Jc,
	size_t M, size_t N,
	char* kernelSource,
	float alpha,
	void* buf)
{
	unsigned int* rowsta = new unsigned int[M+1];
	::memset(rowsta, 0, (M+1)*sizeof(unsigned int));
	unsigned int* colsta = new unsigned int[N+1];
	::memset(colsta, 0, (N+1)*sizeof(unsigned int));

	std::map<unsigned int, unsigned int> tree;
	unsigned int L = 0;
	for (size_t n=0; n<N; n++) {
		for (T i=Jc[n]; i<Jc[n+1]; i++){
			T m = Ir[i];
			unsigned int values = m*N+n;
			tree[values] = L;
			L++;
			rowsta[m+1]++;
			colsta[n+1]++;
		}
	}

	unsigned int rowMax = 0;
	unsigned int colMax = 0;
	for (size_t m=0; m<M; m++) {
		rowMax = (rowsta[m+1] > rowMax) ? rowsta[m+1] : rowMax;
		rowsta[m+1] += rowsta[m];
	}
	for (size_t n=0; n<N; n++) {
		colMax = (colsta[n+1] > colMax) ? colsta[n+1] : colMax;
		colsta[n+1] += colsta[n];
	}

	unsigned int* itlver = new unsigned int[L];
	unsigned int* chkind = new unsigned int[L];
	size_t index = 0;
	for (std::map<unsigned int, unsigned int>::iterator ptr=tree.begin(); ptr!=tree.end(); ptr++) {
		chkind[index] = ptr->first % N;
		itlver[index] = ptr->second;
		index++;
	}

	clLDPCdec* hdec = create(device_id, rowsta, colsta, itlver, chkind, M, N, L, rowMax, colMax, kernelSource, alpha, buf);

	delete[] chkind;
	delete[] itlver;
	delete[] colsta;
	delete[] rowsta;
	return hdec;
}

template clLDPCdec* clLDPCdec::create<size_t>(cl_device_id device_id, size_t* Ir, size_t* Jc, size_t M, size_t N, char* kernelSource, float alpha, void* buf);
template clLDPCdec* clLDPCdec::create<int>(cl_device_id device_id, int* Ir, int* Jc, size_t M, size_t N, char* kernelSource, float alpha, void* buf);
template clLDPCdec* clLDPCdec::create<unsigned int>(cl_device_id device_id, unsigned int* Ir, unsigned int* Jc, size_t M, size_t N, char* kernelSource, float alpha, void* buf);

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

unsigned int clLDPCdec::decode(float* llr, unsigned int maxIter, float* sout)
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
	}

	clEnqueueReadBuffer(queue, clm_sout, CL_TRUE, 0, sizeof(float) * N, sout, 0, NULL, NULL);
	return iter;
}
