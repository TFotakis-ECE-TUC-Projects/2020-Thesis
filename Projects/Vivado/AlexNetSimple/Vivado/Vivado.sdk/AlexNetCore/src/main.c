#include <stdlib.h>
#include <stdio.h>
#include <xil_cache.h>

#include "platform_conf.h"
#include "alexnet_conf.h"
#include "platform_func.h"
#include "test.h"

void setup(){
	setbuf(stdout, NULL); // No printf flushing needed
	printf("\033[2J"); // Clear terminal
	printf("\033[H");  // Move cursor to the home position
	Xil_DCacheDisable();

	setup_accelerator();
}

int main() {
	setup();
	runTests();
	printf("\n*** All Done ***\n");
	return 0;
}
