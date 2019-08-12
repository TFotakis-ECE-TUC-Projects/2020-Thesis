#ifndef _NETWORK_PARAMS_H_
#define _NETWORK_PARAMS_H_

typedef struct alexnet_params_t {
	int C1_KERNEL;
	int C1_STRIDE;
	int C1_PADDING;
	int *C1_IN;
	int *C1_OUT;

	int C2_KERNEL;
	int C2_STRIDE;
	int C2_PADDING;
	int *C2_IN;
	int *C2_OUT;

	int C3_KERNEL;
	int C3_STRIDE;
	int C3_PADDING;
	int *C3_IN;
	int *C3_OUT;

	int C4_KERNEL;
	int C4_STRIDE;
	int C4_PADDING;
	int *C4_IN;
	int *C4_OUT;

	int C5_KERNEL;
	int C5_STRIDE;
	int C5_PADDING;
	int *C5_IN;
	int *C5_OUT;

	int M1_KERNEL;
	int M1_STRIDE;
	int *M1_IN;
	int *M1_OUT;

	int M2_KERNEL;
	int M2_STRIDE;
	int *M2_IN;
	int *M2_OUT;

	int M3_KERNEL;
	int M3_STRIDE;
	int *M3_IN;
	int *M3_OUT;

	int L1_IN;
	int L1_OUT;

	int L2_IN;
	int L2_OUT;

	int L3_IN;
	int L3_OUT;
} alexnet_params_t;

alexnet_params_t getParams() {
	alexnet_params_t p = {
		.C1_KERNEL = 11,
		.C1_STRIDE = 4,
		.C1_PADDING = 2,
		.C1_IN = {3, 224, 224},
		.C1_OUT = {64, 55, 55},

		.C2_KERNEL = 5,
		.C2_STRIDE = 1,
		.C2_PADDING = 2,
		.C2_IN = {64, 27, 27},
		.C2_OUT = {192, 27, 27},

		.C3_KERNEL = 3,
		.C3_STRIDE = 1,
		.C3_PADDING = 1,
		.C3_IN = {192, 13, 13},
		.C3_OUT = {384, 13, 13},

		.C4_KERNEL = 3,
		.C4_STRIDE = 1,
		.C4_PADDING = 1,
		.C4_IN = {384, 13, 13},
		.C4_OUT = {256, 7, 7},

		.C5_KERNEL = 3,
		.C5_STRIDE = 1,
		.C5_PADDING = 1,
		.C5_IN = {256, 7, 7},
		.C5_OUT = {256, 4, 7},

		.M1_KERNEL = 3,
		.M1_STRIDE = 2,
		.M1_IN = {64, 55, 55},
		.M1_OUT = {64, 27, 27},

		.M2_KERNEL = 3,
		.M2_STRIDE = 2,
		.M2_IN = {192, 27, 27},
		.M2_OUT = {192, 13, 13},

		.M3_KERNEL = 3,
		.M3_STRIDE = 2,
		.M3_IN = {256, 4, 4},
		.M3_OUT = {256, 2, 2},

		.L1_IN = 9216,
		.L1_OUT = 4096,

		.L2_IN = 4096,
		.L2_OUT = 4096,

		.L3_IN = 4096,
		.L3_OUT = 1000,
	};
	return p;
}

// typedef struct alexnet_weights_t {
// 	int C1_W[]
// } alexnet_weights_t;

#endif
