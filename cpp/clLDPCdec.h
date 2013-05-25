#pragma once
#ifndef _CL_LDPC_DEC_H_
#define _CL_LDPC_DEC_H_

const char _vnd_kernel_source[] = 
	"#ifndef COL_MAX\n"
	"#define COL_MAX (32)\n"
	"#endif\n"
	"#ifndef ALPHA\n"
	"#define ALPHA (1.0f)\n"
	"#endif\n"
	"__kernel void vnd(__global const int* _colsta, __global float* _x, __global float* _softout, __global const float* _llr) {\n"
	"    int k = get_global_id(0);\n"
	"    float sum = _llr[k];\n"
	"    int iBegin = _colsta[k];\n"
	"    int iLength = _colsta[k + 1] - iBegin;\n"
	"    float x_local[COL_MAX];\n"
	"    for (int i = 0; i < iLength; i++) {\n"
	"        x_local[i] = _x[i+iBegin] * ALPHA;\n"
	"        sum += x_local[i];\n"
	"    }"
	"    for (int i = 0; i < iLength; i++) {\n"
	"        _x[i+iBegin] = sum - x_local[i];\n"
	"    }\n"
	"    _softout[k] = sum;\n"
	"}\n";
	
const char _cnd_kernel_source[] = 
	"#ifndef ROW_MAX\n"
	"#define ROW_MAX (32)\n"
	"#endif\n"
	"#ifndef EPS\n"
	"#define EPS (1.175494351e-38f)\n"
	"#endif\n"
	"#ifndef LIM\n"
	"#define LIM (18.03f)\n"
	"#endif\n"
	"__kernel void cnd(__global const int* _rowsta, __global const int* _itlv, __global const int* _chkind, __global float* _x, __global float* _softout, __global uchar* _check ) {\n"
	"    int k = get_global_id(0);\n"
	"    int valid = 0;\n"
	"    float prod = 1.0f, tt;\n"
	"    int p_local[ROW_MAX];\n"
	"    float x_local[ROW_MAX];\n"
	"    int iBegin = _rowsta[k];\n"
	"    int iLength = _rowsta[k + 1] - iBegin;\n"
	"    // valid check\n"
	"    for (int i = iBegin; i < iBegin+iLength; i++) {\n"
	"        valid += (_softout[_chkind[i]] < 0);\n"
	"    }\n"
	"    _check[k] = valid % 2;\n"
	"    // horizontal process (CND)\n"
	"    for (int i = 0; i < iLength; i++) {\n"
	"        p_local[i] = _itlv[i+iBegin];\n"
	"        x_local[i] = tanh(_x[p_local[i]]/2 + EPS);\n"
	"        prod *= x_local[i];\n"
	"    }\n"
	"    for (int i = 0; i < iLength; i++) {\n"
	"        tt = clamp(prod/x_local[i], -1.0f, 1.0f);\n"
	"        _x[p_local[i]] = clamp(2*atanh(tt), -LIM, LIM);\n"
	"    }\n"
	"}\n";

const char _memset_kernel_source[] = 
	"__kernel void memset(__global float* mem) {\n"
	"    mem[get_global_id(0)] = 0;\n"
	"}\n";


#include <CL\opencl.h>
#include "clException.h"

class clLDPCdec
{
public:
	virtual ~clLDPCdec(void);

	static clLDPCdec* create(cl_platform_id platform_id, cl_device_id device_id, int* rowsta, int* colsta, int* itlver, int* chkind, size_t M, size_t N, size_t L, int rowMax, int colMax, float alpha, char* kernelSource, void* buf=NULL) throw(clException);

	unsigned int decode(float* llr, size_t maxIter, float* sout);

private:
	clLDPCdec(void);
	clLDPCdec(const clLDPCdec& dec);

	cl_context context;
	cl_command_queue queue;
	cl_program program;

	cl_mem clm_rowsta;
	cl_mem clm_colsta;
	cl_mem clm_itlver;
	cl_mem clm_chkind;
	cl_mem clm_xram;
	cl_mem clm_sout;
	cl_mem clm_check;
	cl_mem clm_llr;

	cl_kernel vnd;
	cl_kernel cnd;
	cl_kernel memset;

	size_t M;
	size_t N;
	size_t L;

	unsigned char* check;
};

#endif // _CL_LDPC_DEC_H_