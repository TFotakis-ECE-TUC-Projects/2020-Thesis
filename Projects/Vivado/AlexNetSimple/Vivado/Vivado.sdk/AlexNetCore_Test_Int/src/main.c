#include <stdio.h>
#include <stdlib.h>
#include <xil_cache.h>
#include "xscugic.h"
#include <xparameters.h>
#include <xconv_core.h>
#include "terminal_colors.h"

typedef double matrix_t;
XScuGic ScuGic;

XConv_core Conv_core;
XConv_core_Config *Conv_core_cfg;

volatile static int RunAcc = 0;
volatile static int ResultAvailAcc = 0;

void acc_start(void *InstancePtr) {
	XConv_core *pAccelerator = (XConv_core *) InstancePtr;
	XConv_core_InterruptEnable(pAccelerator,1);
	XConv_core_InterruptGlobalEnable(pAccelerator);
	XConv_core_Start(pAccelerator);
}

void acc_isr(void *InstancePtr){
	XConv_core *pAccelerator = (XConv_core *)InstancePtr;
	//Disable the global interrupt
	XConv_core_InterruptGlobalDisable(pAccelerator);
	//Disable the local interrupt
	XConv_core_InterruptDisable(pAccelerator,0xffffffff);
	// clear the local interrupt
	XConv_core_InterruptClear(pAccelerator,1);
	ResultAvailAcc = 1;
	// restart the core if it should run again
	if(RunAcc){
		acc_start(pAccelerator);
	}
}

int setup_interrupt(){
	//This functions sets up the interrupt on the ARM
	int result;
	XScuGic_Config *pCfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (pCfg == NULL){
		print("Interrupt Configuration Lookup Failed\n");
		return XST_FAILURE;
	}
	result = XScuGic_CfgInitialize(&ScuGic,pCfg,pCfg->CpuBaseAddress);
	if(result != XST_SUCCESS){
		return result;
	}
	// self-test
	result = XScuGic_SelfTest(&ScuGic);
	if(result != XST_SUCCESS){
		return result;
	}
	// Initialize the exception handler
	Xil_ExceptionInit();
	// Register the exception handler
	//print("Register the exception handler\n\r");
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler,&ScuGic);
	//Enable the exception handler
	Xil_ExceptionEnable();
	// Connect the Adder ISR to the exception table
	//print("Connect the Adder ISR to the Exception handler table\n\r");
	result = XScuGic_Connect(&ScuGic, XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
			(Xil_InterruptHandler)acc_isr,&Conv_core);
	if(result != XST_SUCCESS){
		return result;
	}
	//print("Enable the Adder ISR\n\r");
	XScuGic_Enable(&ScuGic,XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR);
	return XST_SUCCESS;
}

int main() {
    setbuf(stdout, NULL); // No printf flushing needed
    printf("\033[2J"); // Clear terminal
    printf("\033[H");  // Move cursor to the home position

    printf("*** AlexNetCore_Test_Int ***\n");
	printf("- Initializing Conv_core: ");
	Conv_core_cfg = XConv_core_LookupConfig(XPAR_CONV_CORE_0_DEVICE_ID);
	if (!Conv_core_cfg) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	int status = XConv_core_CfgInitialize(&Conv_core, Conv_core_cfg);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	Xil_DCacheDisable();
	printf("%sSuccess%s\n", KGRN, KNRM);

	//Setup the interrupt
	status = setup_interrupt();
	if(status != XST_SUCCESS){
		printf("- %sInterrupt setup failed%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

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
		X_ADDR[i] = X_VALUE;
	}

	for(u32 i = 0; i < WEIGHTS_SIZE; i++){
		WEIGHTS_ADDR[i] = WEIGHT_VALUE;
	}

	for(u32 i = 0; i < BIAS_SIZE; i++){
		BIAS_ADDR[i] = BIAS_VALUE;
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

	if (XConv_core_IsReady(&Conv_core))
		printf("- HLS peripheral is ready.\n- Starting...\n");
	else {
		printf("- %sHLS peripheral is not ready!%s\n", KRED, KNRM);
		exit(-1);
	}

	acc_start(&Conv_core);
	while(!ResultAvailAcc);
	printf("- Interrupt received from HLS HW.\n");
	status = XConv_core_Get_return(&Conv_core);
	printf("- Return value: %d\n", status);

	printf("- Validating: ");
	matrix_t pixel_value;
	matrix_t error = 0;

	pixel_value = X_VALUE * WEIGHT_VALUE * KERNEL_SIZE * KERNEL_SIZE * DIN + BIAS_VALUE;
	error += abs(RES_ADDR[0 * HOUT * WOUT + 23 * WOUT + 23] - pixel_value);
	error += abs(RES_ADDR[1 * HOUT * WOUT + 23 * WOUT + 23] - pixel_value);
	pixel_value = X_VALUE * WEIGHT_VALUE * (KERNEL_SIZE - PADDING) * (KERNEL_SIZE - PADDING) * DIN + BIAS_VALUE;
	error += abs(RES_ADDR[0] - pixel_value);
	pixel_value = X_VALUE * WEIGHT_VALUE * (KERNEL_SIZE - PADDING + 1) * (KERNEL_SIZE - PADDING + 1) * DIN + BIAS_VALUE;
	error += abs(RES_ADDR[0 * HOUT * WOUT + 54 * WOUT + 54] - pixel_value);

	if (error < 0.1) {
		printf("%sSuccess%s\n", KGRN, KNRM);
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
	}
	printf("- Exiting!\n");
	return 0;
}
