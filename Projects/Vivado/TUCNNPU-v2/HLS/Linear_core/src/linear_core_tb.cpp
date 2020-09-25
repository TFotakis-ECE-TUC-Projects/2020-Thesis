#include "linear_core.hpp"

#define IN_FEATURES 9216
#define OUT_FEATURES 4096

int main() {
	AXI_STREAM xS, resS;

	matrix_t *x = (matrix_t*) malloc(IN_FEATURES * sizeof(matrix_t));
	matrix_t *weights = (matrix_t*) malloc(
	IN_FEATURES * OUT_FEATURES * sizeof(matrix_t));
	matrix_t *bias = (matrix_t*) malloc(OUT_FEATURES * sizeof(matrix_t));
	matrix_t *res = (matrix_t*) malloc(OUT_FEATURES * sizeof(matrix_t));

	for (u32 i = 0; i < IN_FEATURES; i++) {
		x[i] = 1;
	}

	for (u32 i = 0; i < IN_FEATURES * OUT_FEATURES; i++) {
		weights[i] = 1;
	}

	for (u32 i = 0; i < OUT_FEATURES; i++) {
		bias[i] = 1;
	}

	for (u32 i = 0; i < OUT_FEATURES; i++) {
		res[i] = 0;
	}

	int useInStream = 0;
	int useOutStream = 0;
	int xScale = 0;
	int weightsScale = 0;
	int biasScale = 0;
	int doReLU = 0;
	int resTopBit = 13;

	int status = Linear_Core((uint128_t *) x, xS, useInStream, xScale,
			(uint128_t *) weights, weightsScale, (uint128_t *) bias, biasScale,
			IN_FEATURES, OUT_FEATURES, doReLU, (uint128_t *) res, resTopBit, resS, 0,
			useOutStream);

	for (u32 i = 0; i < OUT_FEATURES; i++) {
		printf("%d: 0x%x\n", i, (unsigned char) res[i]);
	}

	return 0;
}
