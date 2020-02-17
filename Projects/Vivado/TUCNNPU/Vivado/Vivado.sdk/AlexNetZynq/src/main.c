#include <stdio.h>
#include "floatmatrix_utils.h"
#include "file_utils.h"
#include "nn_utils.h"

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
FloatMatrix *forward(Params *params, FloatMatrix *x) {
	x = Conv2dReLU(x, params->matrix[0], params->matrix[1], 3, 64, 11, 4, 2);
	x = MaxPool2d(x, 3, 2);
	x = Conv2dReLU(x, params->matrix[2], params->matrix[3], 64, 192, 5, 1, 2);
	x = MaxPool2d(x, 3, 2);
	x = Conv2dReLU(x, params->matrix[4], params->matrix[5], 192, 384, 3, 1, 1);
	x = Conv2dReLU(x, params->matrix[6], params->matrix[7], 384, 256, 3, 1, 1);
	x = Conv2dReLU(x, params->matrix[8], params->matrix[9], 256, 256, 3, 1, 1);
	x = MaxPool2d(x, 3, 2);
	x = Linear(x, params->matrix[10], params->matrix[11], 9216, 4096, 1);
	x = Linear(x, params->matrix[12], params->matrix[13], 4096, 4096, 1);
	x = Linear(x, params->matrix[14], params->matrix[15], 4096, 1000, 0);
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
int inference(char *path, Params *params, char **labels) {
	FloatMatrix *x = loadImage(path);
	/** Pass the image through the network */
	x = forward(params, x);

	/** Find the class with the greatest likelihood and print its label */
	unsigned int topClass = argmax(x);

	printf("- %s: %s\n", path, labels[topClass]);

	/**
	 * Free the forward pass's resulting FloatMatrix as it is no longer needed
	 * to avoid memory leaks
	 */
	freeFloatMatrix(x);
	return XST_SUCCESS;
}

int main() {
	setbuf(stdout, NULL);
	printf("\033[2J"); // Clear terminal
	printf("\033[H");  // Move cursor to the home position
	printf("- Starting...\n");

	mount_sd();
	read_config(CONFIG_FILE);
	Filelist *imagesPaths = getFileList(IMAGES_DIR);
	char **labels = loadLabels(LABELS_FILE);
	Params *params = params = loadParameters(PARAMS_FILE);

	printf("\n-----------------------\n");
	printf("- Starting Inference...\n\n");
	for (unsigned int i = 0; i < imagesPaths->length; i++) {
		inference(imagesPaths->list[i], params, labels);
	}
	printf("\n- Inference finished successfully!\n");
	printf("-----------------------\n\n");
	return XST_SUCCESS;
}
