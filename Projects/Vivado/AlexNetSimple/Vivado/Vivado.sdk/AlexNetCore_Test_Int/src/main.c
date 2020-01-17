#include <stdio.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xparameters.h>
#include "terminal_colors.h"
#include "xscugic.h"

#include <xconv_core.h>
#include <xlinear_core.h>
#include <xmaxpool_core.h>

typedef double matrix_t;

XScuGic ScuGic;

XConv_core Conv_core;
XLinear_core Linear_core;
XMaxpool_core Maxpool_core;

volatile static int Conv_core_result_avail = 0;
volatile static int Linear_core_result_avail = 0;
volatile static int Maxpool_core_result_avail = 0;

void Maxpool_core_start(void *InstancePtr) {
	printf("- Starting Maxpool_core: ");
	XMaxpool_core *pAccelerator = (XMaxpool_core *)InstancePtr;
	XMaxpool_core_InterruptEnable(pAccelerator, 1);
	XMaxpool_core_InterruptGlobalEnable(pAccelerator);
	XMaxpool_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Maxpool_core_isr(void *InstancePtr) {
	XMaxpool_core *pAccelerator = (XMaxpool_core *)InstancePtr;
	// Disable the global interrupt
	XMaxpool_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XMaxpool_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XMaxpool_core_InterruptClear(pAccelerator, 1);
	Maxpool_core_result_avail = 1;
}

int Maxpool_core_setup_interrupt(void *InstancePtr) {
	XMaxpool_core *pAccelerator = (XMaxpool_core *)InstancePtr;
	// Connect the Maxpool_core ISR to the exception table
	int result =
		XScuGic_Connect(&ScuGic, XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
						(Xil_InterruptHandler)Maxpool_core_isr, pAccelerator);
	if (result != XST_SUCCESS) {
		return result;
	}
	// print("Enable the Adder ISR\n\r");
	XScuGic_Enable(&ScuGic, XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR);
	return XST_SUCCESS;
}

void Maxpool_core_init(void *InstancePtr) {
	printf("- Initializing Maxpool_core: ");

	XMaxpool_core *pAccelerator = (XMaxpool_core *)InstancePtr;

	XMaxpool_core_Config *Maxpool_core_cfg =
		XMaxpool_core_LookupConfig(XPAR_MAXPOOL_CORE_0_DEVICE_ID);
	if (!Maxpool_core_cfg) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	int status = XMaxpool_core_CfgInitialize(pAccelerator, Maxpool_core_cfg);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	Maxpool_core_setup_interrupt(pAccelerator);

	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Maxpool_core_test() {
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
	XMaxpool_core_Set_x(&Maxpool_core, (u32)(u64)X_ADDR);
	XMaxpool_core_Set_d(&Maxpool_core, D);
	XMaxpool_core_Set_hin(&Maxpool_core, HIN);
	XMaxpool_core_Set_win(&Maxpool_core, WIN);
	XMaxpool_core_Set_hout(&Maxpool_core, HOUT);
	XMaxpool_core_Set_wout(&Maxpool_core, WOUT);
	XMaxpool_core_Set_kernel_size(&Maxpool_core, KERNEL_SIZE);
	XMaxpool_core_Set_stride(&Maxpool_core, STRIDE);
	XMaxpool_core_Set_res(&Maxpool_core, (u32)(u64)RES_ADDR);
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Waiting for Maxpool_core to get ready: ");
	while (!XMaxpool_core_IsReady(&Maxpool_core))
		;
	printf("%sSuccess%s\n", KGRN, KNRM);

	Maxpool_core_start(&Maxpool_core);

	while (!Maxpool_core_result_avail)
		;
	Maxpool_core_result_avail = 0;
	printf("- Interrupt received from Maxpool_core.\n");
	int status = XMaxpool_core_Get_return(&Maxpool_core);
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
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
	}

//	Todo: Find why it stucks here
//	free(X_ADDR);
	free(RES_ADDR);
}

void Linear_core_start(void *InstancePtr) {
	printf("- Starting Linear_core: ");
	XLinear_core *pAccelerator = (XLinear_core *)InstancePtr;
	XLinear_core_InterruptEnable(pAccelerator, 1);
	XLinear_core_InterruptGlobalEnable(pAccelerator);
	XLinear_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Linear_core_isr(void *InstancePtr) {
	XLinear_core *pAccelerator = (XLinear_core *)InstancePtr;
	// Disable the global interrupt
	XLinear_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XLinear_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XLinear_core_InterruptClear(pAccelerator, 1);
	Linear_core_result_avail = 1;
}

int Linear_core_setup_interrupt(void *InstancePtr) {
	XLinear_core *pAccelerator = (XLinear_core *)InstancePtr;
	// Connect the Linear_core ISR to the exception table
	int result =
		XScuGic_Connect(&ScuGic, XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
						(Xil_InterruptHandler)Linear_core_isr, pAccelerator);
	if (result != XST_SUCCESS) {
		return result;
	}
	// print("Enable the Adder ISR\n\r");
	XScuGic_Enable(&ScuGic, XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR);
	return XST_SUCCESS;
}

void Linear_core_init(void *InstancePtr) {
	printf("- Initializing Linear_core: ");

	XLinear_core *pAccelerator = (XLinear_core *)InstancePtr;

	XLinear_core_Config *Linear_core_cfg =
		XLinear_core_LookupConfig(XPAR_LINEAR_CORE_0_DEVICE_ID);
	if (!Linear_core_cfg) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	int status = XLinear_core_CfgInitialize(pAccelerator, Linear_core_cfg);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	Linear_core_setup_interrupt(pAccelerator);

	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Linear_core_test() {
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
	XLinear_core_Set_x(&Linear_core, (u32)(u64)X_ADDR);
	XLinear_core_Set_weights(&Linear_core, (u32)(u64)WEIGHTS_ADDR);
	XLinear_core_Set_bias(&Linear_core, (u32)(u64)BIAS_ADDR);
	XLinear_core_Set_in_features(&Linear_core, IN_FEATURES);
	XLinear_core_Set_out_features(&Linear_core, OUT_FEATURES);
	XLinear_core_Set_doReLU(&Linear_core, 1);
	XLinear_core_Set_res(&Linear_core, (u32)(u64)RES_ADDR);
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Waiting for Linear_core to get ready: ");
	while (!XLinear_core_IsReady(&Linear_core))
		;
	printf("%sSuccess%s\n", KGRN, KNRM);


	Linear_core_start(&Linear_core);

	while (!Linear_core_result_avail)
		;
	Linear_core_result_avail = 0;
	printf("- Interrupt received from Linear_core.\n");
	int status = XLinear_core_Get_return(&Linear_core);
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
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
	}


	free(X_ADDR);
	free(WEIGHTS_ADDR);
	free(BIAS_ADDR);
	free(RES_ADDR);
}

void Conv_core_start(void *InstancePtr) {
	printf("- Starting Conv_core: ");
	XConv_core *pAccelerator = (XConv_core *)InstancePtr;
	XConv_core_InterruptEnable(pAccelerator, 1);
	XConv_core_InterruptGlobalEnable(pAccelerator);
	XConv_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Conv_core_isr(void *InstancePtr) {
	XConv_core *pAccelerator = (XConv_core *)InstancePtr;
	// Disable the global interrupt
	XConv_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XConv_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XConv_core_InterruptClear(pAccelerator, 1);
	Conv_core_result_avail = 1;
}

int Conv_core_setup_interrupt(void *InstancePtr) {
	XConv_core *pAccelerator = (XConv_core *)InstancePtr;
	// Connect the Conv_core ISR to the exception table
	int result =
		XScuGic_Connect(&ScuGic, XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
						(Xil_InterruptHandler)Conv_core_isr, pAccelerator);
	if (result != XST_SUCCESS) {
		return result;
	}
	// print("Enable the Adder ISR\n\r");
	XScuGic_Enable(&ScuGic, XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR);
	return XST_SUCCESS;
}

void Conv_core_init(void *InstancePtr) {
	printf("- Initializing Conv_core: ");

	XConv_core *pAccelerator = (XConv_core *)InstancePtr;

	XConv_core_Config *Conv_core_cfg =
		XConv_core_LookupConfig(XPAR_CONV_CORE_0_DEVICE_ID);
	if (!Conv_core_cfg) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	int status = XConv_core_CfgInitialize(pAccelerator, Conv_core_cfg);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	Conv_core_setup_interrupt(pAccelerator);

	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Conv_core_test() {
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
	XConv_core_Set_x(&Conv_core, (u32)(u64)X_ADDR);
	XConv_core_Set_weights(&Conv_core, (u32)(u64)WEIGHTS_ADDR);
	XConv_core_Set_bias(&Conv_core, (u32)(u64)BIAS_ADDR);
	XConv_core_Set_din(&Conv_core, DIN);
	XConv_core_Set_hin(&Conv_core, HIN);
	XConv_core_Set_win(&Conv_core, WIN);
	XConv_core_Set_dout(&Conv_core, DOUT);
	XConv_core_Set_hout(&Conv_core, HOUT);
	XConv_core_Set_wout(&Conv_core, WOUT);
	XConv_core_Set_kernel_size(&Conv_core, KERNEL_SIZE);
	XConv_core_Set_stride(&Conv_core, STRIDE);
	XConv_core_Set_padding(&Conv_core, PADDING);
	XConv_core_Set_res(&Conv_core, (u32)(u64)RES_ADDR);
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Waiting for Conv_core to get ready: ");
	while (!XConv_core_IsReady(&Conv_core))
		;
	printf("%sSuccess%s\n", KGRN, KNRM);

	Conv_core_start(&Conv_core);

	while (!Conv_core_result_avail)
		;
	Conv_core_result_avail = 0;
	printf("- Interrupt received from Conv_core.\n");
	int status = XConv_core_Get_return(&Conv_core);
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
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
	}

	free(X_ADDR);
	free(WEIGHTS_ADDR);
	free(BIAS_ADDR);
	free(RES_ADDR);
}

int setup_interrupt() {
	// This functions sets up the interrupt on the ARM
	int result;
	XScuGic_Config *pCfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (pCfg == NULL) {
		print("Interrupt Configuration Lookup Failed\n");
		return XST_FAILURE;
	}
	result = XScuGic_CfgInitialize(&ScuGic, pCfg, pCfg->CpuBaseAddress);
	if (result != XST_SUCCESS) {
		return result;
	}
	// self-test
	result = XScuGic_SelfTest(&ScuGic);
	if (result != XST_SUCCESS) {
		return result;
	}
	// Initialize the exception handler
	Xil_ExceptionInit();
	// Register the exception handler
	// print("Register the exception handler\n\r");
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
								 &ScuGic);
	// Enable the exception handler
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

void setup() {
	setbuf(stdout, NULL);  // No printf flushing needed
	printf("\033[2J");	 // Clear terminal
	printf("\033[H");	  // Move cursor to the home position
	printf("*** AlexNetCore_Test_Int ***\n");

	Xil_DCacheDisable();

	printf("- Setting up interrupts: ");
	int status = setup_interrupt();
	if (status != XST_SUCCESS) {
		printf("- %sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	Conv_core_init(&Conv_core);
	Maxpool_core_init(&Maxpool_core);
	Linear_core_init(&Linear_core);
}

void run_tests() {
	Conv_core_test();
	Maxpool_core_test();
	Linear_core_test();
}

int main() {
	setup();

	u32 runCount = 0;
	while(1){
		printf("\n\n%s*** Test run %u ***%s\n", KYEL, ++runCount, KNRM);
		run_tests();
	}
	printf("- Exiting!\n");
	return 0;
}
