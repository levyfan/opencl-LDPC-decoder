#ifndef COL_MAX
#define COL_MAX (32)
#endif

#ifndef ALPHA
#define ALPHA (1.0f)
#endif

__kernel void vnd(
        __global const int* _colsta,
		__global float* _x,
		__global float* _softout,
		__global const float* _llr) {
	float x_local[COL_MAX];
	
	int k = get_global_id(0);
	int iBegin = _colsta[k];
	int iLength = _colsta[k + 1] - iBegin;
	float sum = _llr[k];
	
	for (int i = 0; i < iLength; i++) {
		x_local[i] = _x[i+iBegin] * ALPHA;
		sum += x_local[i];
	}
	for (int i = 0; i < iLength; i++) {
		_x[i+iBegin] = sum - x_local[i];
	}
	_softout[k] = sum;
}