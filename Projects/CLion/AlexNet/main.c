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

/**
 * Use multiprocessing with the OpenMP library.
 * Assign to ENABLE_OPENMP flag the values 0, 1 or 2 corresponding to the
 * actions described below.
 *
 * 0: No multiprocessing
 *
 * 1: Layer level multiprocessing (Process in parallel Conv2d, MaxPool2d and
 * Linear network's operations)
 *
 * 2: Image level multiprocessing (Pass through the network in parallel
 * multiple images)
 */
#define ENABLE_OPENMP 1


/** Include omp.h only if OpenMP is used */
#if ENABLE_OPENMP != 0

#include <omp.h>

#endif


/**
 * Define to use pretransformed images in .ppm ASCII files, resized to 224x224^ and cropped to 224x244.
 */
#define USE_PRETRANSFORMED_IMAGES


//#define PRINT_TIMINGS


#include "imageUtils.h"
#include "floatMatrix-tools.h"
#include "nn-tools.h"
#include "terminalColors.h"


/**
 * The folder in which the images to be passed through the network exist.
 * On the dataDir should contain folders named after the label of the images
 * they contain.
 */
#ifdef USE_PRETRANSFORMED_IMAGES
char *dataDir = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Data/ConvertAlexNet/Cat_Dog_data/pretransformed/test/";
#else
char *dataDir = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Data/ConvertAlexNet/Cat_Dog_data/test/";
#endif


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
	return ((int) ((stop->tv_sec - start->tv_sec) * 1000) +
	        (int) ((stop->tv_nsec - start->tv_nsec) / 1000000));
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
	image->path = (char *) malloc(strlen(path) * sizeof(char));
	strcpy(image->path, path);

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


FloatMatrix *loadPretransformedImage(char *path) {
	FILE *f = fopen(path, "r");
	fscanf(f, "P6\n224 224\n255\n");

	FloatMatrix *x = zero3DFloatMatrix(3, 224, 224);
	for (int w = 0; w < 224; w++) {
		for (int h = 0; h < 224; h++) {
			for (int c = 0; c < 3; c++) {
				unsigned int color;
				fscanf(f, "%u", &color);
				uint index = calc3DIndex(x->dims, c, h, w);
				x->matrix[index] = color / 255.0;
			}
		}
	}
	return x;
}


/**
 * Runs the network's forward pass
 * @param[in] params: the network's parameters sets
 * @param[in] x: the input image to get classified
 * @returns a FloatMatrix containing the network's classification estimates,
 * that is an array of matrix_t numbers, one for each class, which shows how
 * likely it is for the input image to be of a class in a logarithmic scale.
 */
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


/**
 * Takes an image and passes it through the network to classify it and print its
 * label. It also measures the time in milliseconds for the forward pass to
 * complete.
 * @param[in] path: The image's path to pass the network
 * @param[in] params: The network's parameters
 * @param[in] labels: The network's classes' labels
 * @returns the time needed in milliseconds for the forward pass to complete.
 */
uint inference(char *path, Params *params, char **labels) {
	/** Define the timestamps */
	struct timespec startTime, endTime;

#ifdef USE_PRETRANSFORMED_IMAGES
	FloatMatrix *x = loadPretransformedImage(path);
#else
	/**
	 * Get the path's jpeg image file transformed and ready for the network's
	 * specifications
	 */
	Image *image = imageTransform(path);

	/**
	 * Convert the transformed image to a FloatMatrix representation, which the
	 * network can work with
	 */
	FloatMatrix *x = ImageToFloatMatrix(image);

	/** Free the now useless image structure */
	freeImage(image);
#endif

	/** Get the starting timestamp */
	clock_gettime(CLOCK_REALTIME, &startTime);

	/** Pass the image through the network */
	x = forward(params, x);

	/** Get the finishing timestamp */
	clock_gettime(CLOCK_REALTIME, &endTime);

	/** Calculate the time needed for the forward pass to complete */
	uint timeNeeded = timedif(&startTime, &endTime);

	/** Find the class with the greatest likelihood and print its label */
	uint topClass = argmax(x);
	printf("* Processed image: %s\n"
	       "\tImage contains: %s%s%s\n", path, KGRN, labels[topClass], KNRM);
#ifdef PRINT_TIMINGS
	printf("\tResult in:\t\t\t\t%d ms\n", timeNeeded);
#endif
	/**
	 * Free the forward pass's resulting FloatMatrix as it is no longer needed
	 * to avoid memory leaks
	 */
	freeFloatMatrix(x);
	return timeNeeded;
}


/**
 * Classifies the images contained in the dataDir variable or in the given
 * arguments when calling this program. Prints the class each image is
 * classified to and the time needed for the classification process to complete.
 * @param[in] argc: the arguments number
 * @param[in] argv: the program's arguments
 * @returns EXIT_SUCCESS (0) if no issue occured.
 */
int main(int argc, char *argv[]) {
#ifdef ENABLE_LOGGING
	/** Enable logging for debugging */
	/** Allocate memory to hold this program's path */
	char *syslog_prefix = (char *) malloc(1024);
	/** Copy this program's path */
	sprintf(syslog_prefix, "%s", argv[0]);
	/** Enable logging */
	openlog(syslog_prefix, LOG_PERROR | LOG_PID, LOG_USER);
#endif

	/** If one argument given use it as a dataDir path */
	if (argc == 2) dataDir = argv[1];

	/** Get all files' paths contained in dataDir */
	Filelist *fileList = getFileList(dataDir);
	printf("- Loading file list: %s✓%s\n", KGRN, KNRM);

	/** Load network's labels using the labelsPath's file */
	char **labels = loadLabels();

	/** Load networks parameters using the parametersPath's file */
	Params *params = loadParameters();

	printf("- %sStarting inference...\n\n%s", KBLU, KNRM);

	/** Initialize the total time needed to process all files */
	uint sumTime = 0;

#if ENABLE_OPENMP == 2
	/** Parallel inferences using multiple images. */
	#pragma omp parallel for
#endif
	/** For every image in file list */
	for (int i = 0; i < fileList->length; i++) {
		/**
		 * Pass the image through the network and add the time needed for the
		 * network's completion to the total time. Finally, print the average
		 * time needed per image and the number of images that have been
		 * processed.
		 */
		sumTime += inference(fileList->list[i], params, labels);

#if ENABLE_OPENMP == 1
	#ifdef PRINT_TIMINGS
		/** This is valid only for no or for layer level multiprocessing */
		printf("\tAverage time per image:\t%d ms\n", sumTime / (i + 1));
		printf("\tImages Processed:\t\t%d\n\n", i + 1);
	#endif
#endif
#if ENABLE_OPENMP == 2
		printf("\n");
#endif
	}

	return EXIT_SUCCESS;
}