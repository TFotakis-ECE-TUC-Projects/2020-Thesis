#include <math.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct floatMatrix_t {
	uint dimsNum;
	uint *dims;
	float *matrix;
} FloatMatrix;


uint flattenDimensions(FloatMatrix *x) {
	uint xLen = 1;
	for (int i = 0; i < x->dimsNum; i++) {
		xLen *= x->dims[i];
	}
	return xLen;
}


uint argmax(FloatMatrix *x) {
	uint xLen = flattenDimensions(x);
	float max = x->matrix[0];
	uint index = 0;
	for (int i = 1; i < xLen; i++) {
		if (max < x->matrix[i]) {
			max = x->matrix[i];
			index = i;
		}
	}
	return index;
}


uint calc3DIndex(uint *dims, uint i, uint j, uint k) {
	return k + j * dims[2] + i * dims[1] * dims[2];
}


uint calc4DIndex(uint *dims, uint i, uint j, uint k, uint l) {
	return l + k * dims[3] + j * dims[2] * dims[3] + i * dims[1] * dims[2] * dims[3];
}


float *zero3Dto1DMatrix(uint dim1, uint dim2, uint dim3) {
	uint arrLen = dim1 * dim2 * dim3;
	float *arr = (float *) malloc(arrLen * sizeof(float));

	for (int i = 0; i < arrLen; i++) {
		arr[i] = 0;
	}
	return arr;
}


void printX(FloatMatrix *x, char *message) {
	printf("%s\n", message);
	uint xLen = flattenDimensions(x);
	for (int i = 0; i < xLen; i++) {
		printf("index: %u, value: %f, ", i, x->matrix[i]);
	}
	printf("\n\n\n");
}