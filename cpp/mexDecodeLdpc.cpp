#include "mex.h"
#include "clLDPCdec.h"

/*
* The gateway function
* iter = decode(hdec, llr, maxIter, softout)
*/
void mexFunction( int nlhs, mxArray *plhs[],
				  int nrhs, const mxArray *prhs[])
{
	clLDPCdec* hdec = (clLDPCdec*) mxGetData(prhs[0]);
	float* llr = (float*) mxGetData(prhs[1]);
	int maxIter = (int) mxGetScalar(prhs[2]);
	float* softout = (float*) mxGetData(prhs[3]);

#ifdef _DEBUG
	mexPrintf("call clLDPCdec::decode() begin ...\n");
#endif

	int iter = hdec->decode(llr, maxIter, softout);

#ifdef _DEBUG
	mexPrintf("call clLDPCdec::decode() end ...\n");
#endif

	plhs[0] = mxCreateDoubleScalar(iter);
}