#include <stdio.h>
#include "platform_conf.h"
#include "floatmatrix_utils.h"
#include "file_utils.h"
#include "nn_utils.h"
#include "platform_func.h"

#define IMAGES_DIR "tzanis/images"
#define CONFIGS_DIR "tzanis/configs"
#define PARAMS_DIR "tzanis/params"
#define LABELS_DIR "tzanis/labels"

#define CONFIG_FILE "tzanis/configs/alexnet.conf"
#define PARAMS_FILE "tzanis/params/alexnet.params"
#define LABELS_FILE "tzanis/labels/alexnet.labels"

/**
 * Runs the network's forward pass
 * @param[in] params: the network's parameters sets
 * @param[in] x: the input image to get classified
 * @returns a FloatMatrix containing the network's classification estimates,
 * that is an array of matrix_t numbers, one for each class, which shows how
 * likely it is for the input image to be of a class in a logarithmic scale.
 */
FloatMatrix *forwardZynq(NetConf *netConf, FloatMatrix *x) {
	// x = Conv2dReLU(x, params->matrix[0], params->matrix[1], 3, 64, 11, 4, 2);
	// x = MaxPool2d(x, 3, 2);
	// x = Conv2dReLU(x, params->matrix[2], params->matrix[3], 64, 192, 5, 1,
	// 2); x = MaxPool2d(x, 3, 2); x = Conv2dReLU(x, params->matrix[4],
	// params->matrix[5], 192, 384, 3, 1, 1); x = Conv2dReLU(x,
	// params->matrix[6], params->matrix[7], 384, 256, 3, 1, 1); x =
	// Conv2dReLU(x, params->matrix[8], params->matrix[9], 256, 256, 3, 1, 1);
	// x = MaxPool2d(x, 3, 2);
	// x = Linear(x, params->matrix[10], params->matrix[11], 9216, 4096, 1);
	// x = Linear(x, params->matrix[12], params->matrix[13], 4096, 4096, 1);
	// x = Linear(x, params->matrix[14], params->matrix[15], 4096, 1000, 0);

	printf("00%%");
	unsigned int params_index = 0;
	for (unsigned int i = 0; i < netConf->layersNum; i++) {
		printf("\b\b\b");
		printf("%02d%%", (i * 100) / netConf->layersNum);

		switch (netConf->layersConf[i].layerType) {
			case CONV_LAYER_TYPE:
				printf("Conv");
				x = Conv2dReLU(
					x,
					netConf->params->matrix[params_index],
					netConf->params->matrix[params_index + 1],
					netConf->layersConf[i].din,
					netConf->layersConf[i].dout,
					netConf->layersConf[i].kernelSize,
					netConf->layersConf[i].stride,
					netConf->layersConf[i].padding);
				params_index += 2;
				break;
			case MAXPOOL_LAYER_TYPE:
				printf("maxpool");
				x = MaxPool2d(
					x,
					netConf->layersConf[i].kernelSize,
					netConf->layersConf[i].stride);
				break;
			case LINEAR_RELU_LAYER_TYPE:
			case LINEAR_LAYER_TYPE:
				printf("Linear");
				x = Linear(
					x,
					netConf->params->matrix[params_index],
					netConf->params->matrix[params_index + 1],
					netConf->layersConf[i].inFeatures,
					netConf->layersConf[i].outFeatures,
					netConf->layersConf[i].doReLU);
				params_index += 2;
				break;
		}
	}
	printf("\b\b\b");
	return x;
}

/**
 * Takes an image and passes it through the network to classify it and print its
 * label. It also measures the time in milliseconds for the forward pass to
 * complete.
 * @param[in] path: The image's path to pass the network
 * @param[in] params: The network's parameters
 * @param[in] labels: The network's classes' labels
 * @returns the time needed in milliseconds for the forward pass to complete.
 */
int inference(char *path, NetConf *netConf) {
	FloatMatrix *x = loadImage(path);
	printf("- %s: ", path);

	/** Pass the image through the network */
	x = forwardZynq(netConf, x);

	/** Find the class with the greatest likelihood and print its label */
	unsigned int topClass = argmax(x);

	printf("%s\n", netConf->labels[topClass]);

	/**
	 * Free the forward pass's resulting FloatMatrix as it is no longer needed
	 * to avoid memory leaks
	 */
	freeFloatMatrix(x);
	return XST_SUCCESS;
}

void setup() {
	setup_stdout();
	printf("*** Starting ***\n");
	setup_cache();
	mount_sd();
}

void loop() {
	while (1) {
		NetConf *netConf = read_config(CONFIG_FILE);
		netConf->imagesPaths = getFileList(IMAGES_DIR);
		netConf->labels = loadLabels(LABELS_FILE);
		netConf->params = loadParameters(PARAMS_FILE);

		printf("*** Starting Inference ***\n\n");
		for (unsigned int i = 0; i < netConf->imagesPaths->length; i++) {
			inference(
				netConf->imagesPaths->list[i],
				netConf);
		}
		printf("\n*** Inference finished ***\n");
		break;
	}
}

int main() {
	setup();
	loop();
	return XST_SUCCESS;
}
