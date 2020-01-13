#include <stdio.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xparameters.h>
#include <xconv_core.h>
#include "terminal_colors.h"

typedef double matrix_t;

int main() {
    setbuf(stdout, NULL); // No printf flushing needed
    printf("\033[2J"); // Clear terminal
    printf("\033[H");  // Move cursor to the home position

	printf("*** AlexNetCore_Test ***\n");
	printf("- Initializing Conv_core: ");
	XConv_core Conv_core;
	XConv_core_Config *Conv_core_cfg;
	Conv_core_cfg = XConv_core_LookupConfig(XPAR_CONV_CORE_0_DEVICE_ID);
	if (Conv_core_cfg) {
		int status = XConv_core_CfgInitialize(&Conv_core, Conv_core_cfg);
		if (status != XST_SUCCESS) {
			printf("%sError%s\n", KRED, KNRM);
			exit(XST_FAILURE);
		}
	}
	Xil_DCacheDisable();
	printf("%sSuccess%s\n", KGRN, KNRM);

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

	matrix_t *X_ADDR = (matrix_t *) malloc(X_SIZE * sizeof(matrix_t));
	matrix_t *WEIGHTS_ADDR = (matrix_t *) malloc(WEIGHTS_SIZE * sizeof(matrix_t));
	matrix_t *BIAS_ADDR = (matrix_t *) malloc(BIAS_SIZE * sizeof(matrix_t));
	matrix_t *RES_ADDR = (matrix_t *) malloc(RES_SIZE * sizeof(matrix_t));

	printf("- Initializing Memory: ");
	for(u32 i = 0; i < X_SIZE; i++){
		X_ADDR[i] = 1;
	}

	for(u32 i = 0; i < WEIGHTS_SIZE; i++){
		WEIGHTS_ADDR[i] = .2;
	}

	for(u32 i = 0; i < BIAS_SIZE; i++){
		BIAS_ADDR[i] = .1;
	}

	for(u32 i = 0; i < RES_SIZE; i++){
		RES_ADDR[i] = 0;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Setup: ");
	XConv_core_Set_x(&Conv_core, (u32) (u64) X_ADDR);
	XConv_core_Set_weights(&Conv_core, (u32) (u64) WEIGHTS_ADDR);
	XConv_core_Set_bias(&Conv_core, (u32) (u64) BIAS_ADDR);
	XConv_core_Set_din(&Conv_core, DIN);
	XConv_core_Set_hin(&Conv_core, HIN);
	XConv_core_Set_win(&Conv_core, WIN);
	XConv_core_Set_dout(&Conv_core, DOUT);
	XConv_core_Set_hout(&Conv_core, HOUT);
	XConv_core_Set_wout(&Conv_core, WOUT);
	XConv_core_Set_kernel_size(&Conv_core, KERNEL_SIZE);
	XConv_core_Set_stride(&Conv_core, STRIDE);
	XConv_core_Set_padding(&Conv_core, PADDING);
	XConv_core_Set_res(&Conv_core, (u32) (u64) RES_ADDR);
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Starting...\n");
	XConv_core_Start(&Conv_core);
	while(!XConv_core_IsDone(&Conv_core));
	int status = XConv_core_Get_return(&Conv_core);
	printf("- Return value: %d\n", status);

	printf("- Validating: ");
	u32 flag = 1;
	for(u32 i = 0; i < RES_SIZE; i++) {
		if (RES_ADDR[i] == 0) {
			flag = 0;
			break;
		}
	}
	if (flag) {
		printf("%sSuccess%s\n", KGRN, KNRM);
	} else {
		printf("%sError%s\n", KRED, KNRM);
	}

	printf("- Exiting!\n");
	return 0;
}
