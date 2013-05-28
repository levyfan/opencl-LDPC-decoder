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
* hdec = mexCreateLdpcDec(H, KernelSource, alpha)
*/
void mexFunction( int nlhs, mxArray *plhs[],
	int nrhs, const mxArray *prhs[])
{
	mwIndex *ir = mxGetIr(prhs[0]);
	mwIndex *jc = mxGetJc(prhs[0]);
	size_t M = mxGetM(prhs[0]);
	size_t N = mxGetN(prhs[0]);

	char* KernelSource = mxArrayToString(prhs[1]);
	float alpha = (float) mxGetScalar(prhs[2]);

	plhs[0] = mxCreateNumericMatrix(sizeof(clLDPCdec), 1, mxUINT8_CLASS, mxREAL);

#ifdef _DEBUG
	mexPrintf("call clLDPCdec::clLDPCdec() begin ...\n");
#endif

	clLDPCdec* hdec = NULL;
	try {
		cl_platform_id platform_id = clUtil::clSelectPlatform(mexUtil::input_d);
		cl_device_id device_id = clUtil::clSelectDevice(platform_id, mexUtil::input_d);
		hdec = clLDPCdec::create<mwIndex>(device_id, ir, jc, M, N, KernelSource, alpha, mxGetData(plhs[0]));
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
