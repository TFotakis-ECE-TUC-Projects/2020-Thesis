/*
 * file-utils.h
 *
 *  Created on: Oct 28, 2019
 *      Author: tzanis
 */

#ifndef SRC_FILE_UTILS_H_
#define SRC_FILE_UTILS_H_


/** Structure to store the network's parameters */
typedef struct params_t {
	uint len; /**< The number of parameter matrices */
	FloatMatrix **matrix; /**< The 1D array of FloatMartix cells to store the parameters */
} Params;


uint readUInt(FIL *f){
	int num;
	uint bytes_read;
	FRESULT fRes = f_read(f, &num, 4, &bytes_read);
	if(fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED,  fRes, KNRM);
		return -1;
	}
	return num;
}


float readFloat(FIL *f){
	float num;
	uint bytes_read;
	FRESULT fRes = f_read(f, &num, 4, &bytes_read);
	if(fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED,  fRes, KNRM);
		return -1;
	}
	return num;
}


/**
 * Loads the network's parameters using the file defined on the parametersPath
 * global variable.
 * @returns a Params structure pointer containing the loaded network parameters.
 */
Params *loadParameters(char *filename) {
	/** Open the parameters file */
	FRESULT fRes;
	FIL f;

	printf("- Opening File \"%s\": ", filename);
	if((fRes = f_open(&f, filename, FA_OPEN_EXISTING | FA_READ)) != FR_OK){
		printf("%sError %d.%s\n", KRED,  fRes, KNRM);
		return 0;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Loading parameters: 00%%");
	/** Allocate the needed memory to store the Params structure */
	Params *params = (Params *) malloc(sizeof(Params));

	/** Get the number of parameters sets existing in this file */
	params->len = readUInt(&f);

	/**
	 * Allocate the needed memory to store all the FloatMatrix structures
	 * (one for each parameters set)
	 */
	params->matrix = (FloatMatrix **) malloc(params->len * sizeof(FloatMatrix *));

	/** For every parameters set */
	FloatMatrix *matrix;
	for (int p = 0; p < params->len; p++) {
		/** Allocate the needed memory to store the FloatMatrix structure */
		matrix = (FloatMatrix *) malloc(sizeof(FloatMatrix));

		/** Get the number of dimensions this parameters set consists of */
		matrix->dimsNum = readUInt(&f);

		/** Allocate the needed memory to store the dimensions sizes */
		matrix->dims = (uint *) malloc(matrix->dimsNum * sizeof(uint));

		/** For every dimension of this parameters set */
		for (int i = 0; i < matrix->dimsNum; i++) {
			/** Read the dimension's size */
			matrix->dims[i] = readUInt(&f);
		}

		/** Calculate the total parameters number */
		uint xLen = flattenDimensions(matrix);

		/** Allocate the needed memory to store all the parameters */
		matrix->matrix = (matrix_t *) malloc(xLen * sizeof(matrix_t));
		if (matrix->matrix == NULL) {
			printf("Error. Not enough memory!\n");
			return NULL;
		}

		/** For every parameter */
		for (int i = 0; i < xLen; i++) {
			/** Read parameter */
			matrix->matrix[i] = readFloat(&f);
		}

		/** Assign matrix to corresponding parameters set index */
		params->matrix[p] = matrix;
		printf("\b\b\b");
		printf("%02d%%", (p*100)/params->len);
	}
	printf("\b\b\b");
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Closing File %s: ", filename);
	fRes = f_close(&f);
	if(fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED,  fRes, KNRM);
		return NULL;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	return params;
}


static FATFS fatfs;


int mount_sd(){
	FRESULT fRes;

	printf("- Mounting SD Card: ");
	fRes = f_mount(&fatfs, "0:/", 0);
	if(fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED,  fRes, KNRM);
		return XST_FAILURE;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
	return XST_SUCCESS;
}


#endif /* SRC_FILE_UTILS_H_ */
