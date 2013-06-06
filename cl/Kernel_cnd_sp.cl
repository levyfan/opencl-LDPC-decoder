#ifndef ROW_MAX
#define ROW_MAX (32)
#endif

#ifndef EPS
#define EPS (1.175494351e-38f)
#endif

#ifndef LIM
#define LIM (18.03f)
#endif

__kernel void cnd(
		__global const int* _rowsta,
		__global const int* _itlv,
		__global const int* _chkind,
		__global float* _x,
		__global float* _softout,
		__global uchar* _check ) {
	int p_local[ROW_MAX];
	float x_local[ROW_MAX];
	
	int k = get_global_id(0);
	int valid = 0;
	float prod = 1.0f;
	float temp;
	int iBegin = _rowsta[k];
	int iLength = _rowsta[k + 1] - iBegin;

	// valid check
	for (int i = iBegin; i < iBegin+iLength; i++) {
	 	valid += (_softout[_chkind[i]] < 0);
	}
	_check[k] = valid % 2;

	// horizontal process (CND)
	for (int i = 0; i < iLength; i++) {
		p_local[i] = _itlv[i+iBegin];
		x_local[i] = tanh(_x[p_local[i]]/2 + EPS);
		prod *= x_local[i];
	}
	for (int i = 0; i < iLength; i++) {
		temp = clamp(prod/x_local[i], -1.0f, 1.0f);
		_x[p_local[i]] = clamp(2*atanh(temp), -LIM, LIM);
	}
}