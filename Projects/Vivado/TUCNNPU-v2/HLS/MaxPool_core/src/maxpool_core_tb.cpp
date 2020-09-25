#include "maxpool_core.hpp"

#define DIN 64
#define HIN 55
#define WIN 55
#define X_SIZE DIN * HIN * WIN

#define HOUT 27
#define WOUT 27
#define RES_SIZE DIN * HOUT * WOUT

#define KERNEL_SIZE 3
#define STRIDE 2

int main() {
	AXI_STREAM xS, resS;

	matrix_t *x = (matrix_t*) malloc(X_SIZE * sizeof(matrix_t));
	matrix_t *res = (matrix_t*) malloc(RES_SIZE * sizeof(matrix_t));

	for (u32 i = 0; i < X_SIZE; i++) {
		x[i] = -128 + (i % 256);
	}

	for (u32 i = 0; i < RES_SIZE; i++) {
		res[i] = 0;
	}

	int useInStream = 0;
	int useOutStream = 0;

	int status = MaxPool_Core((uint128_t *) x, xS, useInStream, DIN, HIN, WIN,
	HOUT, WOUT, KERNEL_SIZE, STRIDE, (uint128_t *) res, resS, 0, useOutStream);

	for (u32 i = 0; i < RES_SIZE; i++) {
		printf("(%d, %d, %d): %d\n", i / (HOUT * WOUT), (i / WOUT) % HOUT,
				i % WOUT, (int) res[i]);
	}

	return 0;
}
