#include <omp.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <dirent.h>
#include <jpeglib.h>
#include <string.h>

#define WRITE_TRANSFORMED_IMAGE_TO_FILE
#define ENABLE_LOGGING
#define ENABLE_CHECKING

#include "imageUtils.h"
#include "floatMatrix-tools.h"
#include "nn-tools.h"
#include "terminalColors.h"


char *dataDir = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Data/ConvertAlexNet/Cat_Dog_data/test/";
char *parametersPath = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Scripts/AlexNetParametersProcessing/binaryParameters.txt";
char *labelsPath = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Data/ConvertAlexNet/labels.txt";


typedef struct params_t {
	uint len;
	FloatMatrix **matrix;
} Params;


uint timedif(struct timespec *start, struct timespec *stop) {
	return ((int) ((stop->tv_sec - start->tv_sec) * 1000) + (int) ((stop->tv_nsec - start->tv_nsec) / 1000000));
}


char **loadLabels() {
	printf("- Loading labels: ");
	fflush(stdout);

	FILE *f = fopen(labelsPath, "r");

	uint labelsNum;
	fscanf(f, "%u\n", &labelsNum);

	char **labels = (char **) malloc(labelsNum * sizeof(char *));

	for (int i = 0; i < labelsNum; i++) {
		labels[i] = (char *) malloc(150 * sizeof(char));
		fscanf(f, "%[^\n]\n", labels[i]);
	}
	printf("%s✓%s\n", KGRN, KNRM);
	return labels;
}


Params *loadParameters() {
	printf("- Loading parameters: ");
	fflush(stdout);

	FILE *f = fopen(parametersPath, "r");
	Params *params = (Params *) malloc(sizeof(Params));

	fscanf(f, "%u\n", &params->len);
	params->matrix = (FloatMatrix **) malloc(params->len * sizeof(FloatMatrix *));

	for (int p = 0; p < params->len; p++) {
		FloatMatrix *matrix = (FloatMatrix *) malloc(sizeof(FloatMatrix));

		fscanf(f, "%u", &matrix->dimsNum);
		matrix->dims = (uint *) malloc(matrix->dimsNum * sizeof(uint));
		for (int i = 0; i < matrix->dimsNum; i++) {
			fscanf(f, "%d", &matrix->dims[i]);
		}

		uint rows = 1;
		for (int i = 0; i < matrix->dimsNum - 1; i++) {
			rows *= matrix->dims[i];
		}
		uint columns = matrix->dims[matrix->dimsNum - 1];
		matrix->matrix = (matrix_t *) malloc(rows * columns * sizeof(matrix_t));
		uint index = 0;
		for (uint i = 0; i < rows; i++) {
			for (uint j = 0; j < columns; j++) {
				fscanf(f, "%lf", &matrix->matrix[index]);
				index++;
			}
		}
		params->matrix[p] = matrix;
	}
	printf("%s✓%s\n", KGRN, KNRM);
	return params;
}


Image *imageTransform(char *path) {
	Image *image = (Image *) malloc(sizeof(Image));
	image->path = path;

	readFileAndDecompress(image);

	convertToMatrix(image);

	Image *newImage = imageResize(image, 256);
	if (newImage != image) freeImage(image);
	image = newImage;

	newImage = imageCenterCrop(image, 224);
	freeImage(image);
	image = newImage;

#ifdef WRITE_TRANSFORMED_IMAGE_TO_FILE
	writeToFile(image);
#endif

	syslog(LOG_INFO, "Done image manipulation.");
	return image;
}


FloatMatrix *forward(Params *params, FloatMatrix *x) {
	x = Conv2d(x, params->matrix[0], params->matrix[1], 3, 64, 11, 4, 2, "Conv2d 1");
	x = ReLU(x, "ReLU 1");
	x = MaxPool2d(x, 3, 2, "MaxPool2d 1");

	x = Conv2d(x, params->matrix[2], params->matrix[3], 64, 192, 5, 1, 2, "Conv2d 2");
	x = ReLU(x, "ReLU 2");
	x = MaxPool2d(x, 3, 2, "MaxPool2d 2");

	x = Conv2d(x, params->matrix[4], params->matrix[5], 192, 384, 3, 1, 1, "Conv2d 3");
	x = ReLU(x, "ReLU 3");

	x = Conv2d(x, params->matrix[6], params->matrix[7], 384, 256, 3, 1, 1, "Conv2d 4");
	x = ReLU(x, "ReLU 4");

	x = Conv2d(x, params->matrix[8], params->matrix[9], 256, 256, 3, 1, 1, "Conv2d 5");
	x = ReLU(x, "ReLU 5");
	x = MaxPool2d(x, 3, 2, "MaxPool2d 5");

	x = Linear(x, params->matrix[10], params->matrix[11], 256 * 6 * 6, 4096, "Linear 6");
	x = ReLU(x, "ReLU 6");

	x = Linear(x, params->matrix[12], params->matrix[13], 4096, 4096, "Linear 7");
	x = ReLU(x, "ReLU 7");

	x = Linear(x, params->matrix[14], params->matrix[15], 4096, 1000, "Linear 8");

	x = LogSoftMax(x, "LogSoftMax");

	syslog(LOG_INFO, "Done forward pass.");
	return x;
}


uint inference(char *path, Params *params, char **labels) {
	struct timespec startTime, endTime;

	Image *image = imageTransform(path);
	FloatMatrix *x = ImageToFloatMatrix(image);
	freeImageChannels(image);
	free(image);

	clock_gettime(CLOCK_REALTIME, &startTime);

	x = forward(params, x);

	clock_gettime(CLOCK_REALTIME, &endTime);

	uint timeNeeded = timedif(&startTime, &endTime);

	uint topClass = argmax(x);
	printf("* Image contains: %s%s%s\n\tResult in:\t\t\t\t%d ms\n", KGRN, labels[topClass], KNRM, timeNeeded);

	freeFloatMatrix(x);
	return timeNeeded;
}


int main(int argc, char *argv[]) {
#ifdef ENABLE_LOGGING
	char *syslog_prefix = (char *) malloc(1024);
	sprintf(syslog_prefix, "%s", argv[0]);
	openlog(syslog_prefix, LOG_PERROR | LOG_PID, LOG_USER);
#endif

//	FloatMatrix *tmp = zero3DFloatMatrix(10, 10, 10);
//	argmax(tmp);
//	freeFloatMatrix(tmp);
//	return EXIT_SUCCESS;

	if (argc == 2) dataDir = argv[1];

	Filelist *fileList = getFileList(dataDir);
	printf("- Loading file list: %s✓%s\n", KGRN, KNRM);
	char **labels = loadLabels();
	Params *params = loadParameters();
	printf("- %sStarting inference...\n\n%s", KBLU, KNRM);

	uint sumTime = 0;
#ifdef _OMP_H
	//#pragma omp parallel for
#endif
	for (int i = 0; i < fileList->length; i++) {
		printf("* Processing image: %s\n", fileList->list[i]);
		sumTime += inference(fileList->list[i], params, labels);
		printf("\tAverage time per image:\t%d ms\n\tImages Processed:\t\t%d\n\n", sumTime / (i + 1), i + 1);
	}

	return EXIT_SUCCESS;
}