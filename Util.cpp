#include "Util.h"
#include <sstream>
#include "mex.h"

cl_platform_id clUtil::clSelectPlatform(int (*input_d)(const char* prompt))
{
	const size_t NUM = 16;
	cl_platform_id platforms[NUM];
	cl_uint num_platforms;
	cl_int error = clGetPlatformIDs(NUM, platforms, &num_platforms);
	if (error != CL_SUCCESS) { throw clException(error, "clGetPlatformIDs"); }

	std::ostringstream obuf;
	obuf << "Please choose your OpenCL platform:" << std::endl;
	const size_t LENGTH = 128;
	char platform_name[LENGTH];
	char platform_vendor[LENGTH];
	for (size_t i=0; i<num_platforms; i++) {
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platform_vendor), platform_vendor, NULL);
		obuf << "[" << i << "]: " << platform_name << ": " << platform_vendor << std::endl;
	}
	obuf << std::endl << "Platform: ";

	int platform_index = input_d(obuf.str().c_str());
	cl_platform_id platform_id = platforms[platform_index];
	return platform_id;
}

cl_device_id clUtil::clSelectDevice(cl_platform_id platform_id, int (*input_d)(const char* prompt))
{
	const size_t NUM = 16;
	cl_device_id devices[NUM];
	cl_uint num_devices;
	cl_int error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, NUM, devices, &num_devices);
	if (error != CL_SUCCESS) { throw clException(error, "clGetDeviceIDs"); }

	std::ostringstream obuf;
	obuf << "Please choose your OpenCL device:" << std::endl;
	const size_t LENGTH = 128;
	char device_name[LENGTH];
	char device_vendor[LENGTH];
	for (size_t i=0; i<num_devices; i++) {
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(device_vendor), device_vendor, NULL);
		obuf << "[" << i << "]: " << device_name << ": " << device_vendor << std::endl;
	}
	obuf << std::endl << "Device: ";

	int device_index = input_d(obuf.str().c_str());
	cl_device_id device_id = devices[device_index];
	return device_id;
}

int mexUtil::input_d(const char* prompt)
{
	mxArray* decimal = NULL;
	mxArray* str = mxCreateString(prompt);

	mexCallMATLAB(1, &decimal, 1, &str, "input");

	mexPrintf("\n");
	return mxGetScalar(decimal);
}