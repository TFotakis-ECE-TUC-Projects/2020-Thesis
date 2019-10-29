#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "ff.h"
#include "xsdps.h"
#include "floatMatrix-tools.h"
#include "terminalColors.h"
#include "file-utils.h"


#define PARAMS_FILE "tzanis/alexnet.bin"


int main() {
    init_platform();

    setbuf(stdout, NULL);
    printf("\033[2J"); // Clear terminal
    printf("\033[H");  // Move cursor to the home position
    printf("- Starting...\n");

    int res = mount_sd();
    if (res) return XST_FAILURE;
    Params *params = loadParameters(PARAMS_FILE);
    if(!params) return XST_FAILURE;

	printf("- Cleaning Up...\n");
	cleanup_platform();
    return XST_SUCCESS;
}
