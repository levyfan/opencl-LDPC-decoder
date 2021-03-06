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
#include <new>
#include "mex.h"
#include "clLDPCdec.h"
#include "Util.h"

/*
* The gateway function
* hdec = mexCreateLdpcDec(rowsta, colsta, itlver, chkind, M, N, L, rowMax, colMax, KernelSource, alpha)
*/
void mexFunction( int nlhs, mxArray *plhs[],
				  int nrhs, const mxArray *prhs[])
{
	unsigned int* rowsta = (unsigned int*) mxGetData(prhs[0]);
	unsigned int* colsta = (unsigned int*) mxGetData(prhs[1]);
	unsigned int* itlver = (unsigned int*) mxGetData(prhs[2]);
	unsigned int* chkind = (unsigned int*) mxGetData(prhs[3]);
	size_t M = (size_t) mxGetScalar(prhs[4]);
	size_t N = (size_t) mxGetScalar(prhs[5]);
	size_t L = (size_t) mxGetScalar(prhs[6]);
	unsigned int rowMax = (unsigned int) mxGetScalar(prhs[7]);
	unsigned int colMax = (unsigned int) mxGetScalar(prhs[8]);
	char* KernelSource = mxArrayToString(prhs[9]);
	float alpha = (float) mxGetScalar(prhs[10]);

	plhs[0] = mxCreateNumericMatrix(sizeof(clLDPCdec), 1, mxUINT8_CLASS, mxREAL);

#ifdef _DEBUG
	mexPrintf("call clLDPCdec::clLDPCdec() begin ...\n");
#endif

	clLDPCdec* hdec = NULL;
	try {
		cl_platform_id platform_id = clUtil::clSelectPlatform(mexUtil::input_d);
		cl_device_id device_id = clUtil::clSelectDevice(platform_id, mexUtil::input_d);
		hdec = clLDPCdec::create(device_id, rowsta, colsta, itlver, chkind, M, N, L, rowMax, colMax, KernelSource, alpha, mxGetData(plhs[0]));
	} catch (clException& error) {
		if(hdec != NULL) {
			hdec->~clLDPCdec();
		}
		mexErrMsgIdAndTxt("clLDPCdec:clLDPCdec", error.what());
	} catch (...) {
		if(hdec != NULL) {
			hdec->~clLDPCdec();
		}
		mexErrMsgIdAndTxt("clLDPCdec:clLDPCdec", "Fatal error");
	}
	
#ifdef _DEBUG
	mexPrintf("call clLDPCdec::clLDPCdec() end ...\n");
#endif
}
