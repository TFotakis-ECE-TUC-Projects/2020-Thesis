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


/** Flags used for debugging */
/** Write every processed image to an output.ppm file to view what the network sees */
//#define WRITE_TRANSFORMED_IMAGE_TO_FILE

/** Enable log messages to be printed */
//#define ENABLE_LOGGING

/** Enable checking of each layer's output matrix by printing matrix's Min, Max, Sum */
//#define ENABLE_CHECKING

/** Enable multiprocessing using OpenMP library */
#define ENABLE_OPENMP

#ifdef ENABLE_OPENMP
#include <omp.h>
#endif
#include "imageUtils.h"
#include "floatMatrix-tools.h"
#include "nn-tools.h"
#include "terminalColors.h"


/**
 * The folder in which the images to be passed through the network exist.
 * On the dataDir should contain folders named after the label of the images
 * they contain.
 */
char *dataDir = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Data/ConvertAlexNet/Cat_Dog_data/test/";

/**
 * The path where the parameters file exists. The Parameters file contains the
 * network's weights and biases
 */
char *parametersPath = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Scripts/AlexNetParametersProcessing/binaryParameters.txt";

/**
 * The path where the labels file exists. The labels file contains the labels
 * of the classes the network is capable of recognizing.
 */
char *labelsPath = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Data/ConvertAlexNet/labels.txt";


/** Structure to store the network's parameters */
typedef struct params_t {
	uint len; /**< The number of parameter matrices */
	FloatMatrix **matrix; /**< The 1D array of FloatMartix cells to store the parameters */
} Params;


/**
 * Calculates the time difference in milliseconds between the given start and
 * stop timestamps.
 * @param[in] start: The timestamp when the action started
 * @param[in] stop: The timestamp when the action ended
 * @returns the difference in milliseconds between the given timestamps
 */
uint timedif(struct timespec *start, struct timespec *stop) {
	/**
	 * Get the time difference in seconds and convert it to milliseconds.
	 * Get the time difference in nanoseconds and convert it to milliseconds.
	 * Add the above number to get the total milliseconds difference.
	 */
	return ((int) ((stop->tv_sec - start->tv_sec) * 1000) + (int) ((stop->tv_nsec - start->tv_nsec) / 1000000));
}


/**
 * Loads the network's classes' labels using the file defined on the labelsPath
 * global variable.
 * @returns an array of strings containing the loaded labels.
 */
char **loadLabels() {
	printf("- Loading labels: ");
	fflush(stdout);

	/** Open the labels file */
	FILE *f = fopen(labelsPath, "r");

	/** Read the number of labels contained in this file */
	uint labelsNum;
	fscanf(f, "%u\n", &labelsNum);

	/** Allocate the needed memory to store the labels array */
	char **labels = (char **) malloc(labelsNum * sizeof(char *));

	/** For every label in the file */
	for (int i = 0; i < labelsNum; i++) {
		/** Allocate memory for each label string */
		labels[i] = (char *) malloc(150 * sizeof(char));

		/** Read the label and store it to the allocated string */
		fscanf(f, "%[^\n]\n", labels[i]);
	}
	printf("%s✓%s\n", KGRN, KNRM);
	return labels;
}


/**
 * Loads the network's parameters using the file defined on the parametersPath
 * global variable.
 * @returns a Params structure pointer containing the loaded network parameters.
 */
Params *loadParameters() {
	printf("- Loading parameters: ");
	fflush(stdout);

	/** Open the parameters file */
	FILE *f = fopen(parametersPath, "r");

	/** Allocate the needed memory to store the Params structure */
	Params *params = (Params *) malloc(sizeof(Params));

	/** Get the number of parameters sets existing in this file */
	fscanf(f, "%u\n", &params->len);

	/**
	 * Allocate the needed memory to store all the FloatMatrix structures
	 * (one for each parameters set)
	 */
	params->matrix = (FloatMatrix **) malloc(params->len * sizeof(FloatMatrix *));

	/** For every parameters set */
	for (int p = 0; p < params->len; p++) {
		/** Allocate the needed memory to store the FloatMatrix structure */
		FloatMatrix *matrix = (FloatMatrix *) malloc(sizeof(FloatMatrix));

		/** Get the number of dimensions this parameters set consists of */
		fscanf(f, "%u", &matrix->dimsNum);

		/** Allocate the needed memory to store the dimensions sizes */
		matrix->dims = (uint *) malloc(matrix->dimsNum * sizeof(uint));

		/** For every dimension of this parameters set */
		for (int i = 0; i < matrix->dimsNum; i++) {
			/** Read the dimension's size */
			fscanf(f, "%d", &matrix->dims[i]);
		}

		/** Calculate the total parameters number */
		uint xLen = flattenDimensions(matrix);

		/** Allocate the needed memory to store all the parameters */
		matrix->matrix = (matrix_t *) malloc(xLen * sizeof(matrix_t));

		/** For every parameter */
		for (int i = 0; i < xLen; i++) {
			/** Read parameter */
			fscanf(f, "%lf", &matrix->matrix[i]);
		}

		/** Assign matrix to corresponding parameters set index */
		params->matrix[p] = matrix;
	}
	printf("%s✓%s\n", KGRN, KNRM);
	return params;
}


/**
 * Reads the given image path's jpeg file and transforms (resizes & crops) to
 * fit the network's specifications.
 * @param[in] path: The path where the image exists
 * @returns an Image structure containing the image's data
 */
Image *imageTransform(char *path) {
	/** Allocate the Image structure's memory */
	Image *image = (Image *) malloc(sizeof(Image));

	/** Copy the path to the image's path field */
	image->path = path;

	/** Read the jpeg file and decompress it */
	readFileAndDecompress(image);

	/** Convert the Image's bmp_buffer field to a channels representation */
	convertToMatrix(image);

	/** Resize the image to as close as possible of the 256 x 256 pixels */
	Image *newImage = imageResize(image, 256);

	/** If the resizing took place free the old image to avoid memory leaks */
	if (newImage != image) freeImage(image);

	/** Assign the newImage to the current image */
	image = newImage;

	/** Crop the image to 224 x 224 pixels */
	newImage = imageCenterCrop(image, 224);

	/** Free the old image to avoid memory leaks */
	freeImage(image);

	/** Assign the newImage to the current image */
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
#ifdef ENABLE_OPENMP
	//#pragma omp parallel for
#endif
	for (int i = 0; i < fileList->length; i++) {
		printf("* Processing image: %s\n", fileList->list[i]);
		sumTime += inference(fileList->list[i], params, labels);
		printf("\tAverage time per image:\t%d ms\n\tImages Processed:\t\t%d\n\n", sumTime / (i + 1), i + 1);
	}

	return EXIT_SUCCESS;
}