#ifndef ROW_MAX
#define ROW_MAX (32)
#endif

#ifndef EPS
#define EPS (1.175494351e-38f)
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
	int minInd = 0;
	float min1;
	float min2 = MAXFLOAT;
	float rsign = 1.0f;
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
		x_local[i] = _x[p_local[i]]/* + EPS*/;
		rsign *= sign(x_local[i]);
	}
	
	min1 = fabs(x_local[0]);
	for (int i = 1; i < iLength; i++){
		temp = fabs(x_local[i]);
		minInd = select(minInd, i, temp < min1);
		min2 = min(min2, max(min1, temp));
		min1 = min(min1, temp);
	}
	for (int i = 0; i < iLength; i++) {
		_x[p_local[i]] = select(min1, min2, minInd==i) * sign(x_local[i]) * rsign;
	}
}