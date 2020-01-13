#ifndef SRC_TEST_H_
#define SRC_TEST_H_

void test_Conv_core(){
//	XConv_core *Conv_Core = get_Conv_core();
	setbuf(stdout, NULL); // No printf flushing needed
	printf("\033[2J"); // Clear terminal
	printf("\033[H");  // Move cursor to the home position
	Xil_DCacheDisable();

	XConv_core Conv_Core;
	XConv_core_Config *Conv_Core_cfg;
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

	printf("\n*** Testing Conv Core ***\n");

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
	matrix_t *x = X_ADDR;
	for(u32 i = 0; i < X_SIZE; i++){
		x[i] = 1;
	}

	matrix_t *weights = WEIGHTS_ADDR;
	for(u32 i = 0; i < WEIGHTS_SIZE; i++){
		weights[i] = 2;
	}

	matrix_t *bias = BIAS_ADDR;
	for(u32 i = 0; i < BIAS_SIZE; i++){
		bias[i] = 1;
	}

	matrix_t *res = RES_ADDR;
	for(u32 i = 0; i < RES_SIZE; i++){
		res[i] = 0;
	}
	printf("Success\n");

	printf("- Setup: ");
	XConv_core_Set_x(&Conv_Core, (u32) (u64) &X_ADDR);
	XConv_core_Set_weights(&Conv_Core, (u32) (u64) &WEIGHTS_ADDR);
	XConv_core_Set_bias(&Conv_Core, (u32) (u64) &BIAS_ADDR);
	XConv_core_Set_din(&Conv_Core, DIN);
	XConv_core_Set_hin(&Conv_Core, HIN);
	XConv_core_Set_win(&Conv_Core, WIN);
	XConv_core_Set_dout(&Conv_Core, DOUT);
	XConv_core_Set_hout(&Conv_Core, HOUT);
	XConv_core_Set_wout(&Conv_Core, WOUT);
	XConv_core_Set_kernel_size(&Conv_Core, KERNEL_SIZE);
	XConv_core_Set_stride(&Conv_Core, STRIDE);
	XConv_core_Set_padding(&Conv_Core, PADDING);
	XConv_core_Set_res(&Conv_Core, (u32) (u64) &RES_ADDR);
	printf("Success\n");

	printf("- Starting: ");
	XConv_core_Start(&Conv_Core);
	printf("Success\n");

	printf("- Return value: ");
//	while (!XConv_core_IsDone(Conv_Core));
	u32 dat = XConv_core_IsDone(&Conv_Core);
	printf("dat: %u\n", dat);
	u32 status = XConv_core_Get_return(&Conv_Core);
	printf("%d\n", status);

	printf("- Validating: ");

	u32 flag = 1;
//	matrix_t *res = RES_ADDR;
//	matrix_t pixel_value = 1 * 2 * KERNEL_SIZE * KERNEL_SIZE * DIN + 1;
	for(u32 i = 0; i < RES_SIZE; i++) {
//		if (res[i] != pixel_value) {
		if (res[i] == 0) {
			flag = 0;
			break;
		}
	}
	printf("%s\n", flag ? "Success" : "Error");
}

void test_Conv_core_with_conf(){
//	XConv_core Conv_Core = get_Conv_core();
	XConv_core Conv_Core;

	printf("\n*** Testing Conv Core with conf ***\n");
	LayerConf lc = {
		.layerType = Conv,
		.kernelSize = 11,
		.stride = 4,
		.padding = 2,
		.din = 3,
		.hin = 224,
		.win = 224,
		.dout = 64,
	};
	lc.hout = (lc.hin + 2 * lc.padding - lc.kernelSize) / lc.stride + 1;
	lc.wout = (lc.win + 2 * lc.padding - lc.kernelSize) / lc.stride + 1;
	lc.xSize = lc.din * lc.hin * lc.win;
	lc.weightsSize = lc.dout * lc.din * lc.hout * lc.wout;
	lc.biasSize = lc.dout;
	lc.resSize = lc.dout * lc.hout * lc.wout;

	lc.xAddr = (matrix_t *) malloc(lc.xSize * sizeof(matrix_t));
	lc.weightsAddr = (matrix_t *) malloc(lc.weightsSize * sizeof(matrix_t));
	lc.biasAddr = (matrix_t *) malloc(lc.biasSize * sizeof(matrix_t));
	lc.resAddr = (matrix_t *) malloc(lc.resSize * sizeof(matrix_t));

	printf("- Initializing Memory: ");
	for(u32 i = 0; i < lc.xSize; i++){
		lc.xAddr[i] = 1;
	}

	for(u32 i = 0; i < lc.weightsSize; i++){
		lc.weightsAddr[i] = 2;
	}

	for(u32 i = 0; i < lc.biasSize; i++){
		lc.biasAddr[i] = 1;
	}
	printf("Success\n");

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

	printf("- Validating: ");
	u32 flag = 1;
	matrix_t pixel_value = 1 * 2 * lc.kernelSize * lc.kernelSize * lc.din + 1;
	for(u32 i = 0; i < lc.resSize; i++) {
		if (lc.resAddr[i] != pixel_value) {
			flag = 0;
			break;
		}
	}
	printf("%s\n", flag ? "Success" : "Error");
}

void test_Maxpool_core(){
	XMaxpool_core Maxpool_Core = get_Maxpool_core();

	printf("\n*** Testing Maxpool Core ***\n");

	u32 D = 64;
	u32 HIN = 55;
	u32 WIN = 55;
	u32 HOUT = 27;
	u32 WOUT = 27;
	u32 KERNEL_SIZE = 3;
	u32 STRIDE = 2;

	u32 X_SIZE = (D * HIN * WIN);
	u32 RES_SIZE = (D * HOUT * WOUT);

	u32 X_ADDR = BASE_ADDR;
	u32 RES_ADDR = X_ADDR + X_SIZE;

	printf("- Initializing Memory: ");
	matrix_t *x = (matrix_t*) (uint64_t) X_ADDR;
	for(u32 i = 0; i < X_SIZE; i++){
		x[i] = 1;
	}
	printf("Success\n");


	printf("- Setup: ");
	XMaxpool_core_Set_x(&Maxpool_Core, X_ADDR);
	XMaxpool_core_Set_d(&Maxpool_Core, D);
	XMaxpool_core_Set_hin(&Maxpool_Core, HIN);
	XMaxpool_core_Set_win(&Maxpool_Core, WIN);
	XMaxpool_core_Set_hout(&Maxpool_Core, HOUT);
	XMaxpool_core_Set_wout(&Maxpool_Core, WOUT);
	XMaxpool_core_Set_kernel_size(&Maxpool_Core, KERNEL_SIZE);
	XMaxpool_core_Set_stride(&Maxpool_Core, STRIDE);
	XMaxpool_core_Set_res(&Maxpool_Core, RES_ADDR);
	printf("Success\n");

	printf("- Starting: ");
	XMaxpool_core_Start(&Maxpool_Core);
	printf("Success\n");

	printf("- Return value: ");
	while (!XMaxpool_core_IsDone(&Maxpool_Core));
	u32 status = XMaxpool_core_Get_return(&Maxpool_Core);
	printf("%d\n", status);

	printf("- Validating: ");
	u32 flag = 1;
	matrix_t *res = (matrix_t *) (uint64_t) RES_ADDR;
	matrix_t pixel_value = 1;
	for(u32 i = 0; i < RES_SIZE; i++) {
		if (res[i] != pixel_value) {
			flag = 0;
			break;
		}
	}
	printf("%s\n", flag ? "Success" : "Error");
}

void test_Linear_core(){
	XLinear_core Linear_Core = get_Linear_core();

	printf("\n*** Testing Linear Core ***\n");

	u32 IN_FEATURES = 9216;
	u32 OUT_FEATURES = 4096;

	u32 X_SIZE = IN_FEATURES;
	u32 WEIGHTS_SIZE = IN_FEATURES * OUT_FEATURES;
	u32 BIAS_SIZE = OUT_FEATURES;
	u32 RES_SIZE = OUT_FEATURES;

	u32 X_ADDR = BASE_ADDR;
	u32 WEIGHTS_ADDR = X_ADDR + X_SIZE;
	u32 BIAS_ADDR = WEIGHTS_ADDR + WEIGHTS_SIZE;
	u32 RES_ADDR = BIAS_ADDR + BIAS_SIZE;

	printf("- Initializing Memory: ");
	matrix_t *x = (matrix_t*) (uint64_t) X_ADDR;
	for(u32 i = 0; i < X_SIZE; i++){
		x[i] = 1;
	}

	matrix_t *weights = (matrix_t*) (uint64_t) WEIGHTS_ADDR;
	for(u32 i = 0; i < WEIGHTS_SIZE; i++){
		weights[i] = 2;
	}

	matrix_t *bias = (matrix_t*) (uint64_t) BIAS_ADDR;
	for(u32 i = 0; i < BIAS_SIZE; i++){
		bias[i] = 1;
	}
	printf("Success\n");

	printf("- Setup: ");
	XLinear_core_Set_x(&Linear_Core, X_ADDR);
	XLinear_core_Set_weights(&Linear_Core, WEIGHTS_ADDR);
	XLinear_core_Set_bias(&Linear_Core, BIAS_ADDR);
	XLinear_core_Set_in_features(&Linear_Core, IN_FEATURES);
	XLinear_core_Set_out_features(&Linear_Core, OUT_FEATURES);
	XLinear_core_Set_doReLU(&Linear_Core, 1);
	XLinear_core_Set_res(&Linear_Core, RES_ADDR);
	printf("Success\n");

	printf("- Starting: ");
	XLinear_core_Start(&Linear_Core);
	printf("Success\n");

	printf("- Return value: ");
	while (!XLinear_core_IsDone(&Linear_Core));
	u32 status = XLinear_core_Get_return(&Linear_Core);
	printf("%d\n", status);

	printf("- Validating: ");
	u32 flag = 1;
	matrix_t *res = (matrix_t *) (uint64_t) RES_ADDR;
	matrix_t pixel_value = 1 * 2 * IN_FEATURES + 1;
	for(u32 i = 0; i < RES_SIZE; i++) {
		if (res[i] != pixel_value) {
			flag = 0;
			break;
		}
	}
	printf("%s\n", flag ? "Success" : "Error");
}

void runTests(){
	test_Conv_core();
	test_Conv_core_with_conf();
	test_Maxpool_core();
	test_Linear_core();
}

#endif /* SRC_TEST_H_ */
