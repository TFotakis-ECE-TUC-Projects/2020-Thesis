#include <stdio.h>
#include <stdlib.h>
#include "terminal_colors.h"

#include "platform_conf.h"
#include "testnet_conf.h"
#include "platform_func.h"

int Conv_core_test() {
	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

	printf("\n*** Testing Conv_core ***\n");

	u32 DIN = 3;
	u32 HIN = 224;
	u32 WIN = 224;
	u32 DOUT = 64;
	u32 HOUT = 55;
	u32 WOUT = 55;
	u32 KERNEL_SIZE = 11;
	u32 STRIDE = 4;
	u32 PADDING = 2;

	u32 X_SIZE = (DIN * HIN * WIN);
	u32 WEIGHTS_SIZE = (DOUT * DIN * HOUT * WOUT);
	u32 BIAS_SIZE = DOUT;
	u32 RES_SIZE = (DOUT * HOUT * WOUT);

	matrix_t *X_ADDR = (matrix_t *)malloc(X_SIZE * sizeof(matrix_t));
	matrix_t *WEIGHTS_ADDR =
		(matrix_t *)malloc(WEIGHTS_SIZE * sizeof(matrix_t));
	matrix_t *BIAS_ADDR = (matrix_t *)malloc(BIAS_SIZE * sizeof(matrix_t));
	matrix_t *RES_ADDR = (matrix_t *)malloc(RES_SIZE * sizeof(matrix_t));

	printf("- Initializing Memory: ");
	for (u32 i = 0; i < X_SIZE; i++) {
		X_ADDR[i] = X_VALUE;
	}

	for (u32 i = 0; i < WEIGHTS_SIZE; i++) {
		WEIGHTS_ADDR[i] = WEIGHT_VALUE;
	}

	for (u32 i = 0; i < BIAS_SIZE; i++) {
		BIAS_ADDR[i] = BIAS_VALUE;
	}

	for (u32 i = 0; i < RES_SIZE; i++) {
		RES_ADDR[i] = 0;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Setup: ");
	XConv_core *Conv_core = get_Conv_core();
	XConv_core_Set_x(Conv_core, (u32)(u64)X_ADDR);
	XConv_core_Set_weights(Conv_core, (u32)(u64)WEIGHTS_ADDR);
	XConv_core_Set_bias(Conv_core, (u32)(u64)BIAS_ADDR);
	XConv_core_Set_din(Conv_core, DIN);
	XConv_core_Set_hin(Conv_core, HIN);
	XConv_core_Set_win(Conv_core, WIN);
	XConv_core_Set_dout(Conv_core, DOUT);
	XConv_core_Set_hout(Conv_core, HOUT);
	XConv_core_Set_wout(Conv_core, WOUT);
	XConv_core_Set_kernel_size(Conv_core, KERNEL_SIZE);
	XConv_core_Set_stride(Conv_core, STRIDE);
	XConv_core_Set_padding(Conv_core, PADDING);
	XConv_core_Set_res(Conv_core, (u32)(u64)RES_ADDR);
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Waiting for Conv_core to get ready: ");
	while (!XConv_core_IsReady(Conv_core))
		;
	printf("%sSuccess%s\n", KGRN, KNRM);

	Conv_core_start(Conv_core);

	while (!Conv_core_result_avail)
		;
	Conv_core_result_avail = 0;
	printf("- Interrupt received from Conv_core.\n");
	int status = XConv_core_Get_return(Conv_core);
	printf("- Return value: %d\n", status);

	printf("- Validating: ");
	matrix_t pixel_value;
	matrix_t error = 0;

	pixel_value =
		X_VALUE * WEIGHT_VALUE * KERNEL_SIZE * KERNEL_SIZE * DIN + BIAS_VALUE;
	error += abs(RES_ADDR[0 * HOUT * WOUT + 23 * WOUT + 23] - pixel_value);
	error += abs(RES_ADDR[1 * HOUT * WOUT + 23 * WOUT + 23] - pixel_value);
	pixel_value = X_VALUE * WEIGHT_VALUE * (KERNEL_SIZE - PADDING) *
					  (KERNEL_SIZE - PADDING) * DIN +
				  BIAS_VALUE;
	error += abs(RES_ADDR[0] - pixel_value);
	pixel_value = X_VALUE * WEIGHT_VALUE * (KERNEL_SIZE - PADDING + 1) *
					  (KERNEL_SIZE - PADDING + 1) * DIN +
				  BIAS_VALUE;
	error += abs(RES_ADDR[0 * HOUT * WOUT + 54 * WOUT + 54] - pixel_value);

	if (error < 0.1) {
		printf("%sSuccess%s\n", KGRN, KNRM);
		status = XST_SUCCESS;
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
		status = XST_FAILURE;
	}

	free(X_ADDR);
	free(WEIGHTS_ADDR);
	free(BIAS_ADDR);
	free(RES_ADDR);

	return status;
}

int Maxpool_core_test() {
	matrix_t X_VALUE = .1;

	printf("\n*** Testing Maxpool_core ***\n");

	u32 D = 64;
	u32 HIN = 55;
	u32 WIN = 55;
	u32 HOUT = 27;
	u32 WOUT = 27;
	u32 KERNEL_SIZE = 3;
	u32 STRIDE = 2;

	u32 X_SIZE = (D * HIN * WIN);
	u32 RES_SIZE = (D * HOUT * WOUT);

	matrix_t *X_ADDR = (matrix_t *)malloc(X_SIZE * sizeof(matrix_t));
	matrix_t *RES_ADDR = (matrix_t *)malloc(RES_SIZE * sizeof(matrix_t));

	printf("- Initializing Memory: ");
	for (u32 i = 0; i < X_SIZE; i++) {
		X_ADDR[i] = X_VALUE;
	}
	for (u32 i = 0; i < RES_SIZE; i++) {
		RES_ADDR[i] = 0;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Setup: ");
	XMaxpool_core *Maxpool_core = get_Maxpool_core();
	XMaxpool_core_Set_x(Maxpool_core, (u32)(u64)X_ADDR);
	XMaxpool_core_Set_d(Maxpool_core, D);
	XMaxpool_core_Set_hin(Maxpool_core, HIN);
	XMaxpool_core_Set_win(Maxpool_core, WIN);
	XMaxpool_core_Set_hout(Maxpool_core, HOUT);
	XMaxpool_core_Set_wout(Maxpool_core, WOUT);
	XMaxpool_core_Set_kernel_size(Maxpool_core, KERNEL_SIZE);
	XMaxpool_core_Set_stride(Maxpool_core, STRIDE);
	XMaxpool_core_Set_res(Maxpool_core, (u32)(u64)RES_ADDR);
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Waiting for Maxpool_core to get ready: ");
	while (!XMaxpool_core_IsReady(Maxpool_core))
		;
	printf("%sSuccess%s\n", KGRN, KNRM);

	Maxpool_core_start(Maxpool_core);

	while (!Maxpool_core_result_avail)
		;
	Maxpool_core_result_avail = 0;
	printf("- Interrupt received from Maxpool_core.\n");
	int status = XMaxpool_core_Get_return(Maxpool_core);
	printf("- Return value: %d\n", status);

	printf("- Validating: ");
	matrix_t *res = (matrix_t *)(uint64_t)RES_ADDR;
	matrix_t pixel_value = X_VALUE;
	matrix_t error = 0;

	for (u32 i = 0; i < RES_SIZE; i++) {
		error += abs(res[i] - pixel_value);
	}

	if (error < 0.1) {
		printf("%sSuccess%s\n", KGRN, KNRM);
		status = XST_SUCCESS;
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
		status = XST_FAILURE;
	}

	free(RES_ADDR);
	free(X_ADDR);

	return status;
}

int Linear_core_test() {
	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

	printf("\n*** Testing Linear_core ***\n");

	u32 IN_FEATURES = 9216;
	u32 OUT_FEATURES = 4096;

	u32 X_SIZE = IN_FEATURES;
	u32 WEIGHTS_SIZE = IN_FEATURES * OUT_FEATURES;
	u32 BIAS_SIZE = OUT_FEATURES;
	u32 RES_SIZE = OUT_FEATURES;

	matrix_t *X_ADDR = (matrix_t *)malloc(X_SIZE * sizeof(matrix_t));
	matrix_t *WEIGHTS_ADDR =
		(matrix_t *)malloc(WEIGHTS_SIZE * sizeof(matrix_t));
	matrix_t *BIAS_ADDR = (matrix_t *)malloc(BIAS_SIZE * sizeof(matrix_t));
	matrix_t *RES_ADDR = (matrix_t *)malloc(RES_SIZE * sizeof(matrix_t));

	printf("- Initializing Memory: ");
	for (u32 i = 0; i < X_SIZE; i++) {
		X_ADDR[i] = X_VALUE;
	}

	for (u32 i = 0; i < WEIGHTS_SIZE; i++) {
		WEIGHTS_ADDR[i] = WEIGHT_VALUE;
	}

	for (u32 i = 0; i < BIAS_SIZE; i++) {
		BIAS_ADDR[i] = BIAS_VALUE;
	}

	for (u32 i = 0; i < RES_SIZE; i++) {
		RES_ADDR[i] = 0;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Setup: ");
	XLinear_core *Linear_core = get_Linear_core();
	XLinear_core_Set_x(Linear_core, (u32)(u64)X_ADDR);
	XLinear_core_Set_weights(Linear_core, (u32)(u64)WEIGHTS_ADDR);
	XLinear_core_Set_bias(Linear_core, (u32)(u64)BIAS_ADDR);
	XLinear_core_Set_in_features(Linear_core, IN_FEATURES);
	XLinear_core_Set_out_features(Linear_core, OUT_FEATURES);
	XLinear_core_Set_doReLU(Linear_core, 1);
	XLinear_core_Set_res(Linear_core, (u32)(u64)RES_ADDR);
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Waiting for Linear_core to get ready: ");
	while (!XLinear_core_IsReady(Linear_core))
		;
	printf("%sSuccess%s\n", KGRN, KNRM);

	Linear_core_start(Linear_core);

	while (!Linear_core_result_avail)
		;
	Linear_core_result_avail = 0;
	printf("- Interrupt received from Linear_core.\n");
	int status = XLinear_core_Get_return(Linear_core);
	printf("- Return value: %d\n", status);

	printf("- Validating: ");
	matrix_t *res = (matrix_t *)(uint64_t)RES_ADDR;
	matrix_t pixel_value = X_VALUE * WEIGHT_VALUE * IN_FEATURES + BIAS_VALUE;
	matrix_t error = 0;

	for (u32 i = 0; i < RES_SIZE; i++) {
		error += abs(res[i] - pixel_value);
	}

	if (error < 0.1) {
		printf("%sSuccess%s\n", KGRN, KNRM);
		status = XST_SUCCESS;
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
		status = XST_FAILURE;
	}

	free(X_ADDR);
	free(WEIGHTS_ADDR);
	free(BIAS_ADDR);
	free(RES_ADDR);

	return status;
}

int run_tests() {
	int status;
	status = Conv_core_test();
	if (status != XST_SUCCESS) return status;
	status = Maxpool_core_test();
	if (status != XST_SUCCESS) return status;
	status = Linear_core_test();
	if (status != XST_SUCCESS) return status;
	return XST_SUCCESS;
}

void setup() { setup_platform("*** AlexNetCore_Test_Int ***\n"); }

void loop() {
	u32 runCount = 0;
	while (1) {
		printf("\n\n%s*** Test run %u ***%s\n", KYEL, ++runCount, KNRM);
		int status = run_tests();
		if (status != XST_SUCCESS) break;
	}
}

int main() {
	setup();
	loop();
	printf("- Exiting!\n");
	return 0;
}
