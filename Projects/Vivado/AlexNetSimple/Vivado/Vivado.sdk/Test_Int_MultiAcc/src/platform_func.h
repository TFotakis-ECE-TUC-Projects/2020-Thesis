#ifndef SRC_PLATFORM_FUNC_H_
#define SRC_PLATFORM_FUNC_H_

#include <xil_cache.h>
#include "xscugic.h"

matrix_t *PARAMS_ADDR[LAYERS_NUMBER * 2];

XScuGic ScuGic;

Core *get_Conv_core() {
	return Conv_core_list[0];
}

void Conv_core_isr(Core *core) {
	XConv_core *pAccelerator = (XConv_core *) core->InstancePtr;
	// Disable the global interrupt
	XConv_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XConv_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XConv_core_InterruptClear(pAccelerator, 1);
	Conv_core_result_avail[core->InstanceId] = 1;
}

int Conv_core_setup_interrupt(Core *core) {
	// Connect the Conv_core ISR to the exception table
	int result = XScuGic_Connect(
		&ScuGic,
		CONV_CORE_INTERRUPT_IDS[core->InstanceId],
		(Xil_InterruptHandler) Conv_core_isr,
		core);
	if (result != XST_SUCCESS) return result;
	XScuGic_Enable(&ScuGic, CONV_CORE_INTERRUPT_IDS[core->InstanceId]);
	return XST_SUCCESS;
}

void Conv_core_init(Core *core) {
	printf("- Initializing Conv_core %u: ", core->InstanceId);
	core->InstancePtr = (void *) malloc(sizeof(XConv_core));
	XConv_core_Config *Conv_core_cfg =
		XConv_core_LookupConfig(CONV_CORE_DEVICE_IDS[core->InstanceId]);
	if (!Conv_core_cfg) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	int status = XConv_core_CfgInitialize(core->InstancePtr, Conv_core_cfg);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	status = Conv_core_setup_interrupt(core);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Conv_conf_params_complete(LayerConf *lc) {
	lc->hout = (lc->hin + 2 * lc->padding - lc->kernelSize) / lc->stride + 1;
	lc->wout = (lc->win + 2 * lc->padding - lc->kernelSize) / lc->stride + 1;
	lc->xSize = lc->din * lc->hin * lc->win;
	lc->weightsSize = lc->dout * lc->din * lc->hout * lc->wout;
	lc->biasSize = lc->dout;
	lc->resSize = lc->dout * lc->hout * lc->wout;
}

void Conv_conf_complete(u32 layer_index, u32 params_index, matrix_t *xAddr) {
	LayerConf *lc = &LAYERS_CONF[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &LAYERS_CONF[layer_index - 1];
		if (plc->layerType == LinearReLU || plc->layerType == Linear) {
			printf("Error. Linear layer cannot be followed by Conv "
				   "layer.\n");
			exit(-1);
		}
		lc->din = plc->dout;
		lc->hin = plc->hout;
		lc->win = plc->wout;
		lc->xAddr = plc->resAddr;
	} else {
		lc->xAddr = xAddr;
	}
	Conv_conf_params_complete(lc);
	lc->weightsAddr = PARAMS_ADDR[params_index];
	lc->biasAddr = PARAMS_ADDR[params_index + 1];
	lc->resAddr = (matrix_t *) malloc(lc->resSize * sizeof(matrix_t));
}

void Conv_core_setup(XConv_core *Conv_core, LayerConf lc) {
	printf("- Setup Conv_core: ");
	XConv_core_Set_x(Conv_core, (u32)(u64) lc.xAddr);
	XConv_core_Set_weights(Conv_core, (u32)(u64) lc.weightsAddr);
	XConv_core_Set_bias(Conv_core, (u32)(u64) lc.biasAddr);
	XConv_core_Set_din(Conv_core, lc.din);
	XConv_core_Set_hin(Conv_core, lc.hin);
	XConv_core_Set_win(Conv_core, lc.win);
	XConv_core_Set_dout(Conv_core, lc.dout);
	XConv_core_Set_hout(Conv_core, lc.hout);
	XConv_core_Set_wout(Conv_core, lc.wout);
	XConv_core_Set_kernel_size(Conv_core, lc.kernelSize);
	XConv_core_Set_stride(Conv_core, lc.stride);
	XConv_core_Set_padding(Conv_core, lc.padding);
	XConv_core_Set_res(Conv_core, (u32)(u64) lc.resAddr);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Conv_core_start(void *InstancePtr) {
	XConv_core *pAccelerator = (XConv_core *) InstancePtr;

	printf("- Waiting for Conv_core to get ready: ");
	while (!XConv_core_IsReady(pAccelerator)) continue;
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Starting Conv_core: ");
	XConv_core_InterruptEnable(pAccelerator, 1);
	XConv_core_InterruptGlobalEnable(pAccelerator);
	XConv_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

int Conv_core_wait_int(Core *core) {
	while (!Conv_core_result_avail[core->InstanceId]) continue;
	Conv_core_result_avail[core->InstanceId] = 0;
	printf("- Interrupt received from Conv_core.\n");
	int status = XConv_core_Get_return(core->InstancePtr);
	printf("- Return value: %d\n", status);
	return status;
}

void Conv_core_process(LayerConf lc) {
	Core *core = get_Conv_core();
	Conv_core_setup(core->InstancePtr, lc);
	Conv_core_start(core->InstancePtr);
	Conv_core_wait_int(core);
}

// ----------------------------------------------

Core *get_Maxpool_core() {
	return Maxpool_core_list[0];
}

void Maxpool_core_isr(Core *core) {
	XMaxpool_core *pAccelerator = (XMaxpool_core *) core->InstancePtr;
	// Disable the global interrupt
	XMaxpool_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XMaxpool_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XMaxpool_core_InterruptClear(pAccelerator, 1);
	Maxpool_core_result_avail[core->InstanceId] = 1;
}

int Maxpool_core_setup_interrupt(Core *core) {
	// Connect the Maxpool_core ISR to the exception table
	int result = XScuGic_Connect(
		&ScuGic,
		CONV_CORE_INTERRUPT_IDS[core->InstanceId],
		(Xil_InterruptHandler) Maxpool_core_isr,
		&core);
	if (result != XST_SUCCESS) { return result; }
	// print("Enable the Adder ISR\n\r");
	XScuGic_Enable(&ScuGic, CONV_CORE_INTERRUPT_IDS[core->InstanceId]);
	return XST_SUCCESS;
}

void Maxpool_core_init(Core *core) {
	printf("- Initializing Maxpool_core %u: ", core->InstanceId);
	core->InstancePtr = (void *) malloc(sizeof(XMaxpool_core));
	XMaxpool_core_Config *Maxpool_core_cfg =
		XMaxpool_core_LookupConfig(CONV_CORE_DEVICE_IDS[core->InstanceId]);
	if (!Maxpool_core_cfg) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	int status = XMaxpool_core_CfgInitialize(core->InstancePtr, Maxpool_core_cfg);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	status = Maxpool_core_setup_interrupt(core);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Maxpool_conf_params_complete(LayerConf *lc) {
	lc->dout = lc->din;
	lc->hout = (lc->hin - lc->kernelSize) / lc->stride + 1;
	lc->wout = (lc->win - lc->kernelSize) / lc->stride + 1;
	lc->xSize = lc->din * lc->hin * lc->win;
	lc->resSize = lc->dout * lc->hout * lc->wout;
}

void Maxpool_conf_complete(u32 layer_index, matrix_t *xAddr) {
	LayerConf *lc = &LAYERS_CONF[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &LAYERS_CONF[layer_index - 1];
		if (plc->layerType == LinearReLU || plc->layerType == Linear) {
			printf("Error. Linear layer cannot be followed by Maxpool "
				   "layer.\n");
			exit(-1);
		}
		lc->din = plc->dout;
		lc->hin = plc->hout;
		lc->win = plc->wout;
		lc->xAddr = plc->resAddr;
	} else {
		lc->xAddr = xAddr;
	}

	Maxpool_conf_params_complete(lc);

	lc->resAddr = (matrix_t *) malloc(lc->resSize * sizeof(matrix_t));
}

void Maxpool_core_setup(XMaxpool_core *Maxpool_core, LayerConf lc) {
	printf("- Setup Maxpool_core: ");
	XMaxpool_core_Set_x(Maxpool_core, (u32)(u64) lc.xAddr);
	XMaxpool_core_Set_d(Maxpool_core, lc.din);
	XMaxpool_core_Set_hin(Maxpool_core, lc.hin);
	XMaxpool_core_Set_win(Maxpool_core, lc.win);
	XMaxpool_core_Set_hout(Maxpool_core, lc.hout);
	XMaxpool_core_Set_wout(Maxpool_core, lc.wout);
	XMaxpool_core_Set_kernel_size(Maxpool_core, lc.kernelSize);
	XMaxpool_core_Set_stride(Maxpool_core, lc.stride);
	XMaxpool_core_Set_res(Maxpool_core, (u32)(u64) lc.resAddr);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Maxpool_core_start(void *InstancePtr) {
	XMaxpool_core *pAccelerator = (XMaxpool_core *) InstancePtr;

	printf("- Waiting for Maxpool_core to get ready: ");
	while (!XMaxpool_core_IsReady(pAccelerator)) continue;
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Starting Maxpool_core: ");
	XMaxpool_core_InterruptEnable(pAccelerator, 1);
	XMaxpool_core_InterruptGlobalEnable(pAccelerator);
	XMaxpool_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

int Maxpool_core_wait_int(Core *core) {
	while (!Maxpool_core_result_avail[core->InstanceId]) continue;
	Maxpool_core_result_avail[core->InstanceId] = 0;
	printf("- Interrupt received from Maxpool_core.\n");
	int status = XMaxpool_core_Get_return(core->InstancePtr);
	printf("- Return value: %d\n", status);
	return status;
}

void Maxpool_core_process(LayerConf lc) {
	Core *core = get_Maxpool_core();
	Maxpool_core_setup(core->InstancePtr, lc);
	Maxpool_core_start(core->InstancePtr);
	Maxpool_core_wait_int(core);
}

// ----------------------------------------------

Core *get_Linear_core() {
	return Linear_core_list[0];
}

void Linear_core_isr(Core *core) {
	XLinear_core *pAccelerator = (XLinear_core *) core->InstancePtr;
	// Disable the global interrupt
	XLinear_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XLinear_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XLinear_core_InterruptClear(pAccelerator, 1);
	Linear_core_result_avail[core->InstanceId] = 1;
}

int Linear_core_setup_interrupt(Core *core) {
	// Connect the Linear_core ISR to the exception table
	int result = XScuGic_Connect(
		&ScuGic,
		CONV_CORE_INTERRUPT_IDS[core->InstanceId],
		(Xil_InterruptHandler) Linear_core_isr,
		&core);
	if (result != XST_SUCCESS) { return result; }
	// print("Enable the Adder ISR\n\r");
	XScuGic_Enable(&ScuGic, CONV_CORE_INTERRUPT_IDS[core->InstanceId]);
	return XST_SUCCESS;
}

void Linear_core_init(Core *core) {
	printf("- Initializing Linear_core %u: ", core->InstanceId);
	core->InstancePtr = (void *) malloc(sizeof(XLinear_core));
	XLinear_core_Config *Linear_core_cfg =
		XLinear_core_LookupConfig(CONV_CORE_DEVICE_IDS[core->InstanceId]);
	if (!Linear_core_cfg) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	int status = XLinear_core_CfgInitialize(core->InstancePtr, Linear_core_cfg);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	status = Linear_core_setup_interrupt(core);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Linear_conf_params_complete(LayerConf *lc) {
	lc->xSize = lc->inFeatures;
	lc->weightsSize = lc->inFeatures * lc->outFeatures;
	lc->biasSize = lc->outFeatures;
	lc->resSize = lc->outFeatures;
	lc->doReLU = lc->layerType == LinearReLU ? 1 : 0;
}

void Linear_conf_complete(u32 layer_index, u32 params_index, matrix_t *xAddr) {
	LayerConf *lc = &LAYERS_CONF[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &LAYERS_CONF[layer_index - 1];
		switch (plc->layerType) {
			case Conv:
			case Maxpool:
				lc->inFeatures = plc->dout * plc->hout * plc->wout;
				break;
			case LinearReLU:
			case Linear: lc->inFeatures = plc->outFeatures; break;
		}
		lc->xAddr = plc->resAddr;
	} else {
		lc->xAddr = xAddr;
	}

	Linear_conf_params_complete(lc);

	lc->weightsAddr = PARAMS_ADDR[params_index];
	lc->biasAddr = PARAMS_ADDR[params_index + 1];
	lc->resAddr = (matrix_t *) malloc(lc->resSize * sizeof(matrix_t));
}

void Linear_core_setup(XLinear_core *Linear_core, LayerConf lc) {
	printf("- Setup Linear_core: ");
	XLinear_core_Set_x(Linear_core, (u32)(u64) lc.xAddr);
	XLinear_core_Set_weights(Linear_core, (u32)(u64) lc.weightsAddr);
	XLinear_core_Set_bias(Linear_core, (u32)(u64) lc.biasAddr);
	XLinear_core_Set_in_features(Linear_core, lc.inFeatures);
	XLinear_core_Set_out_features(Linear_core, lc.outFeatures);
	XLinear_core_Set_doReLU(Linear_core, lc.doReLU);
	XLinear_core_Set_res(Linear_core, (u32)(u64) lc.resAddr);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void Linear_core_start(void *InstancePtr) {
	XLinear_core *pAccelerator = (XLinear_core *) InstancePtr;

	printf("- Waiting for Linear_core to get ready: ");
	while (!XLinear_core_IsReady(pAccelerator)) continue;
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Starting Linear_core: ");
	XLinear_core_InterruptEnable(pAccelerator, 1);
	XLinear_core_InterruptGlobalEnable(pAccelerator);
	XLinear_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

int Linear_core_wait_int(Core *core) {
	while (!Linear_core_result_avail[core->InstanceId]) continue;
	Linear_core_result_avail[core->InstanceId] = 0;
	printf("- Interrupt received from Linear_core.\n");
	int status = XLinear_core_Get_return(core->InstancePtr);
	printf("- Return value: %d\n", status);
	return status;
}

void Linear_core_process(LayerConf lc) {
	Core *core = get_Linear_core();
	Linear_core_setup(core->InstancePtr, lc);
	Linear_core_start(core->InstancePtr);
	Linear_core_wait_int(core);
}

// ----------------------------------------------

void setup_stdout() {
	setbuf(stdout, NULL); // No printf flushing needed
	printf("\033[2J");	// Clear terminal
	printf("\033[H");	 // Move cursor to the home position
}

void setup_cache() {
	Xil_DCacheDisable();
}

void setup_interrupt() {
	printf("- Setting up interrupts: ");

	// This functions sets up the interrupt on the ARM
	int result;
	XScuGic_Config *pCfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (pCfg == NULL) {
		printf(
			"%sError: Interrupt Configuration Lookup Failed%s\n", KRED, KNRM);

		print("Interrupt Configuration Lookup Failed\n");
		exit(XST_FAILURE);
	}
	result = XScuGic_CfgInitialize(&ScuGic, pCfg, pCfg->CpuBaseAddress);
	if (result != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(result);
	}
	// self-test
	result = XScuGic_SelfTest(&ScuGic);
	if (result != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(result);
	}
	// Initialize the exception handler
	Xil_ExceptionInit();
	// Register the exception handler
	// print("Register the exception handler\n\r");
	Xil_ExceptionRegisterHandler(
		XIL_EXCEPTION_ID_INT,
		(Xil_ExceptionHandler) XScuGic_InterruptHandler,
		&ScuGic);
	// Enable the exception handler
	Xil_ExceptionEnable();
	printf("%sSuccess%s\n", KGRN, KNRM);
}

void setup_accelerator() {
	for (u32 i = 0; i < CONV_CORES_NUM; i++) {
		Conv_core_list[i] = (Core *) malloc(sizeof(Core));
		Conv_core_list[i]->InstanceId = i;
		Conv_core_init(Conv_core_list[i]);
	}
	for (u32 i = 0; i < MAXPOOL_CORES_NUM; i++) {
		Maxpool_core_list[i] = (Core *) malloc(sizeof(Core));
		Maxpool_core_list[i]->InstanceId = i;
		Maxpool_core_init(Maxpool_core_list[i]);
	}
	for (u32 i = 0; i < LINEAR_CORES_NUM; i++) {
		Linear_core_list[i] = (Core *) malloc(sizeof(Core));
		Linear_core_list[i]->InstanceId = i;
		Linear_core_init(Linear_core_list[i]);
	}
}

void setup_platform(char *greeting_message) {
	setup_stdout();
	printf("%s", greeting_message);
	setup_cache();
	setup_interrupt();
	setup_accelerator();
}

// ----------------------------------------------

void layer_conf_complete(matrix_t *xAddr) {
	u32 params_index = 0;
	for (u32 i = 0; i < LAYERS_NUMBER; i++) {
		switch (LAYERS_CONF[i].layerType) {
			case Conv:
				Conv_conf_complete(i, params_index, xAddr);
				params_index += 2;
				break;
			case Maxpool: Maxpool_conf_complete(i, xAddr); break;
			case LinearReLU:
			case Linear:
				Linear_conf_complete(i, params_index, xAddr);
				params_index += 2;
				break;
		}
	}
}

void process_layer(LayerConf lc) {
	switch (lc.layerType) {
		case Conv: Conv_core_process(lc); break;
		case Maxpool: Maxpool_core_process(lc); break;
		case LinearReLU:
		case Linear: Linear_core_process(lc); break;
	}
	free(lc.xAddr);
}

void forward(matrix_t *xAddr) {
	layer_conf_complete(xAddr);
	for (u32 i = 0; i < LAYERS_NUMBER; i++) {
		printf("- Layer %d\n", i);
		process_layer(LAYERS_CONF[i]);
	}
}

#endif /* SRC_PLATFORM_FUNC_H_ */
