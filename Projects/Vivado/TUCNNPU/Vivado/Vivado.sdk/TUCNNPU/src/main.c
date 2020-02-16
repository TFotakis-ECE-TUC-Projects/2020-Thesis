#include <stdio.h>
#include <stdlib.h>
#include "terminal_colors.h"

#include "platform_conf.h"
#include "testnet_conf.h"
#include "platform_func.h"
#include "test.h"


void setup() {
	setup_platform("*** AlexNetCore_Test_Int ***\n");
}

void loop() {
	printf("\n");
	int status;
	do {
		status = XST_FAILURE;
	} while (status == XST_SUCCESS);
}

int main() {
	setup();
	run_tests();
	loop();
	printf("- Exiting!\n");
	return 0;
}
