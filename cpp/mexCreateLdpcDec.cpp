#include <new>
#include "mex.h"
#include "clLDPCdec.h"
#include "Util.h"

/*
* The gateway function
* hdec = mexCreateLdpcDec(rowsta, colsta, itlver, chkind, M, N, L, rowMax, colMax, alpha, KernelSource)
*/
void mexFunction( int nlhs, mxArray *plhs[],
				  int nrhs, const mxArray *prhs[])
{
	int* rowsta = (int*)mxGetData(prhs[0]);
	int* colsta = (int*)mxGetData(prhs[1]);
	int* itlver = (int*)mxGetData(prhs[2]);
	int* chkind = (int*)mxGetData(prhs[3]);
	int M = (int) mxGetScalar(prhs[4]);
	int N = (int) mxGetScalar(prhs[5]);
	int L = (int) mxGetScalar(prhs[6]);
	int rowMax = (int) mxGetScalar(prhs[7]);
	int colMax = (int) mxGetScalar(prhs[8]);
	float alpha = (float) mxGetScalar(prhs[9]);
	char* KernelSource = mxArrayToString(prhs[10]);

	plhs[0] = mxCreateNumericMatrix(sizeof(clLDPCdec), 1, mxUINT8_CLASS, mxREAL);

#ifdef _DEBUG
	mexPrintf("call clLDPCdec::clLDPCdec() begin ...\n");
#endif

	clLDPCdec* hdec = NULL;
	try {
		cl_platform_id platform_id = clUtil::clSelectPlatform(mexUtil::input_d);
		cl_device_id device_id = clUtil::clSelectDevice(platform_id, mexUtil::input_d);
		hdec = clLDPCdec::create(platform_id, device_id, rowsta, colsta, itlver, chkind, M, N, L, rowMax, colMax, alpha, KernelSource, mxGetData(plhs[0]));
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