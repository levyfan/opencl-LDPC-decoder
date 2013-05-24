#pragma once
#ifndef _CL_EXCEPTION_H_
#define _CL_EXCEPTION_H_

#include <CL\opencl.h>
#include <stdexcept>
#include <string>

class clException : public std::runtime_error
{
public:
	clException(cl_int error, const std::string& msg);

	virtual const char* what() const throw();

private:
	cl_int error;
};

#endif