__kernel void memset(__global float* mem) {
	mem[get_global_id(0)] = 0;
}