#ifndef SRC_PLATFORM_H_
#define SRC_PLATFORM_H_

#include <xil_cache.h>
#include <xparameters.h>
#include <xscugic.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ff.h>
#include <xstatus.h>
#include "terminal_colors.h"

typedef float matrix_t;

// ----------------------------------------------

typedef enum {
	CONV_LAYER_TYPE,
	MAXPOOL_LAYER_TYPE,
	LINEAR_LAYER_TYPE,
	LINEAR_RELU_LAYER_TYPE
} LayerType;

typedef struct {
	unsigned int InstanceId;
	void *InstancePtr;
} Core;

/** Contains all necessary data for an image to be stored in memory. */
typedef struct {
	/** Image's height */
	unsigned int height;
	/** Image's width */
	unsigned int width;
	/** Image's color profile (RGB -> 3, BW -> 1, CMYK -> 4, etc.) */
	unsigned int depth;
	/**
	 * Holds RGB data after jpeg decompression, one color in each cell,
	 * flattened row by row
	 */
	unsigned char *bmp_buffer;
	/** Size of bmp buffer in cells */
	unsigned long bmp_size;
	/** Image's path */
	char *path;
	/**
	 * A 3-dimensional representation of an image.
	 * First dimension: Color channel
	 * Second dimension: Image's rows in pixels
	 * Third dimension: Image's columns in pixels
	 *
	 * Valid pixel values: unsigned integers in range (0, 255).
	 */
	unsigned char ***channels;
} Image;

/** Structure to contain file paths */
typedef struct {
	/** Length of the list (number of paths stored) */
	unsigned int length;
	/** List of strings to store filepaths */
	char **list;
} Filelist;

typedef struct LayerConf_t {
	LayerType layerType;
	unsigned int kernelSize;
	unsigned int stride;
	unsigned int padding;
	unsigned int din;
	unsigned int hin;
	unsigned int win;
	unsigned int dout;
	unsigned int hout;
	unsigned int wout;
	unsigned int inFeatures;
	unsigned int outFeatures;
	unsigned int doReLU;
	unsigned int xSize;
	unsigned int weightsSize;
	unsigned int biasSize;
	unsigned int resSize;
	matrix_t *weightsAddr;
	matrix_t *biasAddr;
	matrix_t *(*hw_func)();
	matrix_t *(*sw_func)();
} LayerConf;

typedef struct {
	unsigned int layersNum;
	LayerConf *layersConf;
	matrix_t **params;
	char **labels;
	Filelist *imagesPaths;
} NetConf;

// ----------------------------------------------

/**
 * Calculates the index on a 1D buffer representation of a 3D matrix
 * @param[in] dim0 - dim2: the sizes of the matrix's dimensions
 * @param[in] i: The plain dimension
 * @param[in] j: The row dimension
 * @param[in] k: The column dimension
 * @returns the 1D representation's index of a 3D matrix
 */
unsigned int calc3DIndex(
	unsigned int dim0,
	unsigned int dim1,
	unsigned int dim2,
	unsigned int i,
	unsigned int j,
	unsigned int k) {
	return k + j * dim2 + i * dim1 * dim2;
}

/**
 * Calculates the index on a 1D buffer representation of a 4D matrix
 * @param[in] dim0 - dim3: the sizes of the matrix's dimensions
 * @param[in] i: The forth dimension
 * @param[in] j: The plain dimension
 * @param[in] k: The row dimension
 * @param[in] l: The column dimension
 * @returns the 1D representation's index of a 4D matrix
 */
unsigned int calc4DIndex(
	unsigned int dim0,
	unsigned int dim1,
	unsigned int dim2,
	unsigned int dim3,
	unsigned int i,
	unsigned int j,
	unsigned int k,
	unsigned int l) {
	return l + k * dim3 + j * dim2 * dim3 + i * dim1 * dim2 * dim3;
}

/**
 * Allocates a 1-dimensional matrix
 * @param[in] dim: The matrix's size
 * @returns the pointer of the allocated matrix
 */
matrix_t *createMatrix(unsigned int dim) {
	/** Allocate matrix's buffer */
	matrix_t *res = (matrix_t *) malloc(dim * sizeof(matrix_t));
	if (res == NULL) {
		printf("%sError. Not enough memory for createMatrix." KRED, KNRM);
		exit(XST_FAILURE);
	}

	return res;
}

/**
 * Allocates and initializes a 3D FloatMatrix with zeros
 * @param[in] dim1: the plain dimension
 * @param[in] dim2: the row dimension
 * @param[in] dim3: the column dimension
 * @returns the pointer of the allocated and initialized FloatMatrix
 */
matrix_t *zeroMatrix(unsigned int dim) {
	matrix_t *res = createMatrix(dim);
	for (unsigned int i = 0; i < dim; i++) res[i] = 0;
	return res;
}

/**
 * Finds the max value in a matrix and returns its index as if it was 1D
 * @param[in] x: input matrix
 * @param[in] xSize: input matrix size
 * @returns the 1-dimensional index of the highest valued cell
 */
unsigned int argmax(matrix_t *x, unsigned int xSize) {
	/** Initialize max to the matrix's first cell */
	matrix_t max = x[0];

	/** Initialize the highest valued max index to be the first of the matrix */
	unsigned int index = 0;
	for (unsigned int i = 1; i < xSize; i++) {
		/** If max greater than the current value ignore and continue */
		if (max >= x[i]) continue;

		/** If current max is less than current value update it and its index */
		max = x[i];
		index = i;
	}
	return index;
}

void freeLayerConf(LayerConf *lc, unsigned int layersNum) {
	for (unsigned int layer = 0; layer < layersNum; layer++) {
		free(lc[layer].weightsAddr);
		free(lc[layer].biasAddr);
	}
	free(lc);
}

void freeFilelist(Filelist *fl) {
	for (unsigned int f = 0; f < fl->length; f++) free(fl->list[f]);
	free(fl);
}

void freeNetConf(NetConf *netConf) {
	unsigned int labelsNum =
		netConf->layersConf[netConf->layersNum - 1].outFeatures;
	freeLayerConf(netConf->layersConf, netConf->layersNum);
	free(netConf->params);

	for (unsigned int label = 0; label < labelsNum; label++) {
		free(netConf->labels[label]);
	}
	free(netConf->labels);

	freeFilelist(netConf->imagesPaths);

	free(netConf);
}

// ----------------------------------------------

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

/**
 * Reads a path and creates a Filelist which contains all files contained in
 * the given path's children directories
 * @param[in] path
 * @returns Filelist *
 */
Filelist *getFilelist(char *path) {
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

char *selectFilePath(char *filesDir, char *msg) {
	Filelist *configsList = getFileList(filesDir);
	printf("%s", msg);
	for (unsigned int i = 0; i < configsList->length; i++) {
		printf("\t%d. %s\n", i + 1, configsList->list[i]);
	}

	unsigned int selection;
	printf("Select a number [1-%d]: ", configsList->length);
	scanf("%d", &selection);
	while (selection <= 0 || selection > configsList->length) {
		printf("\n%sWrong input.%s\n", KRED, KNRM);
		printf("Select a number [1-%d]: ", configsList->length);
		scanf("%d", &selection);
	}

	char *path;
	strcpy(path, configsList->list[selection - 1]);

	freeFilelist(configsList);

	return path;
}

/**
 * Loads the network's parameters using the file defined on the parametersPath
 * global variable.
 * @returns a Params structure pointer containing the loaded network parameters.
 */
matrix_t **loadParameters(char *filename) {
	printf("- Loading \"%s\": 00%%", filename);

	/** Open the parameters file */
	FIL f = open_file(filename);

	/** Get the number of parameters sets existing in this file */
	unsigned int params_len = readUInt(&f);

	/**
	 * Allocate the needed memory to store all the FloatMatrix structures
	 * (one for each parameters set)
	 */
	matrix_t **params = (matrix_t **) malloc(params_len * sizeof(matrix_t *));
	if (params == NULL) {
		printf("\b\b\b");
		printf(
			"%sError. Not enough memory on loadParameters for params.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	/** For every parameters set */
	for (unsigned int p = 0; p < params_len; p++) {
		/** Get the number of dimensions this parameters set consists of */
		unsigned int matrix_dimsNum = readUInt(&f);

		unsigned int xLen = 1;
		/** For every dimension of this parameters set */
		for (unsigned int i = 0; i < matrix_dimsNum; i++) {
			/** Read the dimension's size */
			xLen *= readUInt(&f);
		}

		/** Allocate the needed memory to store all the parameters */
		params[p] = (matrix_t *) malloc(xLen * sizeof(matrix_t));
		if (params[p] == NULL) {
			printf("\b\b\b");
			printf(
				"%sError. Not enough memory on loadParameters for "
				"params[%d].%s\n",
				KRED,
				p,
				KNRM);
			exit(XST_FAILURE);
		}

		/** For every parameter */
		for (unsigned int i = 0; i < xLen; i++) {
			/** Read parameter */
			params[p][i] = readFloat(&f);
		}

		printf("\b\b\b");
		printf("%02d%%", (p * 100) / params_len);
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

matrix_t *loadImage(char *path) {
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
	unsigned char maxColorValue;
	unsigned int bytes_read;
	f_read(&f, &maxColorValue, 1, &bytes_read);

	matrix_t *x = (matrix_t *) malloc(3 * height * width * sizeof(matrix_t));
	if (x == NULL) {
		printf("\b\b\b");
		printf(
			"%sError. Not enough memory on loadImage for x.%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}

	for (unsigned int h = 0; h < height; h++) {
		for (unsigned int w = 0; w < width; w++) {
			for (unsigned int c = 0; c < 3; c++) {
				f_read(&f, &color, 1, &bytes_read);
				unsigned int index = calc3DIndex(3, height, width, c, h, w);
				x[index] = color / 255.0;
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

	netConf->layersConf =
		(LayerConf *) malloc(netConf->layersNum * sizeof(LayerConf));
	if (netConf->layersConf == NULL) {
		printf(
			"%sError. Not enough memory for read config "
			"netConf->layersConf.%s\n",
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

NetConf *selectNetConf(
	char *configsDir,
	char *labelsDir,
	char *paramsDir,
	char *imagesDir) {
	char *configFile =
		selectFilePath(configsDir, "- Select network configuration:\n");
	char *configDir, configName;
	sscanf(configFile, "%s/%s.conf", configDir, configName);

	char *labelsFile;
	sprintf(labelsFile, "%s/%s.labels", labelsDir, configName);

	char *paramsFile;
	sprintf(paramsFile, "%s/%s.params", paramsDir, configName);

	NetConf *netConf = read_config(configFile);
	netConf->labels = loadLabels(labelsFile);
	netConf->params = loadParameters(paramsFile);
	netConf->imagesPaths = getFileList(imagesDir);

	free(configFile);
	free(configName);
	free(labelsFile);
	free(paramsFile);

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

// ----------------------------------------------

/**
 * Calculates the 2-dimensional convolution of FloatMatrix x and filters stored
 * in FloatMatrix weights, biased by FloatMatrix bias.
 * @param[in] x: The input FloatMatrix to convolute
 * @param[in] weights: The kernels to convolute with
 * @param[in] bias: The bias of the convolution
 * @param[in] in_channels: The number of x's channels
 * @param[in] out_channels: The number of output channels
 * @param[in] lc.kernelSize: The kernels' dimensions
 * @param[in] lc.stride: The amount of pixels to skip for each convolution
 * @param[in] lc.padding: The circular lc.padding applied to the input before
 * convoluting
 * @returns a FloatMatrix pointer containing the convolution's result
 */
matrix_t *Conv_sw(matrix_t *x, LayerConf lc) {
	matrix_t *res = zeroMatrix(lc.resSize);

	/** For every output channel */
	for (unsigned int out_channel = 0; out_channel < lc.dout; out_channel++) {
		/** For every output row */
		for (unsigned int oh = 0; oh < lc.hout; oh++) {
			int imgStartH = oh * lc.stride - lc.padding;

			unsigned int iStart = imgStartH < 0 ? lc.padding : 0;
			unsigned int iEnd = imgStartH + lc.kernelSize >= lc.hin ?
				lc.kernelSize - (imgStartH + lc.kernelSize - lc.hin) :
				lc.kernelSize;

			/** For every output row's pixel */
			for (unsigned int ow = 0; ow < lc.wout; ow++) {
				/** Calculate starting coordinates on the padded matrix */
				int imgStartW = ow * lc.stride - lc.padding;

				unsigned int jStart = imgStartW < 0 ? lc.padding : 0;
				unsigned int jEnd = imgStartW + lc.kernelSize >= lc.win ?
					lc.kernelSize - (imgStartW + lc.kernelSize - lc.win) :
					lc.kernelSize;

				/** Initialize output pixel */
				matrix_t pixel = lc.biasAddr[out_channel];
				/** For every input channel */
				for (unsigned int in_channel = 0; in_channel < lc.din;
					 in_channel++) {
					/** For lc.kernelSize rows on padded matrix */
					for (unsigned int i = iStart; i < iEnd; i++) {
						/** For lc.kernelSize pixels on each padded matrix's row
						 */
						for (unsigned int j = jStart; j < jEnd; j++) {
							unsigned int imgH = i + imgStartH;
							unsigned int imgW = j + imgStartW;

							/**
							 * Calculate the 1-dimensional representation's
							 * index of the kernel's matrix
							 */
							unsigned int weightsIndex = calc4DIndex(
								lc.dout,
								lc.din,
								lc.kernelSize,
								lc.kernelSize,
								out_channel,
								in_channel,
								i,
								j);

							/**
							 * Calculate the 1-dimensional representation's
							 * index of the padded matrix
							 */
							unsigned int arrIndex = calc3DIndex(
								lc.din, lc.hin, lc.win, in_channel, imgH, imgW);

							/** Calculate dot product of the two matrices */
							pixel += x[arrIndex] * lc.weightsAddr[weightsIndex];
						}
					}
				}
				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				unsigned int resIndex =
					calc3DIndex(lc.dout, lc.hout, lc.wout, out_channel, oh, ow);

				/**
				 * Assign biased dot product result on the corresponding
				 * output pixel
				 */
				res[resIndex] = pixel > 0 ? pixel : 0;
			}
		}
	}

	/** Free the input matrix */
	free(x);
	return res;
}

#ifdef XPAR_XCONV_CORE_NUM_INSTANCES
#include <xconv_core.h>
#define CONV_CORES_NUM XPAR_XCONV_CORE_NUM_INSTANCES
Core *Conv_core_list[CONV_CORES_NUM];
#endif

#if CONV_CORES_NUM == 1
const unsigned int Conv_core_device_ids[CONV_CORES_NUM] = {
	XPAR_XCONV_CORE_0_DEVICE_ID,
};
const unsigned int Conv_core_interrupt_ids[CONV_CORES_NUM] = {
	XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
};
volatile static int Conv_core_result_avail[CONV_CORES_NUM] = {
	0,
};
#elif CONV_CORES_NUM == 2
const unsigned int Conv_core_device_ids[CONV_CORES_NUM] = {
	XPAR_XCONV_CORE_0_DEVICE_ID,
	XPAR_XCONV_CORE_1_DEVICE_ID,
};
const unsigned int Conv_core_interrupt_ids[CONV_CORES_NUM] = {
	XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_1_INTERRUPT_INTR,
};
volatile static int Conv_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
};
#elif CONV_CORES_NUM == 3
const unsigned int Conv_core_device_ids[CONV_CORES_NUM] = {
	XPAR_XCONV_CORE_0_DEVICE_ID,
	XPAR_XCONV_CORE_1_DEVICE_ID,
	XPAR_XCONV_CORE_2_DEVICE_ID,
};
const unsigned int Conv_core_interrupt_ids[CONV_CORES_NUM] = {
	XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_2_INTERRUPT_INTR,
};
volatile static int Conv_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
};
#elif CONV_CORES_NUM == 4
const unsigned int Conv_core_device_ids[CONV_CORES_NUM] = {
	XPAR_XCONV_CORE_0_DEVICE_ID,
	XPAR_XCONV_CORE_1_DEVICE_ID,
	XPAR_XCONV_CORE_2_DEVICE_ID,
	XPAR_XCONV_CORE_3_DEVICE_ID,
};
const unsigned int Conv_core_interrupt_ids[CONV_CORES_NUM] = {
	XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_2_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_3_INTERRUPT_INTR,
};
volatile static int Conv_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
	0,
};
#endif

Core *get_Conv_core() {
#ifdef CONV_CORES_NUM
	return Conv_core_list[0];
#else
	return NULL
#endif
}

void Conv_core_isr(Core *core) {
	XConv_core *pAccelerator = (XConv_core *) core->InstancePtr;
	// Disable the global interrupt
	XConv_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XConv_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XConv_core_InterruptClear(pAccelerator, 1);
	Conv_core_result_avail[core->InstanceId] = 1;
}

int Conv_core_setup_interrupt(Core *core, XScuGic ScuGic) {
	// Connect the Conv_core ISR to the exception table
	int result = XScuGic_Connect(
		&ScuGic,
		Conv_core_interrupt_ids[core->InstanceId],
		(Xil_InterruptHandler) Conv_core_isr,
		core);
	if (result != XST_SUCCESS) return result;
	XScuGic_Enable(&ScuGic, Conv_core_interrupt_ids[core->InstanceId]);
	return XST_SUCCESS;
}

void Conv_core_init(Core *core, XScuGic ScuGic) {
	printf("- Initializing Conv_core %u: ", core->InstanceId);
	core->InstancePtr = (void *) malloc(sizeof(XConv_core));
	int status = XConv_core_Initialize(
		core->InstancePtr, Conv_core_device_ids[core->InstanceId]);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	status = Conv_core_setup_interrupt(core, ScuGic);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
}

matrix_t *Conv_core_setup(XConv_core *Conv_core, LayerConf lc, matrix_t *x) {
	printf("- Setup Conv_core: ");
	XConv_core_Set_x(Conv_core, (unsigned int) (u64) x);
	XConv_core_Set_weights(Conv_core, (unsigned int) (u64) lc.weightsAddr);
	XConv_core_Set_bias(Conv_core, (unsigned int) (u64) lc.biasAddr);
	XConv_core_Set_din(Conv_core, lc.din);
	XConv_core_Set_hin(Conv_core, lc.hin);
	XConv_core_Set_win(Conv_core, lc.win);
	XConv_core_Set_dout(Conv_core, lc.dout);
	XConv_core_Set_hout(Conv_core, lc.hout);
	XConv_core_Set_wout(Conv_core, lc.wout);
	XConv_core_Set_kernel_size(Conv_core, lc.kernelSize);
	XConv_core_Set_stride(Conv_core, lc.stride);
	XConv_core_Set_padding(Conv_core, lc.padding);
	matrix_t *resAddr = (matrix_t *) malloc(lc.resSize * sizeof(matrix_t));
	if (resAddr == NULL) {
		printf(
			"%sError. Not enough memory for Linear_core_setup resAddr.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}
	XConv_core_Set_res(Conv_core, (unsigned int) (u64) resAddr);
	printf("%sSuccess%s\n", KGRN, KNRM);
	return resAddr;
}

void Conv_core_start(void *InstancePtr) {
	XConv_core *pAccelerator = (XConv_core *) InstancePtr;

	printf("- Waiting for Conv_core to get ready: ");
	while (!XConv_core_IsReady(pAccelerator)) continue;
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Starting Conv_core: ");
	XConv_core_InterruptEnable(pAccelerator, 1);
	XConv_core_InterruptGlobalEnable(pAccelerator);
	XConv_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

int Conv_core_wait_int(Core *core) {
	while (!Conv_core_result_avail[core->InstanceId]) continue;
	Conv_core_result_avail[core->InstanceId] = 0;
	printf("- Interrupt received from Conv_core.\n");
	int status = XConv_core_Get_return(core->InstancePtr);
	printf("- Return value: %d\n", status);
	return status;
}

matrix_t *Conv_core_process(LayerConf lc, matrix_t *x) {
	Core *core = get_Conv_core();
	matrix_t *res = Conv_core_setup(core->InstancePtr, lc, x);
	Conv_core_start(core->InstancePtr);
	Conv_core_wait_int(core);
	free(x);
	return res;
}

void Conv_conf_params_complete(LayerConf *lc) {
	lc->hout = (lc->hin + 2 * lc->padding - lc->kernelSize) / lc->stride + 1;
	lc->wout = (lc->win + 2 * lc->padding - lc->kernelSize) / lc->stride + 1;
	lc->xSize = lc->din * lc->hin * lc->win;
	lc->weightsSize = lc->dout * lc->din * lc->hout * lc->wout;
	lc->biasSize = lc->dout;
	lc->resSize = lc->dout * lc->hout * lc->wout;

	lc->hw_func = &Conv_core_process;
	lc->sw_func = &Conv_sw;
}

void Conv_conf_complete(
	NetConf *netConf,
	unsigned int layer_index,
	unsigned int params_index) {
	LayerConf *lc = &netConf->layersConf[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &netConf->layersConf[layer_index - 1];
		if (plc->layerType == LINEAR_RELU_LAYER_TYPE ||
			plc->layerType == LINEAR_LAYER_TYPE) {
			printf("Error. Linear layer cannot be followed by Conv "
				   "layer.\n");
			exit(XST_FAILURE);
		}
		lc->din = plc->dout;
		lc->hin = plc->hout;
		lc->win = plc->wout;
	}
	Conv_conf_params_complete(lc);
	lc->weightsAddr = netConf->params[params_index];
	lc->biasAddr = netConf->params[params_index + 1];
}

int Conv_core_test(unsigned int testAllCores) {
	printf("*** Conv_core_test ***\n");

	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

	if (CONV_CORES_NUM <= 0) {
		printf("%s- No Conv_core available!%s\n", KRED, KNRM);
		return XST_DEVICE_NOT_FOUND;
	}

	LayerConf lc = {
		.layerType = CONV_LAYER_TYPE,
		.kernelSize = 11,
		.stride = 4,
		.padding = 2,
		.din = 3,
		.hin = 224,
		.win = 224,
		.dout = 64,
	};

	Conv_conf_params_complete(&lc);

	printf("- Initializing Memory: ");
	matrix_t *x = (matrix_t *) malloc(lc.xSize * sizeof(matrix_t));
	for (unsigned int i = 0; i < lc.xSize; i++) x[i] = X_VALUE;

	lc.weightsAddr = (matrix_t *) malloc(lc.weightsSize * sizeof(matrix_t));
	for (unsigned int i = 0; i < lc.weightsSize; i++)
		lc.weightsAddr[i] = WEIGHT_VALUE;

	lc.biasAddr = (matrix_t *) malloc(lc.biasSize * sizeof(matrix_t));
	for (unsigned int i = 0; i < lc.biasSize; i++) lc.biasAddr[i] = BIAS_VALUE;
	printf("%sSuccess%s\n", KGRN, KNRM);

	int status = XST_SUCCESS;
	unsigned int core_num = testAllCores ? CONV_CORES_NUM : 1;
	for (unsigned int core = 0; core < core_num; core++) {
		printf("- Conv_core[%u]: ", core);

		matrix_t *res = (*lc.hw_func)(lc, x);

		matrix_t pixel_value;
		matrix_t error = 0;

		pixel_value =
			X_VALUE * WEIGHT_VALUE * lc.kernelSize * lc.kernelSize * lc.din +
			BIAS_VALUE;
		error +=
			abs(res[0 * lc.hout * lc.wout + 23 * lc.wout + 23] - pixel_value);
		error +=
			abs(res[1 * lc.hout * lc.wout + 23 * lc.wout + 23] - pixel_value);
		pixel_value = X_VALUE * WEIGHT_VALUE * (lc.kernelSize - lc.padding) *
				(lc.kernelSize - lc.padding) * lc.din +
			BIAS_VALUE;
		error += abs(res[0] - pixel_value);
		pixel_value = X_VALUE * WEIGHT_VALUE *
				(lc.kernelSize - lc.padding + 1) *
				(lc.kernelSize - lc.padding + 1) * lc.din +
			BIAS_VALUE;
		error +=
			abs(res[0 * lc.hout * lc.wout + 54 * lc.wout + 54] - pixel_value);

		if (error < 0.1) {
			printf("%sSuccess%s\n", KGRN, KNRM);
		} else {
			printf("%sError %f%s\n", KRED, error, KNRM);
			status = XST_FAILURE;
		}
		free(res);
	}

	free(lc.weightsAddr);
	free(lc.biasAddr);

	return status;
}

// ----------------------------------------------

/**
 * Applies 2-dimensional max pooling on the input FloatMatrix.
 * Creates pools of pixels, finds the maximum value for each pool and assigns
 * that value to the output FloatMatrix.
 * @param[in] x: The input FloatMatrix to apply max pooling
 * @param[in] lc.kernelSize: The kernel's dimensions
 * @param[in] lc.stride: The amount of pixels to skip for every pool
 * @returns a FloatMatrix pointer containing the max pooling's result
 */
matrix_t *Maxpool_sw(matrix_t *x, LayerConf lc) {
	matrix_t *res = zeroMatrix(lc.resSize);

	/** For every channel */
	for (unsigned int i = 0; i < lc.din; i++) {
		/** For every output row */
		for (unsigned int j = 0; j < lc.hout; j++) {
			/** For every output row's pixel */
			for (unsigned int k = 0; k < lc.wout; k++) {
				/** Calculate pools starting coordinates on the input matrix */
				unsigned int a = j * lc.stride;
				unsigned int b = k * lc.stride;

				/** Initialize max value */
				matrix_t max = -100000000;
				/** For every pool's row */
				for (unsigned int l = a; l < a + lc.kernelSize; l++) {
					/** For every pool row's pixel */
					for (unsigned int m = b; m < b + lc.kernelSize; m++) {
						/**
						 * Calculate the 1-dimensional representation's index of
						 * the input matrix
						 */
						unsigned int index =
							calc3DIndex(lc.din, lc.hin, lc.win, i, l, m);
						/**
						 * Check if current value is greater than max and
						 * update it
						 */
						max = max < x[index] ? x[index] : max;
					}
				}

				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				unsigned int resIndex =
					calc3DIndex(lc.dout, lc.hout, lc.wout, i, j, k);

				/** Assign found max value to the output matrix */
				res[resIndex] = max;
			}
		}
	}

	/** Free the input matrix */
	free(x);
	return res;
}

#ifdef XPAR_XMAXPOOL_CORE_NUM_INSTANCES
#include <xmaxpool_core.h>
#define MAXPOOL_CORES_NUM XPAR_XMAXPOOL_CORE_NUM_INSTANCES
Core *Maxpool_core_list[MAXPOOL_CORES_NUM];
#endif

#if MAXPOOL_CORES_NUM == 1
const unsigned int Maxpool_core_device_ids[MAXPOOL_CORES_NUM] = {
	XPAR_XMAXPOOL_CORE_0_DEVICE_ID,
};
const unsigned int Maxpool_core_interrupt_ids[MAXPOOL_CORES_NUM] = {
	XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
};
volatile static int Maxpool_core_result_avail[CONV_CORES_NUM] = {
	0,
};
#elif MAXPOOL_CORES_NUM == 2
const unsigned int Maxpool_core_device_ids[MAXPOOL_CORES_NUM] = {
	XPAR_XMAXPOOL_CORE_0_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_1_DEVICE_ID,
};
const unsigned int Maxpool_core_interrupt_ids[MAXPOOL_CORES_NUM] = {
	XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_1_INTERRUPT_INTR,
};
volatile static int Maxpool_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
};
#elif MAXPOOL_CORES_NUM == 3
const unsigned int Maxpool_core_device_ids[MAXPOOL_CORES_NUM] = {
	XPAR_XMAXPOOL_CORE_0_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_1_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_2_DEVICE_ID,
};
const unsigned int Maxpool_core_interrupt_ids[MAXPOOL_CORES_NUM] = {
	XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_2_INTERRUPT_INTR,
};
volatile static int Maxpool_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
};
#elif MAXPOOL_CORES_NUM == 4
const unsigned int Maxpool_core_device_ids[MAXPOOL_CORES_NUM] = {
	XPAR_XMAXPOOL_CORE_0_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_1_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_2_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_3_DEVICE_ID,
};
const unsigned int Maxpool_core_interrupt_ids[MAXPOOL_CORES_NUM] = {
	XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_2_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_3_INTERRUPT_INTR,
};
volatile static int Maxpool_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
	0,
};
#endif

Core *get_Maxpool_core() {
#ifdef MAXPOOL_CORES_NUM
	return Maxpool_core_list[0];
#else
	return NULL
#endif
}

void Maxpool_core_isr(Core *core) {
	XMaxpool_core *pAccelerator = (XMaxpool_core *) core->InstancePtr;
	// Disable the global interrupt
	XMaxpool_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XMaxpool_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XMaxpool_core_InterruptClear(pAccelerator, 1);
	Maxpool_core_result_avail[core->InstanceId] = 1;
}

int Maxpool_core_setup_interrupt(Core *core, XScuGic ScuGic) {
	// Connect the Maxpool_core ISR to the exception table
	int result = XScuGic_Connect(
		&ScuGic,
		Maxpool_core_interrupt_ids[core->InstanceId],
		(Xil_InterruptHandler) Maxpool_core_isr,
		core);
	if (result != XST_SUCCESS) return result;
	XScuGic_Enable(&ScuGic, Maxpool_core_interrupt_ids[core->InstanceId]);
	return XST_SUCCESS;
}

void Maxpool_core_init(Core *core, XScuGic ScuGic) {
	printf("- Initializing Maxpool_core %u: ", core->InstanceId);
	core->InstancePtr = (void *) malloc(sizeof(XMaxpool_core));
	int status = XMaxpool_core_Initialize(
		core->InstancePtr, Maxpool_core_device_ids[core->InstanceId]);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	status = Maxpool_core_setup_interrupt(core, ScuGic);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
}

matrix_t *
	Maxpool_core_setup(XMaxpool_core *Maxpool_core, LayerConf lc, matrix_t *x) {
	printf("- Setup Maxpool_core: ");
	XMaxpool_core_Set_x(Maxpool_core, (unsigned int) (u64) x);
	XMaxpool_core_Set_d(Maxpool_core, lc.din);
	XMaxpool_core_Set_hin(Maxpool_core, lc.hin);
	XMaxpool_core_Set_win(Maxpool_core, lc.win);
	XMaxpool_core_Set_hout(Maxpool_core, lc.hout);
	XMaxpool_core_Set_wout(Maxpool_core, lc.wout);
	XMaxpool_core_Set_kernel_size(Maxpool_core, lc.kernelSize);
	XMaxpool_core_Set_stride(Maxpool_core, lc.stride);
	matrix_t *resAddr = (matrix_t *) malloc(lc.resSize * sizeof(matrix_t));
	if (resAddr == NULL) {
		printf(
			"%sError. Not enough memory for Maxpool_core_setup resAddr.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}
	XMaxpool_core_Set_res(Maxpool_core, (unsigned int) (u64) resAddr);
	printf("%sSuccess%s\n", KGRN, KNRM);
	return resAddr;
}

void Maxpool_core_start(void *InstancePtr) {
	XMaxpool_core *pAccelerator = (XMaxpool_core *) InstancePtr;

	printf("- Waiting for Maxpool_core to get ready: ");
	while (!XMaxpool_core_IsReady(pAccelerator)) continue;
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Starting Maxpool_core: ");
	XMaxpool_core_InterruptEnable(pAccelerator, 1);
	XMaxpool_core_InterruptGlobalEnable(pAccelerator);
	XMaxpool_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

int Maxpool_core_wait_int(Core *core) {
	while (!Maxpool_core_result_avail[core->InstanceId]) continue;
	Maxpool_core_result_avail[core->InstanceId] = 0;
	printf("- Interrupt received from Maxpool_core.\n");
	int status = XMaxpool_core_Get_return(core->InstancePtr);
	printf("- Return value: %d\n", status);
	return status;
}

matrix_t *Maxpool_core_process(LayerConf lc, matrix_t *x) {
	Core *core = get_Maxpool_core();
	matrix_t *res = Maxpool_core_setup(core->InstancePtr, lc, x);
	Maxpool_core_start(core->InstancePtr);
	Maxpool_core_wait_int(core);
	free(x);
	return res;
}

void Maxpool_conf_params_complete(LayerConf *lc) {
	lc->dout = lc->din;
	lc->hout = (lc->hin - lc->kernelSize) / lc->stride + 1;
	lc->wout = (lc->win - lc->kernelSize) / lc->stride + 1;
	lc->xSize = lc->din * lc->hin * lc->win;
	lc->resSize = lc->dout * lc->hout * lc->wout;

	lc->hw_func = &Maxpool_core_process;
	lc->sw_func = &Maxpool_sw;
}

void Maxpool_conf_complete(NetConf *netConf, unsigned int layer_index) {
	LayerConf *lc = &netConf->layersConf[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &netConf->layersConf[layer_index - 1];
		if (plc->layerType == LINEAR_RELU_LAYER_TYPE ||
			plc->layerType == LINEAR_LAYER_TYPE) {
			printf("Error. Linear layer cannot be followed by Maxpool "
				   "layer.\n");
			exit(XST_FAILURE);
		}
		lc->din = plc->dout;
		lc->hin = plc->hout;
		lc->win = plc->wout;
	}

	Maxpool_conf_params_complete(lc);
	lc->weightsAddr = NULL;
	lc->biasAddr = NULL;
}

int Maxpool_core_test(unsigned int testAllCores) {
	printf("*** Maxpool_core_test ***\n");
	matrix_t X_VALUE = .1;

	if (MAXPOOL_CORES_NUM <= 0) {
		printf("%s- No Maxpool_core available!%s\n", KRED, KNRM);
		return XST_DEVICE_NOT_FOUND;
	}

	LayerConf lc = {
		.layerType = MAXPOOL_LAYER_TYPE,
		.kernelSize = 3,
		.stride = 2,
		.din = 64,
		.hin = 55,
		.win = 55,
	};

	Maxpool_conf_params_complete(&lc);

	printf("- Initializing Memory: ");
	matrix_t *x = (matrix_t *) malloc(lc.xSize * sizeof(matrix_t));
	for (unsigned int i = 0; i < lc.xSize; i++) x[i] = X_VALUE;
	printf("%sSuccess%s\n", KGRN, KNRM);

	int status = XST_SUCCESS;
	unsigned int core_num = testAllCores ? MAXPOOL_CORES_NUM : 1;
	for (unsigned int core = 0; core < core_num; core++) {
		printf("- Maxpool_core[%u]: ", core);

		matrix_t *res = (*lc.hw_func)(lc, x);

		matrix_t pixel_value = X_VALUE;
		matrix_t error = 0;

		for (unsigned int i = 0; i < lc.resSize; i++) {
			error += abs(res[i] - pixel_value);
		}

		if (error < 0.1) {
			printf("%sSuccess%s\n", KGRN, KNRM);
		} else {
			printf("%sError %f%s\n", KRED, error, KNRM);
			status = XST_FAILURE;
		}

		free(res);
	}
	return status;
}

// ----------------------------------------------

/**
 * Applies a linear transformation to the input FloatMatrix
 * @param[in] x: The input FloatMatrix
 * @param[in] lc: The Layer Configuration
 * @returns a FloatMatrix pointer containing the linear transformation's result
 */
matrix_t *Linear_sw(matrix_t *x, LayerConf lc) {
	/** Allocate and create a 1D matrix to store the result */
	matrix_t *res = createMatrix(lc.resSize);

	/** For every output feature */
	for (unsigned int i = 0; i < lc.outFeatures; i++) {
		/** Initialize output feature with its bias */
		matrix_t output = lc.biasAddr[i];

		/** For every input feature */
		for (unsigned int j = 0; j < lc.inFeatures; j++) {
			/** Add to the output feature the input feature's weighted value */
			output += x[j] * lc.weightsAddr[j + i * lc.inFeatures];
		}
		res[i] = lc.doReLU != 0 && output < 0 ? 0 : output;
	}

	/** Free the input matrix */
	free(x);
	return res;
}

#ifdef XPAR_XLINEAR_CORE_NUM_INSTANCES
#include <xlinear_core.h>
#define LINEAR_CORES_NUM XPAR_XLINEAR_CORE_NUM_INSTANCES
Core *Linear_core_list[LINEAR_CORES_NUM];
#endif

#if LINEAR_CORES_NUM == 1
const unsigned int Linear_core_device_ids[LINEAR_CORES_NUM] = {
	XPAR_XLINEAR_CORE_0_DEVICE_ID,
};
const unsigned int Linear_core_interrupt_ids[LINEAR_CORES_NUM] = {
	XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
};
volatile static int Linear_core_result_avail[CONV_CORES_NUM] = {
	0,
};
#elif LINEAR_CORES_NUM == 2
const unsigned int Linear_core_device_ids[LINEAR_CORES_NUM] = {
	XPAR_XLINEAR_CORE_0_DEVICE_ID,
	XPAR_XLINEAR_CORE_1_DEVICE_ID,
};
const unsigned int Linear_core_interrupt_ids[LINEAR_CORES_NUM] = {
	XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_1_INTERRUPT_INTR,
};
volatile static int Linear_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
};
#elif LINEAR_CORES_NUM == 3
const unsigned int Linear_core_device_ids[LINEAR_CORES_NUM] = {
	XPAR_XLINEAR_CORE_0_DEVICE_ID,
	XPAR_XLINEAR_CORE_1_DEVICE_ID,
	XPAR_XLINEAR_CORE_2_DEVICE_ID,
};
const unsigned int Linear_core_interrupt_ids[LINEAR_CORES_NUM] = {
	XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_2_INTERRUPT_INTR,
};
volatile static int Linear_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
};
#elif LINEAR_CORES_NUM == 4
const unsigned int Linear_core_device_ids[LINEAR_CORES_NUM] = {
	XPAR_XLINEAR_CORE_0_DEVICE_ID,
	XPAR_XLINEAR_CORE_1_DEVICE_ID,
	XPAR_XLINEAR_CORE_2_DEVICE_ID,
	XPAR_XLINEAR_CORE_3_DEVICE_ID,
};
const unsigned int Linear_core_interrupt_ids[LINEAR_CORES_NUM] = {
	XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_2_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_3_INTERRUPT_INTR,
};
volatile static int Linear_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
	0,
};
#endif

Core *get_Linear_core() {
#ifdef LINEAR_CORES_NUM
	return Linear_core_list[0];
#else
	return NULL
#endif
}

void Linear_core_isr(Core *core) {
	XLinear_core *pAccelerator = (XLinear_core *) core->InstancePtr;
	// Disable the global interrupt
	XLinear_core_InterruptGlobalDisable(pAccelerator);
	// Disable the local interrupt
	XLinear_core_InterruptDisable(pAccelerator, 0xffffffff);
	// clear the local interrupt
	XLinear_core_InterruptClear(pAccelerator, 1);
	Linear_core_result_avail[core->InstanceId] = 1;
}

int Linear_core_setup_interrupt(Core *core, XScuGic ScuGic) {
	// Connect the Linear_core ISR to the exception table
	int result = XScuGic_Connect(
		&ScuGic,
		Linear_core_interrupt_ids[core->InstanceId],
		(Xil_InterruptHandler) Linear_core_isr,
		core);
	if (result != XST_SUCCESS) return result;
	XScuGic_Enable(&ScuGic, Linear_core_interrupt_ids[core->InstanceId]);
	return XST_SUCCESS;
}

void Linear_core_init(Core *core, XScuGic ScuGic) {
	printf("- Initializing Linear_core %u: ", core->InstanceId);
	core->InstancePtr = (void *) malloc(sizeof(XLinear_core));
	int status = XLinear_core_Initialize(
		core->InstancePtr, Linear_core_device_ids[core->InstanceId]);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	status = Linear_core_setup_interrupt(core, ScuGic);
	if (status != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(XST_FAILURE);
	}
	printf("%sSuccess%s\n", KGRN, KNRM);
}

matrix_t *
	Linear_core_setup(XLinear_core *Linear_core, LayerConf lc, matrix_t *x) {
	printf("- Setup Linear_core: ");
	XLinear_core_Set_x(Linear_core, (unsigned int) (u64) x);
	XLinear_core_Set_weights(Linear_core, (unsigned int) (u64) lc.weightsAddr);
	XLinear_core_Set_bias(Linear_core, (unsigned int) (u64) lc.biasAddr);
	XLinear_core_Set_in_features(Linear_core, lc.inFeatures);
	XLinear_core_Set_out_features(Linear_core, lc.outFeatures);
	XLinear_core_Set_doReLU(Linear_core, lc.doReLU);
	matrix_t *resAddr = (matrix_t *) malloc(lc.resSize * sizeof(matrix_t));
	if (resAddr == NULL) {
		printf(
			"%sError. Not enough memory for Linear_core_setup resAddr.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}
	XLinear_core_Set_res(Linear_core, (unsigned int) (u64) resAddr);
	printf("%sSuccess%s\n", KGRN, KNRM);
	return resAddr;
}

void Linear_core_start(void *InstancePtr) {
	XLinear_core *pAccelerator = (XLinear_core *) InstancePtr;

	printf("- Waiting for Linear_core to get ready: ");
	while (!XLinear_core_IsReady(pAccelerator)) continue;
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Starting Linear_core: ");
	XLinear_core_InterruptEnable(pAccelerator, 1);
	XLinear_core_InterruptGlobalEnable(pAccelerator);
	XLinear_core_Start(pAccelerator);
	printf("%sSuccess%s\n", KGRN, KNRM);
}

int Linear_core_wait_int(Core *core) {
	while (!Linear_core_result_avail[core->InstanceId]) continue;
	Linear_core_result_avail[core->InstanceId] = 0;
	printf("- Interrupt received from Linear_core.\n");
	int status = XLinear_core_Get_return(core->InstancePtr);
	printf("- Return value: %d\n", status);
	return status;
}

matrix_t *Linear_core_process(LayerConf lc, matrix_t *x) {
	Core *core = get_Linear_core();
	matrix_t *res = Linear_core_setup(core->InstancePtr, lc, x);
	Linear_core_start(core->InstancePtr);
	Linear_core_wait_int(core);
	free(x);
	return res;
}

void Linear_conf_params_complete(LayerConf *lc) {
	lc->xSize = lc->inFeatures;
	lc->weightsSize = lc->inFeatures * lc->outFeatures;
	lc->biasSize = lc->outFeatures;
	lc->resSize = lc->outFeatures;
	lc->doReLU = lc->layerType == LINEAR_RELU_LAYER_TYPE ? 1 : 0;

	lc->hw_func = &Linear_core_process;
	lc->sw_func = &Linear_sw;
}

void Linear_conf_complete(
	NetConf *netConf,
	unsigned int layer_index,
	unsigned int params_index) {
	LayerConf *lc = &netConf->layersConf[layer_index];
	if (layer_index > 0) {
		LayerConf *plc = &netConf->layersConf[layer_index - 1];
		switch (plc->layerType) {
			case CONV_LAYER_TYPE:
			case MAXPOOL_LAYER_TYPE:
				lc->inFeatures = plc->dout * plc->hout * plc->wout;
				break;
			case LINEAR_RELU_LAYER_TYPE:
			case LINEAR_LAYER_TYPE: lc->inFeatures = plc->outFeatures; break;
		}
	}
	Linear_conf_params_complete(lc);

	lc->weightsAddr = netConf->params[params_index];
	lc->biasAddr = netConf->params[params_index + 1];
}

int Linear_core_test(unsigned int testAllCores) {
	printf("*** Linear_core_test ***\n");

	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

	if (LINEAR_CORES_NUM <= 0) {
		printf("%s- No Linear_core available!%s\n", KRED, KNRM);
		return XST_DEVICE_NOT_FOUND;
	}

	LayerConf lc = {
		.layerType = LINEAR_RELU_LAYER_TYPE,
		.inFeatures = 9216,
		.outFeatures = 4096,
	};

	Linear_conf_params_complete(&lc);

	printf("- Initializing Memory: ");
	matrix_t *x = (matrix_t *) malloc(lc.xSize * sizeof(matrix_t));
	for (unsigned int i = 0; i < lc.xSize; i++) x[i] = X_VALUE;

	lc.weightsAddr = (matrix_t *) malloc(lc.weightsSize * sizeof(matrix_t));
	for (unsigned int i = 0; i < lc.weightsSize; i++)
		lc.weightsAddr[i] = WEIGHT_VALUE;

	lc.biasAddr = (matrix_t *) malloc(lc.biasSize * sizeof(matrix_t));
	for (unsigned int i = 0; i < lc.biasSize; i++) lc.biasAddr[i] = BIAS_VALUE;
	printf("%sSuccess%s\n", KGRN, KNRM);

	int status = XST_SUCCESS;
	unsigned int core_num = testAllCores ? LINEAR_CORES_NUM : 1;
	for (unsigned int core = 0; core < core_num; core++) {
		printf("- Maxpool_core[%u]: ", core);

		matrix_t *res = (*lc.hw_func)(lc, x);

		matrix_t pixel_value =
			X_VALUE * WEIGHT_VALUE * lc.inFeatures + BIAS_VALUE;
		matrix_t error = 0;

		for (unsigned int i = 0; i < lc.resSize; i++) {
			error += abs(res[i] - pixel_value);
		}

		if (error < 0.1) {
			printf("%sSuccess%s\n", KGRN, KNRM);
		} else {
			printf("%sError %f%s\n", KRED, error, KNRM);
			status = XST_FAILURE;
		}

		free(res);
	}

	free(lc.weightsAddr);
	free(lc.biasAddr);

	return status;
}

// ----------------------------------------------

void layer_conf_complete(NetConf *netConf) {
	unsigned int params_index = 0;
	for (unsigned int i = 0; i < netConf->layersNum; i++) {
		switch (netConf->layersConf[i].layerType) {
			case CONV_LAYER_TYPE:
				Conv_conf_complete(netConf, i, params_index);
				params_index += 2;
				break;
			case MAXPOOL_LAYER_TYPE: Maxpool_conf_complete(netConf, i); break;
			case LINEAR_RELU_LAYER_TYPE:
			case LINEAR_LAYER_TYPE:
				Linear_conf_complete(netConf, i, params_index);
				params_index += 2;
				break;
		}
	}
}

/**
 * Runs the network's forward pass
 * @param[in] params: the network's parameters sets
 * @param[in] x: the input image to get classified
 * @returns a FloatMatrix containing the network's classification estimates,
 * that is an array of matrix_t numbers, one for each class, which shows how
 * likely it is for the input image to be of a class in a logarithmic scale.
 */
matrix_t *forward(NetConf *netConf, matrix_t *x, unsigned int useHW) {
	printf("000/%03d", netConf->layersNum);
	for (unsigned int i = 0; i < netConf->layersNum; i++) {
		printf("\b\b\b\b\b\b\b");
		printf("%03d/%03d", i, netConf->layersNum);
		if (useHW)
			x = (*netConf->layersConf[i].hw_func)(netConf->layersConf[i], x);
		else
			x = (*netConf->layersConf[i].sw_func)(netConf->layersConf[i], x);
	}
	printf("\b\b\b");
	return x;
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
int inference(
	NetConf *netConf,
	unsigned int runForNumImages,
	unsigned int selfCheck) {
	if (runForNumImages == 0 || runForNumImages > netConf->imagesPaths->length)
		runForNumImages = netConf->imagesPaths->length;

	for (unsigned int i = 0; i < runForNumImages; i++) {
		matrix_t *x = loadImage(netConf->imagesPaths->list[i]);
		printf("- %s: ", netConf->imagesPaths->list[i]);

		/** Pass the image through the network */
		matrix_t *x_hw = forward(netConf, x, 1);
		matrix_t *x_sw = NULL;
		if (selfCheck) x_sw = forward(netConf, x, 0);

		/** Find the class with the greatest likelihood and print its label
		 */
		unsigned int xSize =
			netConf->layersConf[netConf->layersNum - 1].resSize;
		unsigned int topClass_hw = argmax(x_hw, xSize);
		unsigned int topClass_sw =
			selfCheck ? argmax(x_sw, xSize) : topClass_hw;

		if (topClass_hw == topClass_sw)
			printf("%s\n", netConf->labels[topClass_sw]);
		else
			printf("%sError. No match between sw and hw.%s\n", KRED, KNRM);

		/**
		 * Free the forward pass's resulting FloatMatrix as it is no longer
		 * needed to avoid memory leaks
		 */
		free(x);
		free(x_hw);
		free(x_sw);
	}
	return XST_SUCCESS;
}

int Network_test() {
	printf("*** Network_test ***\n");

	matrix_t X_VALUE = 1;
	matrix_t WEIGHT_VALUE = .2;
	matrix_t BIAS_VALUE = .1;

	unsigned int LAYERS_NUMBER = 11;

	NetConf netConf;
	netConf.layersNum = 11;

	LayerConf lc[] = {
		{
			.layerType = CONV_LAYER_TYPE,
			.kernelSize = 11,
			.stride = 4,
			.padding = 2,
			.din = 3,
			.hin = 224,
			.win = 224,
			.dout = 64,
		},
		{
			.layerType = MAXPOOL_LAYER_TYPE,
			.kernelSize = 3,
			.stride = 2,
		},
		{
			.layerType = CONV_LAYER_TYPE,
			.kernelSize = 5,
			.stride = 1,
			.padding = 2,
			.dout = 192,
		},
		{
			.layerType = MAXPOOL_LAYER_TYPE,
			.kernelSize = 3,
			.stride = 2,
		},
		{
			.layerType = CONV_LAYER_TYPE,
			.kernelSize = 3,
			.stride = 1,
			.padding = 1,
			.dout = 384,
		},
		{
			.layerType = CONV_LAYER_TYPE,
			.kernelSize = 3,
			.stride = 1,
			.padding = 1,
			.dout = 256,
		},
		{
			.layerType = CONV_LAYER_TYPE,
			.kernelSize = 3,
			.stride = 1,
			.padding = 1,
			.dout = 256,
		},
		{
			.layerType = MAXPOOL_LAYER_TYPE,
			.kernelSize = 3,
			.stride = 2,
		},
		{
			.layerType = LINEAR_RELU_LAYER_TYPE,
			.outFeatures = 4096,
		},
		{
			.layerType = LINEAR_RELU_LAYER_TYPE,
			.outFeatures = 4096,
		},
		{
			.layerType = LINEAR_LAYER_TYPE,
			.outFeatures = 1000,
		},
	};
	netConf.layersConf = lc;
	matrix_t *params[16];
	netConf.params = params;

	layer_conf_complete(&netConf);

	printf("- Initializing Memory: ");
	unsigned int xSize = lc[0].xSize;
	matrix_t *xAddr = (matrix_t *) malloc(xSize * sizeof(matrix_t));
	for (unsigned int i = 0; i < xSize; i++) xAddr[i] = X_VALUE;

	unsigned int params_index = 0;
	for (unsigned int i = 0; i < LAYERS_NUMBER; i++) {
		if (lc[i].layerType == MAXPOOL_LAYER_TYPE) continue;

		params[params_index] =
			(matrix_t *) malloc(lc[i].weightsSize * sizeof(matrix_t));
		for (unsigned int j = 0; j < lc[i].weightsSize; j++) {
			params[params_index][j] = WEIGHT_VALUE;
		}
		params_index++;

		params[params_index] =
			(matrix_t *) malloc(lc[i].biasSize * sizeof(matrix_t));
		for (unsigned int j = 0; j < lc[i].biasSize; j++) {
			params[params_index][j] = BIAS_VALUE;
		}
		params_index++;
	}
	printf("%sSuccess%s\n", KGRN, KNRM);

	printf("- Forwarding...\n");
	matrix_t *res = forward(&netConf, xAddr, 1);
	printf("- %sForwarded Successfully%s\n", KGRN, KNRM);

	for (unsigned int i = 0; i < params_index; i++) free(params[i]);
	free(res);

	return XST_SUCCESS;
}

// ----------------------------------------------

void setup_stdout() {
	setbuf(stdout, NULL); // No printf flushing needed
	printf("\033[2J");	// Clear terminal
	printf("\033[H");	 // Move cursor to the home position
}

void setup_cache() {
	Xil_DCacheDisable();
}

XScuGic setup_interrupt() {
	printf("- Setting up interrupts: ");

	// This functions sets up the interrupt on the ARM
	int result;
	XScuGic_Config *pCfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (pCfg == NULL) {
		printf(
			"%sError: Interrupt Configuration Lookup Failed%s\n", KRED, KNRM);

		print("Interrupt Configuration Lookup Failed\n");
		exit(XST_FAILURE);
	}
	XScuGic ScuGic;

	result = XScuGic_CfgInitialize(&ScuGic, pCfg, pCfg->CpuBaseAddress);
	if (result != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(result);
	}
	// self-test
	result = XScuGic_SelfTest(&ScuGic);
	if (result != XST_SUCCESS) {
		printf("%sError%s\n", KRED, KNRM);
		exit(result);
	}
	// Initialize the exception handler
	Xil_ExceptionInit();
	// Register the exception handler
	// print("Register the exception handler\n\r");
	Xil_ExceptionRegisterHandler(
		XIL_EXCEPTION_ID_INT,
		(Xil_ExceptionHandler) XScuGic_InterruptHandler,
		&ScuGic);
	// Enable the exception handler
	Xil_ExceptionEnable();
	printf("%sSuccess%s\n", KGRN, KNRM);
	return ScuGic;
}

void setup_accelerator(XScuGic ScuGic) {
	for (unsigned int i = 0; i < CONV_CORES_NUM; i++) {
		Conv_core_list[i] = (Core *) malloc(sizeof(Core));
		Conv_core_list[i]->InstanceId = i;
		Conv_core_init(Conv_core_list[i], ScuGic);
	}
	for (unsigned int i = 0; i < MAXPOOL_CORES_NUM; i++) {
		Maxpool_core_list[i] = (Core *) malloc(sizeof(Core));
		Maxpool_core_list[i]->InstanceId = i;
		Maxpool_core_init(Maxpool_core_list[i], ScuGic);
	}
	for (unsigned int i = 0; i < LINEAR_CORES_NUM; i++) {
		Linear_core_list[i] = (Core *) malloc(sizeof(Core));
		Linear_core_list[i]->InstanceId = i;
		Linear_core_init(Linear_core_list[i], ScuGic);
	}
}

void setup_platform(char *greeting_message) {
	setup_stdout();
	printf("%s", greeting_message);
	setup_cache();
	XScuGic ScuGic = setup_interrupt();
	setup_accelerator(ScuGic);
}

#endif /* SRC_PLATFORM_H_ */
