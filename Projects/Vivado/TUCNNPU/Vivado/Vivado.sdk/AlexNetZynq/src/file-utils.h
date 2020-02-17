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


/** Structure to contain file paths */
typedef struct filelist_t {
	/** Length of the list (number of paths stored) */
	uint length;
	/** List of strings to store filepaths */
	char **list;
} Filelist;


/**
 * Reads a path and creates a Filelist which containes all files contained in
 * the given path's children directories
 * @param[in] path
 * @returns Filelist *
 */
Filelist *getFileList(char *path) {
	DIR dp;
	printf("- Opening directory \"%s\": ", path);
	FRESULT fRes = f_opendir(&dp, path);
	if (fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED, fRes, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess.%s\n", KGRN, KNRM);

	Filelist *filelist = (Filelist *) malloc(sizeof(Filelist));
	uint filesCount = 0;
	while (1) {
		FILINFO fno;
		// Read a directory item
		int res = f_readdir(&dp, &fno);
		// Break on error or end of dir
		if (res != FR_OK || fno.fname[0] == 0) break;
		// Ignore dot entry
		if (fno.fname[0] == '.') continue;
		/* Ignore directories */
		if (fno.fattrib & AM_DIR) continue;

		// char *fn = fno.fname;
		// printf("%s/%s\n\r", path, fn);
		filesCount++;
	}
	filelist->length = filesCount;

	fRes = f_closedir(&dp);
	fRes = f_opendir(&dp, path);

	char **pathsArr = (char**) malloc(filesCount * sizeof(char*));
	uint index = 0;
	while (1) {
		FILINFO fno;
		// Read a directory item
		int res = f_readdir(&dp, &fno);
		// Break on error or end of dir
		if (res != FR_OK || fno.fname[0] == 0) break;
		// Ignore dot entry
		if (fno.fname[0] == '.') continue;
		/* Ignore directories */
		if (fno.fattrib & AM_DIR) continue;

		char *fn = fno.fname;
		int destLen = strlen(path) + 1 + strlen(fn) + 1;
		char *dest = (char *) malloc(destLen * sizeof(char));
		strcpy(dest, path);
		strcat(dest, "/");
		strcat(dest, fn);
		pathsArr[index] = dest;
		index++;
	}
	filelist->list = pathsArr;

	printf("- Closing directory \"%s\": ", path);
	fRes = f_closedir(&dp);
	if (fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED, fRes, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess.%s\n", KGRN, KNRM);
	return filelist;
}


FIL open_file(char *path) {
	FIL f;
	printf("- Opening File \"%s\": ", path);
	FRESULT fRes = f_open(&f, path, FA_OPEN_EXISTING | FA_READ);
	if(fRes != FR_OK){
		printf("%sError %d.%s\n", KRED,  fRes, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
	return f;
}


int close_file(FIL f) {
//	printf("- Closing File %s: ", filename);
	printf("- Closing File: ");
	FRESULT fRes = f_close(&f);
	if(fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED,  fRes, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
	return XST_SUCCESS;
}


/**
 * Loads the network's parameters using the file defined on the parametersPath
 * global variable.
 * @returns a Params structure pointer containing the loaded network parameters.
 */
Params *loadParameters(char *filename) {
	/** Open the parameters file */
	FIL f = open_file(filename);

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
			exit(XST_FAILURE);
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

	close_file(f);

	return params;
}


/**
 * Loads the network's classes' labels using the file defined on the labelsPath
 * global variable.
 * @returns an array of strings containing the loaded labels.
 */
char **loadLabels(char *labelsPath) {
	/** Open the labels file */
	FIL f = open_file(labelsPath);

	printf("- Loading labels: ");

	char *s;
	char buff[200];

	/** Read the number of labels contained in this file */
	uint labelsNum;
	s = f_gets(buff, sizeof(buff), &f);
	sscanf(s, "%u\n", &labelsNum);

	/** Allocate the needed memory to store the labels array */
	char **labels = (char **) malloc(labelsNum * sizeof(char *));

	/** For every label in the file */
	for (int i = 0; i < labelsNum; i++) {
		/** Allocate memory for each label string */
		labels[i] = (char *) malloc(150 * sizeof(char));

		/** Read the label and store it to the allocated string */
		s = f_gets(buff, sizeof(buff), &f);
		sscanf(s, "%[^\n]\n", labels[i]);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	close_file(f);

	return labels;
}


FloatMatrix *loadImage(char *path) {
	FIL f = open_file(path);
	printf("- Loading image \"%s\": ", path);
	char *s;
	char buff[16];

	// check the image format
	s = f_gets(buff, sizeof(buff), &f);
	if (!s || buff[0] != 'P' || buff[1] != '6') {
		printf("%sInvalid image format (must be 'P6')%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

	int character, width, height, depth;
	// check for comments
	s = f_gets(buff, sizeof(buff), &f);
	character = buff[0];
	while (character == '#') {
		s = f_gets(buff, sizeof(buff), &f);
		character = buff[0];
	}

	// read image size information
	if (sscanf(s, "%d %d", &width, &height) != 2) {
		printf("%sInvalid image size (error loading '%s')%s\n", KRED, path, KNRM);
		exit(XST_FAILURE);
	}

	// read RGB component
	s = f_gets(buff, sizeof(buff), &f);
	if (sscanf(s, "%d", &depth) != 1) {
		printf("%sInvalid RGB component (error loading '%s')%s\n", KRED, path, KNRM);
		exit(XST_FAILURE);
	}

	uint8_t color;
	uint bytes_read;
	f_read(&f, &color, 1, &bytes_read);

	FloatMatrix *x = zero3DFloatMatrix(3, height, width);
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			for (int c = 0; c < 3; c++) {
				f_read(&f, &color, 1, &bytes_read);
				uint index = calc3DIndex(x->dims, c, h, w);
				// matrix_t num = roundToDecimals(color / 255.0, 10);
				matrix_t num = color / 255.0;
				x->matrix[index] = num;
			}
		}
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	close_file(f);

	return x;
}


static FATFS fatfs;


int mount_sd(){
	FRESULT fRes;

	printf("- Mounting SD Card: ");
	fRes = f_mount(&fatfs, "0:/", 0);
	if(fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED,  fRes, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
	return XST_SUCCESS;
}


#endif /* SRC_FILE_UTILS_H_ */
