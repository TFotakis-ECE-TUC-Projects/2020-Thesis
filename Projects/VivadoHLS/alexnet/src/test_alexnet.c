#include <stdio.h>
#include "alexnet-params.h"
#include "alexnet.h"

int testConv() {
	printf("- Conv: ");
	fflush(stdout);

	alexnet_params_t p = getParams();
	int K = 3 * 224 * 224;
	int L = 64 * 11 * 11 * 3;
	int M = 64;
	int N = 64 * 55 * 55;

	din_t x[K];
	din_t w[L];
	din_t b[M];
	dout_t o[N];

	int i, j;

	for (i = 0; i < K; i++) {
		x[i] = i;
	}

	for (i = 0; i < L; i++) {
		w[i] = 0;
	}

	for (i = 0; i < M; i++) {
		b[i] = i + 1;
	}

	_Conv2dReLU(x, o, w, b, p.C1_IN[0], p.C1_OUT[0], p.C1_KERNEL, p.C1_KERNEL,
				p.C1_PADDING, p.C1_IN, p.C1_OUT);

	int sum = 0;
	for (i = 0; i < N; i++) {
		sum += o[i];
	}
	int retval = sum == (55 * 55) * (M * (M + 1) / 2) ? 0 : 1;

	if (!retval)
		printf("pass\n");
	else
		printf("fail\n");
	return retval;
}

int testMaxPool() {
	printf("- MaxPool: ");
	fflush(stdout);

	alexnet_params_t p = getParams();
	int N = 1;
	int M = 1;
	int i, j, k;
	for (i = 0; i < 3; i++) {
		N *= p.M1_IN[i];
		M *= p.M1_OUT[i];
	}
	int x[N];
	int o[M];
	for (i = 0; i < N; i++) {
		x[i] = i;
	}

	double targetSum = 0;
	int tmp = 112;
	for (i = 0; i < p.M1_OUT[0]; i++) {
		for (j = 0; j < p.M1_OUT[1]; j++) {
			for (k = 0; k < p.M1_OUT[2]; k++) {
				// printf("%d\n", tmp);
				targetSum += tmp;
				tmp += 2;
			}
			tmp += 56;
		}
		tmp += 55;
	}

	_MaxPool(x, o, p.M1_KERNEL, p.M1_STRIDE, p.M1_IN, p.M1_OUT);

	double sum = 0;
	for (i = 0; i < M; i++) {
		// printf("%d\n", o[i]);
		sum += o[i];
	}

	// printf("Sum: %lf, Target: %lf, ", sum, targetSum);
	int retval = sum == targetSum ? 0 : 1;

	if (!retval)
		printf("pass\n");
	else
		printf("fail\n");
	return retval;
}

int testLinearReLU() {
	printf("- LinearReLU: ");
	fflush(stdout);

	alexnet_params_t p = getParams();
	int N = 2000;
	int M = 1000;

	int x[N];
	int w[N * M];
	int b[M];
	int o[M];

	int i, j;
	for (i = 0; i < N; i++) {
		x[i] = i;
	}
	for (i = 0; i < N * M; i++) {
		w[i] = i - (N * M) / 2;
	}
	for (i = 0; i < M; i++) {
		b[i] = i;
	}

	double targetSum = 0;
	for (i = 0; i < M; i++) {
		int acc = b[i];
		for (j = 0; j < N; j++) {
			acc += x[j] * w[i * N + j];
		}
		if (acc > 0) targetSum += acc;
	}

	_LinearReLU(x, w, b, o, N, M);

	double sum = 0;
	for (i = 0; i < M; i++) {
		sum += o[i];
	}

	int retval = sum == targetSum ? 0 : 1;
	if (!retval)
		printf("pass\n");
	else
		printf("fail\n");
	return retval;
}

int testLinear() {
	printf("- Linear: ");
	fflush(stdout);

	alexnet_params_t p = getParams();
	int N = 2000;
	int M = 1000;

	int x[N];
	int w[N * M];
	int b[M];
	int o[M];

	int i, j;
	for (i = 0; i < N; i++) {
		x[i] = i;
	}
	for (i = 0; i < N * M; i++) {
		w[i] = i - (N * M) / 2;
	}
	for (i = 0; i < M; i++) {
		b[i] = i;
	}

	double targetSum = 0;
	for (i = 0; i < M; i++) {
		int acc = b[i];
		for (j = 0; j < N; j++) {
			acc += x[j] * w[i * N + j];
		}
		targetSum += acc;
	}

	_Linear(x, w, b, o, N, M);

	double sum = 0;
	for (i = 0; i < M; i++) {
		sum += o[i];
	}

	int retval = sum == targetSum ? 0 : 1;
	if (!retval)
		printf("pass\n");
	else
		printf("fail\n");
	return retval;
}

int testWhole() {
	printf("- Whole: ");
	fflush(stdout);

	int retval = 0;
	if (!retval)
		printf("pass\n");
	else
		printf("fail\n");
	return retval;
}

int main() {
	printf("\n**************** Testing ****************\n");

	int retval = 0;
	retval += testConv();
	retval += testMaxPool();
	retval += testLinearReLU();
	retval += testLinear();
	retval += testWhole();

	if (retval == 0)
		printf("**************** Success ****************\n\n");
	else
		printf("***************** Error *****************\n\n");

	return 0;
}
