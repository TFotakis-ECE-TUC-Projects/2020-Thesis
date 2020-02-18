#ifndef SRC_FILE_UTILS_H_
#define SRC_FILE_UTILS_H_

#include <string.h>
#include <ff.h>
#include <xstatus.h>
#include "floatmatrix_utils.h"
#include "platform_conf.h"
#include "terminal_colors.h"

/** Structure to store the network's parameters */
typedef struct {
	unsigned int len;	 /** The number of parameter matrices */
	FloatMatrix **matrix; /** The 1D array of FloatMartix cells to store the
							 parameters */
} Params;

unsigned int readUInt(FIL *f) {
	unsigned int num;
	unsigned int bytes_read;
	FRESULT fRes = f_read(f, &num, 4, &bytes_read);
	if (fRes != FR_OK) {
		printf("%sError %d. Cannot read uint.%s\n", KRED, fRes, KNRM);
		exit(XST_FAILURE);
	}
	return num;
}

float readFloat(FIL *f) {
	float num;
	unsigned int bytes_read;
	FRESULT fRes = f_read(f, &num, 4, &bytes_read);
	if (fRes != FR_OK) {
		printf("%sError %d. Cannot read float.%s\n", KRED, fRes, KNRM);
		exit(XST_FAILURE);
	}
	return num;
}

/** Structure to contain file paths */
typedef struct {
	/** Length of the list (number of paths stored) */
	unsigned int length;
	/** List of strings to store filepaths */
	char **list;
} Filelist;

/**
 * Reads a path and creates a Filelist which contains all files contained in
 * the given path's children directories
 * @param[in] path
 * @returns Filelist *
 */
Filelist *getFileList(char *path) {
	printf("- Loading file list \"%s\": ", path);
	DIR dp;
	FRESULT fRes = f_opendir(&dp, path);
	if (fRes != FR_OK) {
		printf("%sError %d. Cannot open directory.%s\n", KRED, fRes, KNRM);
		exit(XST_FAILURE);
	}

	Filelist *filelist = (Filelist *) malloc(sizeof(Filelist));
	if (filelist == NULL) {
		printf("%sError. Not enough memory.%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

	unsigned int filesCount = 0;
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

	char **pathsArr = (char **) malloc(filesCount * sizeof(char *));
	if (pathsArr == NULL) {
		printf("%sError. Not enough memory.%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

	unsigned int index = 0;
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
		if (dest == NULL) {
			printf("%sError. Not enough memory.%s\n", KRED, KNRM);
			exit(XST_FAILURE);
		}

		strcpy(dest, path);
		strcat(dest, "/");
		strcat(dest, fn);
		pathsArr[index] = dest;
		index++;
	}
	filelist->list = pathsArr;

	fRes = f_closedir(&dp);
	if (fRes != FR_OK) {
		printf("%sError %d. Cannot close directory.%s\n", KRED, fRes, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess.%s\n", KGRN, KNRM);
	return filelist;
}

FIL open_file(char *path) {
	FIL f;
	FRESULT fRes = f_open(&f, path, FA_OPEN_EXISTING | FA_READ);
	if (fRes != FR_OK) {
		printf("%sError %d. Cannot open file %s.%s\n", KRED, fRes, path, KNRM);
		exit(XST_FAILURE);
	}
	return f;
}

int close_file(FIL f) {
	FRESULT fRes = f_close(&f);
	if (fRes != FR_OK) {
		printf("%sError %d. Cannot close file%s\n", KRED, fRes, KNRM);
		exit(XST_FAILURE);
	}
	return XST_SUCCESS;
}

/**
 * Loads the network's parameters using the file defined on the parametersPath
 * global variable.
 * @returns a Params structure pointer containing the loaded network parameters.
 */
Params *loadParameters(char *filename) {
	printf("- Loading \"%s\": 00%%", filename);

	/** Open the parameters file */
	FIL f = open_file(filename);

	/** Allocate the needed memory to store the Params structure */
	Params *params = (Params *) malloc(sizeof(Params));
	if (params == NULL) {
		printf("\b\b\b");
		printf("%sError. Not enough memory.%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

	/** Get the number of parameters sets existing in this file */
	params->len = readUInt(&f);

	/**
	 * Allocate the needed memory to store all the FloatMatrix structures
	 * (one for each parameters set)
	 */
	params->matrix =
		(FloatMatrix **) malloc(params->len * sizeof(FloatMatrix *));
	if (params->matrix == NULL) {
		printf("\b\b\b");
		printf("%sError. Not enough memory.%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

	/** For every parameters set */
	FloatMatrix *matrix;
	for (unsigned int p = 0; p < params->len; p++) {
		/** Allocate the needed memory to store the FloatMatrix structure */
		matrix = (FloatMatrix *) malloc(sizeof(FloatMatrix));
		if (matrix == NULL) {
			printf("\b\b\b");
			printf("%sError. Not enough memory.%s\n", KRED, KNRM);
			exit(XST_FAILURE);
		}

		/** Get the number of dimensions this parameters set consists of */
		matrix->dimsNum = readUInt(&f);

		/** Allocate the needed memory to store the dimensions sizes */
		matrix->dims =
			(unsigned int *) malloc(matrix->dimsNum * sizeof(unsigned int));
		if (matrix->dims == NULL) {
			printf("\b\b\b");
			printf("%sError. Not enough memory.%s\n", KRED, KNRM);
			exit(XST_FAILURE);
		}

		/** For every dimension of this parameters set */
		for (unsigned int i = 0; i < matrix->dimsNum; i++) {
			/** Read the dimension's size */
			matrix->dims[i] = readUInt(&f);
		}

		/** Calculate the total parameters number */
		unsigned int xLen = flattenDimensions(matrix);

		/** Allocate the needed memory to store all the parameters */
		matrix->matrix = (matrix_t *) malloc(xLen * sizeof(matrix_t));
		if (matrix->matrix == NULL) {
			printf("\b\b\b");
			printf("%sError. Not enough memory.%s\n", KRED, KNRM);
			exit(XST_FAILURE);
		}

		/** For every parameter */
		for (unsigned int i = 0; i < xLen; i++) {
			/** Read parameter */
			matrix->matrix[i] = readFloat(&f);
		}

		/** Assign matrix to corresponding parameters set index */
		params->matrix[p] = matrix;
		printf("\b\b\b");
		printf("%02d%%", (p * 100) / params->len);
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
	printf("- Loading \"%s\": ", labelsPath);

	/** Open the labels file */
	FIL f = open_file(labelsPath);

	char *s;
	char buff[200];

	/** Read the number of labels contained in this file */
	unsigned int labelsNum;
	s = f_gets(buff, sizeof(buff), &f);
	sscanf(s, "%u\n", &labelsNum);

	/** Allocate the needed memory to store the labels array */
	char **labels = (char **) malloc(labelsNum * sizeof(char *));
	if (labels == NULL) {
		printf("%sError. Not enough memory.%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

	/** For every label in the file */
	for (unsigned int i = 0; i < labelsNum; i++) {
		/** Allocate memory for each label string */
		labels[i] = (char *) malloc(150 * sizeof(char));
		if (labels[i] == NULL) {
			printf("%sError. Not enough memory.%s\n", KRED, KNRM);
			exit(XST_FAILURE);
		}

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
	char *s;
	char buff[16];

	// check the image format
	s = f_gets(buff, sizeof(buff), &f);
	if (!s || buff[0] != 'P' || buff[1] != '6') {
		printf(
			"%s- Error. Invalid image format on \"%s\".%s\n", KRED, path, KNRM);
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
		printf("%s- Invalid image size of \"%s\"%s\n", KRED, path, KNRM);
		exit(XST_FAILURE);
	}

	// read RGB component
	s = f_gets(buff, sizeof(buff), &f);
	if (sscanf(s, "%d", &depth) != 1) {
		printf(
			"%s- Error. Invalid RGB component on \"%s\".%s\n",
			KRED,
			path,
			KNRM);
		exit(XST_FAILURE);
	}

	unsigned char color;
	unsigned int bytes_read;
	f_read(&f, &color, 1, &bytes_read);

	FloatMatrix *x = zero3DFloatMatrix(3, height, width);
	for (unsigned int h = 0; h < height; h++) {
		for (unsigned int w = 0; w < width; w++) {
			for (unsigned int c = 0; c < 3; c++) {
				f_read(&f, &color, 1, &bytes_read);
				unsigned int index = calc3DIndex(x->dims, c, h, w);
				matrix_t num = color / 255.0;
				x->matrix[index] = num;
			}
		}
	}
	close_file(f);

	return x;
}

NetConf *read_config(char *path) {
	printf("- Loading \"%s\": ", path);

	FIL f = open_file(path);

	char buff[200];

	NetConf *netConf = (NetConf *) malloc(sizeof(NetConf));
	if (netConf == NULL) {
		printf(
			"%sError. Not enough memory for read config netConf.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	char *s = f_gets(buff, sizeof(buff), &f);
	sscanf(s, "LAYERS_NUMBER=%u\n", &netConf->layersNum);

	netConf->layersConf = (LayerConf *) malloc(netConf->layersNum * sizeof(LayerConf));
	if (netConf->layersConf == NULL) {
		printf(
			"%sError. Not enough memory for read config netConf->layersConf.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	s = f_gets(buff, sizeof(buff), &f);
	sscanf(s, "din=%u\n", &netConf->layersConf[0].din);

	s = f_gets(buff, sizeof(buff), &f);
	sscanf(s, "hin=%u\n", &netConf->layersConf[0].hin);

	s = f_gets(buff, sizeof(buff), &f);
	sscanf(s, "win=%u\n", &netConf->layersConf[0].win);

	for (unsigned int i = 0; i < netConf->layersNum; i++) {
		s = f_gets(buff, sizeof(buff), &f);
		char layerType[200];
		sscanf(s, "layerType=%s\n", layerType);

		if (!strcmp(layerType, "Conv")) {
			netConf->layersConf[i].layerType = CONV_LAYER_TYPE;

			s = f_gets(buff, sizeof(buff), &f);
			sscanf(s, "kernelSize=%u\n", &netConf->layersConf[i].kernelSize);

			s = f_gets(buff, sizeof(buff), &f);
			sscanf(s, "stride=%u\n", &netConf->layersConf[i].stride);

			s = f_gets(buff, sizeof(buff), &f);
			sscanf(s, "padding=%u\n", &netConf->layersConf[i].padding);

			s = f_gets(buff, sizeof(buff), &f);
			sscanf(s, "dout=%u\n", &netConf->layersConf[i].dout);

		} else if (!strcmp(layerType, "Maxpool")) {
			netConf->layersConf[i].layerType = MAXPOOL_LAYER_TYPE;

			s = f_gets(buff, sizeof(buff), &f);
			sscanf(s, "kernelSize=%u\n", &netConf->layersConf[i].kernelSize);

			s = f_gets(buff, sizeof(buff), &f);
			sscanf(s, "stride=%u\n", &netConf->layersConf[i].stride);
		} else if (!strcmp(layerType, "Linear")) {
			netConf->layersConf[i].layerType = LINEAR_LAYER_TYPE;
			netConf->layersConf[i].doReLU = 0;

			s = f_gets(buff, sizeof(buff), &f);
			sscanf(s, "outFeatures=%u\n", &netConf->layersConf[i].outFeatures);
		} else if (!strcmp(layerType, "LinearReLU")) {
			netConf->layersConf[i].layerType = LINEAR_RELU_LAYER_TYPE;
			netConf->layersConf[i].doReLU = 1;

			s = f_gets(buff, sizeof(buff), &f);
			sscanf(s, "outFeatures=%u\n", &netConf->layersConf[i].outFeatures);
		}
	}
	close_file(f);
	printf("%sSuccess%s\n", KGRN, KNRM);
	return netConf;
}

static FATFS fatfs;

int mount_sd() {
	FRESULT fRes;

	printf("- Mounting SD Card: ");
	fRes = f_mount(&fatfs, "0:/", 0);
	if (fRes != FR_OK) {
		printf("%sError %d.%s\n", KRED, fRes, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
	return XST_SUCCESS;
}

#endif /* SRC_FILE_UTILS_H_ */
