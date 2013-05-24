#pragma once
#ifndef _UTIL_H_
#define _UTIL_H_

#include <CL\opencl.h>
#include "clException.h"

class clUtil
{
public:
	static cl_platform_id clSelectPlatform(int (*input_d)(const char* prompt)) throw(clException);
	static cl_device_id clSelectDevice(cl_platform_id platform_id, int (*input_d)(const char* prompt)) throw(clException);
};


class mexUtil
{
public:
	static int input_d(const char* prompt);
};

#endif