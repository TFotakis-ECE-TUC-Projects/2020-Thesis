#include <stdio.h>
#include <stdlib.h>
#include "terminal_colors.h"

#include "platform_conf.h"
#include "testnet_conf.h"
#include "platform_func.h"

int Conv_core_single_test() {
	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

	printf("*** Testing Conv_core ***\n");

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

	Conv_conf_params_complete(&lc);

	printf("- Initializing Memory: ");
	lc.xAddr = (matrix_t *) malloc(lc.xSize * sizeof(matrix_t));
	lc.weightsAddr = (matrix_t *) malloc(lc.weightsSize * sizeof(matrix_t));
	lc.biasAddr = (matrix_t *) malloc(lc.biasSize * sizeof(matrix_t));
	lc.resAddr = (matrix_t *) malloc(lc.resSize * sizeof(matrix_t));

	for (u32 i = 0; i < lc.xSize; i++) { lc.xAddr[i] = X_VALUE; }
	for (u32 i = 0; i < lc.weightsSize; i++) {
		lc.weightsAddr[i] = WEIGHT_VALUE;
	}
	for (u32 i = 0; i < lc.biasSize; i++) { lc.biasAddr[i] = BIAS_VALUE; }
	for (u32 i = 0; i < lc.resSize; i++) { lc.resAddr[i] = 0; }
	printf("%sSuccess%s\n", KGRN, KNRM);

	process_layer(lc);

	printf("- Validating: ");
	matrix_t pixel_value;
	matrix_t error = 0;

	pixel_value =
		X_VALUE * WEIGHT_VALUE * lc.kernelSize * lc.kernelSize * lc.din +
		BIAS_VALUE;
	error += abs(
		lc.resAddr[0 * lc.hout * lc.wout + 23 * lc.wout + 23] - pixel_value);
	error += abs(
		lc.resAddr[1 * lc.hout * lc.wout + 23 * lc.wout + 23] - pixel_value);
	pixel_value = X_VALUE * WEIGHT_VALUE * (lc.kernelSize - lc.padding) *
			(lc.kernelSize - lc.padding) * lc.din +
		BIAS_VALUE;
	error += abs(lc.resAddr[0] - pixel_value);
	pixel_value = X_VALUE * WEIGHT_VALUE * (lc.kernelSize - lc.padding + 1) *
			(lc.kernelSize - lc.padding + 1) * lc.din +
		BIAS_VALUE;
	error += abs(
		lc.resAddr[0 * lc.hout * lc.wout + 54 * lc.wout + 54] - pixel_value);

	int status;
	if (error < 0.1) {
		printf("%sSuccess%s\n", KGRN, KNRM);
		status = XST_SUCCESS;
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
		status = XST_FAILURE;
	}

	free(lc.weightsAddr);
	free(lc.biasAddr);
	free(lc.resAddr);

	return status;
}

int Maxpool_core_single_test() {
	matrix_t X_VALUE = .1;

	printf("*** Testing Maxpool_core ***\n");

	LayerConf lc = {
		.layerType = Maxpool,
		.kernelSize = 3,
		.stride = 2,
		.din = 64,
		.hin = 55,
		.win = 55,
	};

	Maxpool_conf_params_complete(&lc);

	printf("- Initializing Memory: ");
	lc.xAddr = (matrix_t *) malloc(lc.xSize * sizeof(matrix_t));
	lc.resAddr = (matrix_t *) malloc(lc.resSize * sizeof(matrix_t));

	for (u32 i = 0; i < lc.xSize; i++) { lc.xAddr[i] = X_VALUE; }
	for (u32 i = 0; i < lc.resSize; i++) { lc.resAddr[i] = 0; }
	printf("%sSuccess%s\n", KGRN, KNRM);

	process_layer(lc);

	printf("- Validating: ");
	matrix_t *res = (matrix_t *) (uint64_t) lc.resAddr;
	matrix_t pixel_value = X_VALUE;
	matrix_t error = 0;

	for (u32 i = 0; i < lc.resSize; i++) { error += abs(res[i] - pixel_value); }

	int status;
	if (error < 0.1) {
		printf("%sSuccess%s\n", KGRN, KNRM);
		status = XST_SUCCESS;
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
		status = XST_FAILURE;
	}

	free(lc.resAddr);

	return status;
}

int Linear_core_single_test() {
	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

	printf("*** Testing Linear_core ***\n");

	LayerConf lc = {
		.layerType = LinearReLU,
		.inFeatures = 9216,
		.outFeatures = 4096,
	};

	Linear_conf_params_complete(&lc);

	printf("- Initializing Memory: ");
	lc.xAddr = (matrix_t *) malloc(lc.xSize * sizeof(matrix_t));
	lc.weightsAddr = (matrix_t *) malloc(lc.weightsSize * sizeof(matrix_t));
	lc.biasAddr = (matrix_t *) malloc(lc.biasSize * sizeof(matrix_t));
	lc.resAddr = (matrix_t *) malloc(lc.resSize * sizeof(matrix_t));

	for (u32 i = 0; i < lc.xSize; i++) { lc.xAddr[i] = X_VALUE; }
	for (u32 i = 0; i < lc.weightsSize; i++) {
		lc.weightsAddr[i] = WEIGHT_VALUE;
	}
	for (u32 i = 0; i < lc.biasSize; i++) { lc.biasAddr[i] = BIAS_VALUE; }
	for (u32 i = 0; i < lc.resSize; i++) { lc.resAddr[i] = 0; }
	printf("%sSuccess%s\n", KGRN, KNRM);

	process_layer(lc);

	printf("- Validating: ");
	matrix_t *res = (matrix_t *) (uint64_t) lc.resAddr;
	matrix_t pixel_value = X_VALUE * WEIGHT_VALUE * lc.inFeatures + BIAS_VALUE;
	matrix_t error = 0;

	for (u32 i = 0; i < lc.resSize; i++) { error += abs(res[i] - pixel_value); }

	int status;
	if (error < 0.1) {
		printf("%sSuccess%s\n", KGRN, KNRM);
		status = XST_SUCCESS;
	} else {
		printf("%sError %lf%s\n", KRED, error, KNRM);
		status = XST_FAILURE;
	}

	free(lc.weightsAddr);
	free(lc.biasAddr);
	free(lc.resAddr);

	return status;
}

int Network_single_test() {
	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

	printf("*** Testing Single Network ***\n");

	printf("- Initializing Memory: ");

	layer_conf_complete(NULL);

	u32 xSize = LAYERS_CONF[0].xSize;
	matrix_t *xAddr = (matrix_t *) malloc(xSize * sizeof(matrix_t));
	for (u32 i = 0; i < xSize; i++) { xAddr[i] = X_VALUE; }

	u32 params_index = 0;
	for (u32 i = 0; i < LAYERS_NUMBER; i++) {
		if (LAYERS_CONF[i].layerType == Maxpool) continue;
		PARAMS_ADDR[params_index] =
			(matrix_t *) malloc(LAYERS_CONF[i].weightsSize * sizeof(matrix_t));
		for (u32 j = 0; j < LAYERS_CONF[i].weightsSize; j++) {
			PARAMS_ADDR[params_index][j] = WEIGHT_VALUE;
		}
		params_index++;

		PARAMS_ADDR[params_index] =
			(matrix_t *) malloc(LAYERS_CONF[i].biasSize * sizeof(matrix_t));
		for (u32 j = 0; j < LAYERS_CONF[i].biasSize; j++) {
			PARAMS_ADDR[params_index][j] = BIAS_VALUE;
		}
		params_index++;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Forwarding...\n");
	forward(xAddr);
	printf("- %sForwarded Successfully%s\n", KGRN, KNRM);

	printf("- Freeing PARAMS_ADDR: ");
	for (u32 i = 0; i < params_index; i++) {
		free(PARAMS_ADDR[i]);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf(" - Freeing last resAddr: ");
	free(LAYERS_CONF[LAYERS_NUMBER - 1].resAddr);
	printf("%sSuccess%s\n", KGRN, KNRM);

	return XST_SUCCESS;
}

int run_tests() {
	int status;
	status = Conv_core_single_test();
	if (status != XST_SUCCESS) return status;
	printf("\n");
	status = Maxpool_core_single_test();
	if (status != XST_SUCCESS) return status;
	printf("\n");
	status = Linear_core_single_test();
	if (status != XST_SUCCESS) return status;
	printf("\n");
	status = Network_single_test();
	if (status != XST_SUCCESS) return status;

	return XST_SUCCESS;
}

void setup() {
	setup_platform("*** AlexNetCore_Test_Int ***\n");
}

void loop() {
	printf("\n");
	u32 runCount = 0;
	int status;
	do {
		printf("%s*** Test run %u ***%s\n", KYEL, ++runCount, KNRM);
		status = run_tests();
		printf("\n");
	} while (status == XST_SUCCESS);
}

int main() {
	setup();
	loop();
	printf("- Exiting!\n");
	return 0;
}
