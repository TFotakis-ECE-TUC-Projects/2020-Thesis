#include "platform.hpp"

#define IMAGES_DIR "tzanis/images"
#define CONFIGS_DIR "tzanis/configs"
#define PARAMS_DIR "tzanis/params"
#define LABELS_DIR "tzanis/labels"
#define RUN_FOR_NUM_IMAGES 5

void runTests() {
	if (!askNoDefault("- Run tests?")) return;

	u32 testAllCores = askNoDefault("- Test all core instances?");
	while(1){
		if(askNoDefault("- Test conv?")){
			Conv_core_test(testAllCores);
			printf("\n");
		}
		if(askNoDefault("- Test MaxPool?")){
			Maxpool_core_test(testAllCores);
			printf("\n");
		}
		if(askNoDefault("- Test Linear?")){
			Linear_core_test(testAllCores);
			printf("\n");
		}
		if(askNoDefault("- Test Network?")){
			Network_test();
			printf("\n");
		}
		if(!askNoDefault("- Test Again?")){
			break;
		}
	}
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
