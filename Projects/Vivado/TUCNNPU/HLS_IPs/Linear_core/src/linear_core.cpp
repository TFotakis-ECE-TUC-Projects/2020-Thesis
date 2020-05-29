#include <string.h>

#define PRAGMA_SUB(x) _Pragma(#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

#define M_AXI_DEPTH 1024
#define UNROLL_FACTOR 32
#define MAX_X_SIZE 9216
#define MAX_RES_SIZE 4096

 typedef float matrix_t;

//#define AP_INT_MAX_W 16
//#include <ap_fixed.h>
//typedef ap_fixed<16, 2, AP_RND> matrix_t;

// unsigned int lastRandomNumber;
//#define MODULUS 1000
//#define MULTIPLIER 17
//#define INCREMENT 25
//
// unsigned int randomNumberGenerator() {
//	unsigned int newRand = (MULTIPLIER * lastRandomNumber + INCREMENT) %
// MODULUS; 	lastRandomNumber = newRand; 	return newRand;
//}

int Linear_Core(
	matrix_t *x,
	matrix_t *weights,
	matrix_t *bias,
	unsigned int in_features,
	unsigned int out_features,
	unsigned int doReLU,
	//	unsigned int randomThreshold,
	//	unsigned int randomSeed,
	matrix_t *res) {
	// clang-format off
#pragma HLS INTERFACE m_axi port=x offset=slave
#pragma HLS INTERFACE m_axi port=weights offset=slave
#pragma HLS INTERFACE m_axi port=bias offset=slave
#pragma HLS INTERFACE s_axilite port=in_features
#pragma HLS INTERFACE s_axilite port=out_features
#pragma HLS INTERFACE s_axilite port=doReLU
// #pragma HLS INTERFACE s_axilite port=randomThreshold
// #pragma HLS INTERFACE s_axilite port=randomSeed
#pragma HLS INTERFACE m_axi port=res offset=slave
#pragma HLS INTERFACE s_axilite port=return
	// clang-format on

	//	lastRandomNumber = randomSeed;

//	in_features = 9216;
//	out_features = 4096;

	// matrix_t xCache[MAX_X_SIZE];
	// memcpy(xCache, x, in_features * sizeof(matrix_t));

	// 	matrix_t value;
	// Loop_out_features:
	// 	for (unsigned int i = 0; i < out_features; i++) {
	// 		value = bias[i];

	// 	Loop_in_features:
	// 		for (unsigned int j = 0; j < in_features; j++) {
	// #pragma HLS PIPELINE
	// 			value += xCache[j] * weights[j + i * in_features];
	// 		}

	// 		res[i] = doReLU && value < 0 ? (matrix_t) 0 : value;
	// 	}

	matrix_t resCache[MAX_RES_SIZE];
	memcpy(resCache, bias, out_features * sizeof(matrix_t));

Loop_in_features:
	for (unsigned int j = 0; j < in_features; j++) {
		// matrix_t xCache = x[j];
	Loop_out_features:
		for (unsigned int i = 0; i < out_features; i++) {
#pragma HLS PIPELINE
			res[i] += x[j] * weights[j + i * in_features];
		}
	}

	for (unsigned int k = 0; k < out_features; k++) {
#pragma HLS PIPELINE
		matrix_t value = resCache[k];
		res[k] = doReLU && value < 0 ? (matrix_t) 0 : value;
	}

	return 0;
}
