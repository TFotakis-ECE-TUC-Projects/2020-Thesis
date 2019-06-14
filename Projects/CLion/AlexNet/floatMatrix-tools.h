typedef double matrix_t;


typedef struct floatMatrix_t {
	uint dimsNum;
	uint *dims;
	matrix_t *matrix;
} FloatMatrix;


uint flattenDimensions(FloatMatrix *x) {
	uint xLen = 1;
	for (int i = 0; i < x->dimsNum; i++) xLen *= x->dims[i];
	return xLen;
}


uint argmax(FloatMatrix *x) {
	uint xLen = flattenDimensions(x);
	matrix_t max = x->matrix[0];
	uint index = 0;
	for (int i = 1; i < xLen; i++) {
		if (max >= x->matrix[i]) continue;
		max = x->matrix[i];
		index = i;
	}
	return index;
}


uint calc3DIndex(uint *dims, uint i, uint j, uint k) {
	return k + j * dims[2] + i * dims[1] * dims[2];
}


uint calc4DIndex(uint *dims, uint i, uint j, uint k, uint l) {
	return l + k * dims[3] + j * dims[2] * dims[3] + i * dims[1] * dims[2] * dims[3];
}


matrix_t *zero3Dto1DMatrix(uint dim1, uint dim2, uint dim3) {
	uint arrLen = dim1 * dim2 * dim3;
	matrix_t *arr = (matrix_t *) malloc(arrLen * sizeof(matrix_t));
	for (int i = 0; i < arrLen; i++) arr[i] = 0;
	return arr;
}


FloatMatrix *zero3DFloatMatrix(uint dim1, uint dim2, uint dim3) {
	FloatMatrix *res = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	res->dimsNum = 3;
	res->dims = (uint *) malloc(3 * sizeof(uint));
	res->dims[0] = dim1;
	res->dims[1] = dim2;
	res->dims[2] = dim3;
	res->matrix = zero3Dto1DMatrix(dim1, dim2, dim3);
	return res;
}


FloatMatrix *create1DFloatMatrix(uint dim) {
	FloatMatrix *res = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	res->dimsNum = 1;
	res->dims = (uint *) malloc(1 * sizeof(uint));
	res->dims[0] = dim;
	res->matrix = (matrix_t *) malloc(dim * sizeof(matrix_t));
	return res;
}


void printMatrix(FloatMatrix *x, char *message) {
	printf("%s\n", message);
	uint xLen = flattenDimensions(x);
	for (int i = 0; i < xLen; i++) printf("index: %u | value: %lf\n", i, x->matrix[i]);
	printf("\n\n\n");
}


void printMinMaxSum(FloatMatrix *x, char *message) {
#ifdef SKIP_CHECKING
	return;
#endif
	matrix_t sum = 0;
	matrix_t min = 10000000000000000000.0;
	matrix_t max = -10000000000000000000.0;
	for (int i = 0; i < flattenDimensions(x); i++) {
		sum += x->matrix[i];
		if (isnan(sum)) {
			printf("Sum is nan!\n");
		}
		min = min < x->matrix[i] ? min : x->matrix[i];
		max = max > x->matrix[i] ? max : x->matrix[i];
	}
	printf("%sSum: %lf, Min: %lf, Max: %lf\n", message, sum, min, max);
}


void freeFloatMatrix(FloatMatrix *x) {
	free(x->dims);
	free(x->matrix);
	free(x);
}


FloatMatrix *ImageToFloatMatrix(Image *image) {
	FloatMatrix *x = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	x->dimsNum = 3;
	x->dims = (uint *) malloc(3 * sizeof(uint));
	x->dims[0] = image->depth;
	x->dims[1] = image->height;
	x->dims[2] = image->width;
	x->matrix = (matrix_t *) malloc(image->depth * image->height * image->width * sizeof(matrix_t));
	uint index = 0;
	for (int i = 0; i < image->depth; i++) {
		for (int j = 0; j < image->height; j++) {
			for (int k = 0; k < image->width; k++) {
				x->matrix[index] = image->channels[i][j][k] / 256.0;
				index++;
			}
		}
	}
	syslog(LOG_INFO, "Done converting image to parameters.");
	return x;
}