#include "conv_core.hpp"

//#define MASK 0xFF
//#define MAX_KERNEL_SIZE 11
//#define MAX_INPUT_CHANNEL 3
//#define MAX_INPUT_HEIGHT 17
//#define MAX_INPUT_WIDTH 17
////#define MAX_X_SIZE 150528
//#define MAX_X_SIZE 111867
//#define MAX_OUTPUT_HEIGHT 3
//#define MAX_OUTPUT_WIDTH 3
//#define MAX_OUTPUT_CHANNELS 2

#define DIN 3
#define HIN 17
#define WIN 17
#define DOUT 2
#define HOUT 3
#define WOUT 3
#define KERNEL_SIZE 11
#define STRIDE 4
#define PADDING 2
#define X_SIZE DIN * HIN * WIN
#define WEIGHTS_SIZE DOUT * DIN * KERNEL_SIZE * KERNEL_SIZE
#define BIAS_SIZE DOUT
#define RES_SIZE DOUT * HOUT * WOUT
#define X_SCALE 0
#define WEIGHTS_SCALE 0
#define BIAS_SCALE 0
#define RES_TOP_BIT 8
#define DO_RELU 1

int main() {
	AXI_STREAM xS, resS;

	matrix_t *x = (matrix_t*) malloc(X_SIZE * sizeof(matrix_t));
	matrix_t *weights = (matrix_t*) malloc(WEIGHTS_SIZE * sizeof(matrix_t));
	matrix_t *bias = (matrix_t*) malloc(BIAS_SIZE * sizeof(matrix_t));
	matrix_t *res = (matrix_t*) malloc(RES_SIZE * sizeof(matrix_t));

	for (u32 i = 0; i < X_SIZE; i++) {
//		x[i] = -128 + (i % 256);
		x[i] = 1;
	}
	for (u32 i = 0; i < WEIGHTS_SIZE; i++) {
		weights[i] = 1;
	}
	for (u32 i = 0; i < BIAS_SIZE; i++) {
		bias[i] = 1;
	}
	for (u32 i = 0; i < RES_SIZE; i++) {
		res[i] = 0;
	}

	int useInStream = 0;
	int useOutStream = 0;

	int status = Conv_Core((uint128_t *) x, xS, useInStream, X_SCALE,
			(uint128_t *) weights, WEIGHTS_SCALE, (uint128_t *) bias,
			BIAS_SCALE, DIN, HIN, WIN, DOUT, HOUT, WOUT, KERNEL_SIZE, STRIDE,
			PADDING, DO_RELU, (uint128_t *) res, RES_TOP_BIT, resS, 0, useOutStream);

	for (u32 i = 0; i < RES_SIZE; i++) {
		printf("(%d, %d, %d): %d\n", i / (HOUT * WOUT), (i / WOUT) % HOUT,
				i % WOUT, (int) res[i]);
	}

	return 0;
}
