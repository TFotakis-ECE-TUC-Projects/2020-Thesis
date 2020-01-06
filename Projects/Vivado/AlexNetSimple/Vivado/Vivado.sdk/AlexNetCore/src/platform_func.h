#ifndef SRC_PLATFORM_FUNC_H_
#define SRC_PLATFORM_FUNC_H_

matrix_t *PARAMS_ADDR[LAYERS_NUMBER * 2];

void init_Conv_core(XConv_core Conv_Core, XConv_core_Config *Conv_Core_cfg) {
	printf("- Initializing Core: ");
	Conv_Core_cfg = XConv_core_LookupConfig(XPAR_CONV_CORE_0_DEVICE_ID);
	if (Conv_Core_cfg) {
		int status = XConv_core_CfgInitialize(&Conv_Core, Conv_Core_cfg);
		if (status != XST_SUCCESS) {
			printf("Error\n");
			exit(-1);
		}
	}
	printf("Success\n");
}

void init_Maxpool_core(XMaxpool_core Maxpool_Core, XMaxpool_core_Config *Maxpool_Core_cfg) {
	printf("- Initializing Core: ");
	Maxpool_Core_cfg = XMaxpool_core_LookupConfig(XPAR_MAXPOOL_CORE_0_DEVICE_ID);
	if (Maxpool_Core_cfg) {
		int status = XMaxpool_core_CfgInitialize(&Maxpool_Core, Maxpool_Core_cfg);
		if (status != XST_SUCCESS) {
			printf("Error\n");
			exit(-1);
		}
	}
	printf("Success\n");
}

void init_Linear_core(XLinear_core Linear_Core, XLinear_core_Config *Linear_Core_cfg) {
	printf("- Initializing Core: ");
	Linear_Core_cfg = XLinear_core_LookupConfig(XPAR_LINEAR_CORE_0_DEVICE_ID);
	if (Linear_Core_cfg) {
		int status = XLinear_core_CfgInitialize(&Linear_Core, Linear_Core_cfg);
		if (status != XST_SUCCESS) {
			printf("Error\n");
			exit(-1);
		}
	}
	printf("Success\n");
}

void setup_accelerator() {
	for(u32 i = 0; i < CONV_CORES_NUM; i++){
		init_Conv_core(Conv_core_list[i], Conv_core_cfg_list[i]);
	}
	for(u32 i = 0; i < MAXPOOL_CORES_NUM; i++){
		init_Maxpool_core(Maxpool_core_list[i], Maxpool_core_cfg_list[i]);
	}
	for(u32 i = 0; i < LINEAR_CORES_NUM; i++){
		init_Linear_core(Linear_core_list[i], Linear_core_cfg_list[i]);
	}
}

void complete_Conv_conf(u32 layer_index, u32 params_index, matrix_t *xAddr) {
	LayerConf *lc = &LAYERS_CONF[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &LAYERS_CONF[layer_index - 1];
		if(plc->layerType == LinearReLU || plc->layerType == Linear) {
			printf("Error. Linear layer cannot be followed by Conv layer.\n");
			exit(-1);
		}
		lc->din = plc->dout;
		lc->hin = plc->hout;
		lc->win = plc->wout;
		lc->xAddr = plc->resAddr;
	} else {
		lc->xAddr = xAddr;
	}
	lc->hout = (lc->hin + 2 * lc->padding - lc->kernelSize) / lc->stride + 1;
	lc->wout = (lc->win + 2 * lc->padding - lc->kernelSize) / lc->stride + 1;
	lc->xSize = lc->din * lc->hin * lc->win;
	lc->weightsSize = lc->dout * lc->din * lc->hout * lc->wout;
	lc->biasSize = lc->dout;
	lc->resSize = lc->dout * lc->hout * lc->wout;

	lc->weightsAddr = PARAMS_ADDR[params_index];
	lc->biasAddr = PARAMS_ADDR[params_index + 1];
	lc->resAddr = (matrix_t *) malloc(lc->resSize * sizeof(matrix_t));
}

void complete_Maxpool_conf(u32 layer_index, matrix_t *xAddr) {
	LayerConf *lc = &LAYERS_CONF[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &LAYERS_CONF[layer_index - 1];
		if(plc->layerType == LinearReLU || plc->layerType == Linear) {
			printf("Error. Linear layer cannot be followed by MaxPool layer.\n");
			exit(-1);
		}
		lc->din = plc->dout;
		lc->hin = plc->hout;
		lc->win = plc->wout;
		lc->xAddr = plc->resAddr;
	} else {
		lc->xAddr = xAddr;
	}
	lc->dout = lc->din;
	lc->hout = (lc->hin - lc->kernelSize) / lc->stride + 1;
	lc->wout = (lc->win - lc->kernelSize) / lc->stride + 1;
	lc->xSize = lc->din * lc->hin * lc->win;
	lc->resSize = lc->dout * lc->hout * lc->wout;

	lc->resAddr = (matrix_t *) malloc(lc->resSize * sizeof(matrix_t));
}

void complete_Linear_conf(u32 layer_index, u32 params_index, matrix_t *xAddr) {
	LayerConf *lc = &LAYERS_CONF[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &LAYERS_CONF[layer_index - 1];
		switch(plc->layerType) {
		case Conv:
		case MaxPool:
			lc->inFeatures = plc->dout * plc->hout	* plc->wout;
			break;
		case LinearReLU:
		case Linear:
			lc->inFeatures = plc->outFeatures;
			break;
		}
		lc->xAddr = plc->resAddr;
	} else {
		lc->xAddr = xAddr;
	}
	lc->xSize = lc->inFeatures;
	lc->weightsSize = lc->inFeatures * lc->outFeatures;
	lc->biasSize = lc->outFeatures;
	lc->resSize = lc->outFeatures;
	lc->doReLU = 0;

	lc->weightsAddr = PARAMS_ADDR[params_index];
	lc->biasAddr = PARAMS_ADDR[params_index + 1];
	lc->resAddr = (matrix_t *) malloc(lc->resSize * sizeof(matrix_t));
}

void complete_LinearReLU_conf(u32 layer_index, u32 params_index, matrix_t *xAddr) {
	complete_Linear_conf(layer_index, params_index, xAddr);
	LayerConf *lc = &LAYERS_CONF[layer_index];
	lc->doReLU = 1;
}

void complete_layer_conf(matrix_t *xAddr) {
	u32 params_index = 0;
	for(u32 i = 0; i < LAYERS_NUMBER; i++) {
		switch(LAYERS_CONF[i].layerType) {
		case Conv:
			complete_Conv_conf(i, params_index, xAddr);
			params_index += 2;
			break;
		case MaxPool:
			complete_Maxpool_conf(i, xAddr);
			break;
		case LinearReLU:
			complete_LinearReLU_conf(i, params_index, xAddr);
			params_index += 2;
			break;
		case Linear:
			complete_Linear_conf(i, params_index, xAddr);
			params_index += 2;
			break;
		}
	}
}

XConv_core get_Conv_core() {
	return Conv_core_list[0];
}

XMaxpool_core get_Maxpool_core() {
	return Maxpool_core_list[0];
}

XLinear_core get_Linear_core() {
	return Linear_core_list[0];
}

void process_Conv(LayerConf lc) {
	XConv_core Conv_Core = get_Conv_core();
	printf("- Setup: ");
	XConv_core_Set_x(&Conv_Core, (u32) (u64) &lc.xAddr);
	XConv_core_Set_weights(&Conv_Core, (u32) (u64) &lc.weightsAddr);
	XConv_core_Set_bias(&Conv_Core, (u32) (u64) &lc.biasAddr);
	XConv_core_Set_din(&Conv_Core, lc.din);
	XConv_core_Set_hin(&Conv_Core, lc.hin);
	XConv_core_Set_win(&Conv_Core, lc.win);
	XConv_core_Set_dout(&Conv_Core, lc.dout);
	XConv_core_Set_hout(&Conv_Core, lc.hout);
	XConv_core_Set_wout(&Conv_Core, lc.wout);
	XConv_core_Set_kernel_size(&Conv_Core, lc.kernelSize);
	XConv_core_Set_stride(&Conv_Core, lc.stride);
	XConv_core_Set_padding(&Conv_Core, lc.padding);
	XConv_core_Set_res(&Conv_Core, (u32) (u64) &lc.resAddr);
	printf("Success\n");

	printf("- Starting: ");
	XConv_core_Start(&Conv_Core);
	printf("Success\n");

	printf("- Return value: ");
	while (!XConv_core_IsDone(&Conv_Core));
	u32 status = XConv_core_Get_return(&Conv_Core);
	printf("%d\n", status);
}

void process_Maxpool(LayerConf lc) {
	XMaxpool_core Maxpool_Core = get_Maxpool_core();
	printf("- Setup: ");
	XMaxpool_core_Set_x(&Maxpool_Core, (u32) (u64) &lc.xAddr);
	XMaxpool_core_Set_d(&Maxpool_Core, lc.din);
	XMaxpool_core_Set_hin(&Maxpool_Core, lc.hin);
	XMaxpool_core_Set_win(&Maxpool_Core, lc.win);
	XMaxpool_core_Set_hout(&Maxpool_Core, lc.hout);
	XMaxpool_core_Set_wout(&Maxpool_Core, lc.wout);
	XMaxpool_core_Set_kernel_size(&Maxpool_Core, lc.kernelSize);
	XMaxpool_core_Set_stride(&Maxpool_Core, lc.stride);
	XMaxpool_core_Set_res(&Maxpool_Core, (u32) (u64) &lc.resAddr);
	printf("Success\n");

	printf("- Starting: ");
	XMaxpool_core_Start(&Maxpool_Core);
	printf("Success\n");

	printf("- Return value: ");
	while (!XMaxpool_core_IsDone(&Maxpool_Core));
	u32 status = XMaxpool_core_Get_return(&Maxpool_Core);
	printf("%d\n", status);
}

void process_Linear(LayerConf lc) {
	XLinear_core Linear_Core = get_Linear_core();
	printf("- Setup: ");
	XLinear_core_Set_x(&Linear_Core, (u32) (u64) &lc.xAddr);
	XLinear_core_Set_weights(&Linear_Core, (u32) (u64) &lc.weightsAddr);
	XLinear_core_Set_bias(&Linear_Core, (u32) (u64) &lc.biasAddr);
	XLinear_core_Set_in_features(&Linear_Core, lc.inFeatures);
	XLinear_core_Set_out_features(&Linear_Core, lc.outFeatures);
	XLinear_core_Set_doReLU(&Linear_Core, lc.doReLU);
	XLinear_core_Set_res(&Linear_Core, (u32) (u64) &lc.resAddr);
	printf("Success\n");

	printf("- Starting: ");
	XLinear_core_Start(&Linear_Core);
	printf("Success\n");

	printf("- Return value: ");
	while (!XLinear_core_IsDone(&Linear_Core));
	u32 status = XLinear_core_Get_return(&Linear_Core);
	printf("%d\n", status);
}

void process_layer(LayerConf lc) {
	switch(lc.layerType) {
	case Conv:
		process_Conv(lc);
		break;
	case MaxPool:
		process_Maxpool(lc);
		break;
	case LinearReLU:
	case Linear:
		process_Linear(lc);
		break;
	}
	free(lc.xAddr);
}

void forward(matrix_t *xAddr) {
	complete_layer_conf(xAddr);
	for(u32 i = 0; i < LAYERS_NUMBER; i++) {
		process_layer(LAYERS_CONF[i]);
	}
}

#endif /* SRC_PLATFORM_FUNC_H_ */
