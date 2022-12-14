#include <stdio.h>
#include <xstatus.h>
#include "platform.h"

#define IMAGES_DIR "tzanis/images"
#define CONFIGS_DIR "tzanis/configs"
#define PARAMS_DIR "tzanis/params"
#define LABELS_DIR "tzanis/labels"
#define RUN_FOR_NUM_IMAGES 5

void runTests() {
	if (!askNoDefault("- Run tests?")) return;

	u32 testAllCores = askNoDefault("- Test all core instances?");
	Conv_core_test(testAllCores);
	printf("\n");
	Maxpool_core_test(testAllCores);
	printf("\n");
	Linear_core_test(testAllCores);
	printf("\n");
	Network_test();
}

void setup() {
	setup_platform("*** Starting ***\n");
	mount_sd();
	clearTerminal();

	runTests();
	clearTerminal();
}

void loop() {
	while (1) {
		NetConf *netConf =
			selectNetConf(CONFIGS_DIR, LABELS_DIR, PARAMS_DIR, IMAGES_DIR);
		printf("\n");
		u32 useSelfCheck = askNoDefault("- Use self check?");
		u32 runForNumImages = askNumber(
			"- Forward N images",
			1,
			netConf->imagesPaths->length,
			netConf->imagesPaths->length);

		printf("\n");
		printf("*** Starting Inference ***\n");
		inference(netConf, runForNumImages, useSelfCheck);
		printf("*** Inference finished ***\n");

		freeNetConf(netConf);
	}
}

int main() {
	setup();
	loop();
	cleanup_platform();
	return XST_SUCCESS;
}
