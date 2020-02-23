#include <stdio.h>
#include <xstatus.h>
#include "platform.h"

#define IMAGES_DIR "tzanis/images"
#define CONFIGS_DIR "tzanis/configs"
#define PARAMS_DIR "tzanis/params"
#define LABELS_DIR "tzanis/labels"

#define CONFIG_FILE "tzanis/configs/alexnet.conf"
#define PARAMS_FILE "tzanis/params/alexnet.params"
#define LABELS_FILE "tzanis/labels/alexnet.labels"

void runTests() {
	Conv_core_test(1);
	Maxpool_core_test(1);
	Linear_core_test(1);
	Network_test();
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
	matrix_t *x = loadImage(path);
	printf("- %s: ", path);

	/** Pass the image through the network */
	matrix_t *x_hw = forward(netConf, x, 1);
	matrix_t *x_sw = forward(netConf, x, 0);

	/** Find the class with the greatest likelihood and print its label */
	unsigned int xSize = netConf->layersConf[netConf->layersNum - 1].resSize;
	unsigned int topClass_hw = argmax(x_hw, xSize);
	unsigned int topClass_sw = argmax(x_sw, xSize);

	if (topClass_hw == topClass_sw)
		printf("%s\n", netConf->labels[topClass_sw]);
	else
		printf("%sError. No match between sw and hw.%s\n", KRED, KNRM);

	/**
	 * Free the forward pass's resulting FloatMatrix as it is no longer needed
	 * to avoid memory leaks
	 */
	free(x);
	free(x_hw);
	free(x_sw);
	return XST_SUCCESS;
}

void setup() {
	setup_stdout();
	printf("*** Starting ***\n");
	setup_cache();
	mount_sd();

	runTests();
}

void loop() {
	while (1) {
		NetConf *netConf = read_config(CONFIG_FILE);
		netConf->imagesPaths = getFileList(IMAGES_DIR);
		netConf->labels = loadLabels(LABELS_FILE);
		netConf->params = loadParameters(PARAMS_FILE);

		printf("*** Starting Inference ***\n\n");
		for (unsigned int i = 0; i < netConf->imagesPaths->length; i++) {
			inference(netConf->imagesPaths->list[i], netConf);
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
