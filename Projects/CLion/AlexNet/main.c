#include <omp.h>
#include <time.h>
#include "imageUtils.h"
#include "terminalColors.h"
#include "floatMatrix-tools.h"
#include "nn-tools.h"

char *dataDir = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Data/ConvertAlexNet/Cat_Dog_data/test/";
char *parametersPath = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Scripts/AlexNetParametersProcessing/binaryParameters.txt";
char *labelsPath = "/home/tzanis/Workspace/Thesis/Projects/PyCharm/Data/ConvertAlexNet/labels.txt";


typedef struct params_t {
	uint parametersNum;
	FloatMatrix *parametersWeights;
} Params;


uint timedif(struct timespec *start, struct timespec *stop) {
	return ((int) ((stop->tv_sec - start->tv_sec) * 1000) + (int) ((stop->tv_nsec - start->tv_nsec) / 1000000));
}


uint sumTime = 0;


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

	Params *params = (Params *) malloc(sizeof(Params));

	FILE *f = fopen(parametersPath, "r");
	uint parametersNum;
	fscanf(f, "%u\n", &parametersNum);

	params->parametersNum = parametersNum;
	params->parametersWeights = (FloatMatrix *) malloc(parametersNum * sizeof(FloatMatrix));

	for (int p = 0; p < parametersNum; p++) {
		uint dimsNum;
		fscanf(f, "%u", &dimsNum);

		params->parametersWeights[p].dimsNum = dimsNum;
		params->parametersWeights[p].dims = (uint *) malloc(dimsNum * sizeof(uint));

		uint *dims = (uint *) malloc(dimsNum * sizeof(uint));
		for (int i = 0; i < dimsNum; i++) {
			fscanf(f, "%d", &dims[i]);
			params->parametersWeights[p].dims[i] = dims[i];
		}

		uint rows = 1;
		for (int i = 0; i < dimsNum - 1; i++) {
			rows *= dims[i];
		}
		uint columns = dims[dimsNum - 1];

		float *arr = (float *) malloc(rows * columns * sizeof(float));

		uint index = 0;
		uint len = rows * columns;
		for (uint i = 0; i < rows; i++) {
			for (uint j = 0; j < columns; j++) {
				fscanf(f, "%f", &arr[index]);
				index++;
			}
		}
		params->parametersWeights[p].matrix = arr;
	}
	printf("%s✓%s\n", KGRN, KNRM);
	return params;
}


Image *imageTransform(char *path) {
	Image *image = (Image *) malloc(sizeof(Image));
	image->filename = path;

	readFileAndDecompress(image);

	convertToMatrix(image);

	Image *newImage = imageResize(image, 256);
	if (newImage != image) {
		free(image->bmp_buffer);
		for (int i = 0; i < image->depth; i++) {
			for (int j = 0; j < image->height; j++) {
				free(image->channels[i][j]);
			}
			free(image->channels[i]);
		}
		free(image);
	}
	image = newImage;
	newImage = imageCenterCrop(image, 224);
	for (int i = 0; i < image->depth; i++) {
		for (int j = 0; j < image->height; j++) {
			free(image->channels[i][j]);
		}
		free(image->channels[i]);
	}
	free(image);
	image = newImage;

//	writeToFile(image);

	syslog(LOG_INFO, "Done image manipulation.");
	return image;
}


FloatMatrix *forward(Params *params, Image *image) {
	FloatMatrix *x = ImageToFloatMatrix(image);

	x = Conv2d(x, params->parametersWeights[0], params->parametersWeights[1], 3, 64, 11, 4, 2);
	x = ReLU(x);
	x = MaxPool2d(x, 3, 2);

	x = Conv2d(x, params->parametersWeights[2], params->parametersWeights[3], 64, 192, 5, 1, 2);
	x = ReLU(x);
	x = MaxPool2d(x, 3, 2);

	x = Conv2d(x, params->parametersWeights[4], params->parametersWeights[5], 192, 384, 3, 1, 1);
	x = ReLU(x);

	x = Conv2d(x, params->parametersWeights[6], params->parametersWeights[7], 384, 256, 3, 1, 1);
	x = ReLU(x);

	x = Conv2d(x, params->parametersWeights[8], params->parametersWeights[9], 256, 256, 3, 1, 1);
	x = ReLU(x);
	x = MaxPool2d(x, 3, 2);

	x = Linear(x, params->parametersWeights[10], params->parametersWeights[11], 256 * 6 * 6, 4096);
	x = ReLU(x);

	x = Linear(x, params->parametersWeights[12], params->parametersWeights[13], 4096, 4096);
	x = ReLU(x);

	x = Linear(x, params->parametersWeights[14], params->parametersWeights[15], 4096, 1000);

	x = LogSoftMax(x, 1);

	syslog(LOG_INFO, "Done forward pass.");
	return x;
}


void inference(char *path, Params *params, char **labels) {
	struct timespec startTime, endTime;

	Image *image = imageTransform(path);

	clock_gettime(CLOCK_REALTIME, &startTime);

	FloatMatrix *x = forward(params, image);

	clock_gettime(CLOCK_REALTIME, &endTime);

	uint topClass = argmax(x);

	uint timeNeeded = timedif(&startTime, &endTime);
	sumTime += timeNeeded;

	syslog(LOG_INFO, "- Image contains: %s%s%s\n\tResult in: %dms\n", KGRN, labels[topClass], KNRM, timeNeeded);
	printf("- Image contains: %s%s%s\n\tResult in:\t\t\t\t%dms\n", KGRN, labels[topClass], KNRM, timeNeeded);

	for (int i = 0; i < image->depth; i++) {
		for (int j = 0; j < image->height; j++) {
			free(image->channels[i][j]);
		}
		free(image->channels[i]);
	}
	free(image->channels);
	free(image);
	free(x->matrix);
	free(x);
}


int main(int argc, char *argv[]) {
	//	char *syslog_prefix = (char *) malloc(1024);
	//	sprintf(syslog_prefix, "%s", argv[0]);
	//	openlog(syslog_prefix, LOG_PERROR | LOG_PID, LOG_USER);

	//	if (argc != 2) {
	//		fprintf(stderr, "USAGE: %s filename.jpg\n", argv[0]);
	//		exit(EXIT_FAILURE);
	//	}

	Filelist *fileList = getFileList(dataDir);
	syslog(LOG_INFO, "Found %u images.", fileList->length);

	char **labels = loadLabels();
	Params *params = loadParameters();

//#pragma omp parallel for
	for (int i = 0; i < fileList->length; i++) {
		inference(fileList->list[i], params, labels);
		printf("\tAverage time per image:\t%d ms\n\n", sumTime / (i + 1));
	}

	return EXIT_SUCCESS;
}