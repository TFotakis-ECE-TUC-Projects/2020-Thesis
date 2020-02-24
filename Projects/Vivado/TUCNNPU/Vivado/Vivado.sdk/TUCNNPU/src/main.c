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

#define USE_SELF_CHECK 1
#define TEST_ALL_CORES 1
#define RUN_FOR_NUM_IMAGES 5

void runTests() {
	printf("\n");
	Conv_core_test(TEST_ALL_CORES);
	printf("\n");
	Maxpool_core_test(TEST_ALL_CORES);
	printf("\n");
	Linear_core_test(TEST_ALL_CORES);
	printf("\n");
	Network_test();
}

void setup() {
	setup_platform("*** Starting ***\n");
	mount_sd();

	runTests();
}

void loop() {
	while (1) {
		NetConf *netConf =
			selectNetConf(CONFIGS_DIR, LABELS_DIR, PARAMS_DIR, IMAGES_DIR);

		printf("*** Starting Inference ***\n\n");
		inference(netConf, RUN_FOR_NUM_IMAGES, USE_SELF_CHECK);
		printf("\n*** Inference finished ***\n");

		freeNetConf(netConf);
		break;
	}
}

int main() {
	setup();
	loop();
	return XST_SUCCESS;
}
