#include "mex.h"
#include "clLDPCdec.h"

/*
* The gateway function
* mexDestroyLdpcDec(hdec)
*/
void mexFunction( int nlhs, mxArray *plhs[],
				  int nrhs, const mxArray *prhs[])
{
	clLDPCdec* hdec = (clLDPCdec*) mxGetData(prhs[0]);
	
#ifdef _DEBUG
	mexPrintf("call clLDPCdec::~clLDPCdec() begin ...\n");
#endif

	if (hdec != NULL) {
		hdec->~clLDPCdec();
	}

#ifdef _DEBUG
	mexPrintf("call clLDPCdec::~clLDPCdec() end ...\n");
#endif
}