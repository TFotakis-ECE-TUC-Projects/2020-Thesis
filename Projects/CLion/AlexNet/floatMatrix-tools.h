/** Matrix data type for easy switching when needed to other types */
typedef double matrix_t;


/**
 * Struct to store a multi-dimensional matrix of type matrix_t in a buffer-like
 * representation.
 */
typedef struct floatMatrix_t {
	uint dimsNum; /**< Size of dims array */
	uint *dims; /**< Stores the size of each dimension */
	/**
	 * Stores the matrix_t data as a 1D buffer by flattening every dimension.
	 *
	 * Its size is defined by:
	 * dims[0] * dims[1] * dims[2] * ... * dims[dimsNum -1] * sizeof(matrix_t)
	 *
	 * To access data from the i-th plain, j-th row, k-th column calculate an
	 * index as follows:
	 * index = k + j * dims[2] + i * dims[1] * dims[2]
	 *
	 * For ease of calculating indices use calc3DIndex or calc4DIndex.
	 */
	matrix_t *matrix;
} FloatMatrix;


/**
 * Calculates the total cells number of a multi-dimensional matrix
 * @param[in] x: The matrix to calculate its total cells number
 * @returns The total cells
 */
uint flattenDimensions(FloatMatrix *x) {
	/** Multiplies every dimension size to get the total cells */
	uint xLen = 1;
	for (int i = 0; i < x->dimsNum; i++) xLen *= x->dims[i];
	return xLen;
}


/**
 * Finds the max value in a matrix and returns its index as if it was 1D
 * @param[in] x
 * @returns the 1-dimensional index of the highest valued cell
 */
uint argmax(FloatMatrix *x) {
	/** Get the total number of cells in matrix x */
	uint xLen = flattenDimensions(x);

	/** Initialize max to the matrix's first cell */
	matrix_t max = x->matrix[0];

	/** Initialize the highest valued max index to be the first of the matrix */
	uint index = 0;
	for (int i = 1; i < xLen; i++) {
		/** If max greater than the current value ignore and continue */
		if (max >= x->matrix[i]) continue;

		/** If current max is less than current value update it and its index */
		max = x->matrix[i];
		index = i;
	}
	return index;
}


/**
 * Calculates the index on a 1D buffer representation of a 3D matrix
 * @param[in] dims: the sizes of the matrix's dimensions
 * @param[in] i: The plain dimension
 * @param[in] j: The row dimension
 * @param[in] k: The column dimension
 * @returns the 1D representation's index of a 3D matrix
 */
uint calc3DIndex(uint *dims, uint i, uint j, uint k) {
	return k + j * dims[2] + i * dims[1] * dims[2];
}


/**
 * Calculates the index on a 1D buffer representation of a 4D matrix
 * @param[in] dims: the sizes of the matrix's dimensions
 * @param[in] i: The forth dimension
 * @param[in] j: The plain dimension
 * @param[in] k: The row dimension
 * @param[in] l: The column dimension
 * @returns the 1D representation's index of a 4D matrix
 */
uint calc4DIndex(uint *dims, uint i, uint j, uint k, uint l) {
	return l + k * dims[3] + j * dims[2] * dims[3] + i * dims[1] * dims[2] * dims[3];
}


/**
 * Allocates and initializes with zeros a 3D matrix's matrix buffer with
 * dimensions: dim1 x dim2 x dim3
 * @param[in] dim1: the plain dimension
 * @param[in] dim2: the row dimension
 * @param[in] dim3: the column dimension
 * @returns the pointer of the allocated and initialized matrix buffer
 */
matrix_t *zero3Dto1DMatrix(uint dim1, uint dim2, uint dim3) {
	/** Calculate the buffer's size */
	uint arrLen = dim1 * dim2 * dim3;

	/** Allocate needed memory */
	matrix_t *arr = (matrix_t *) malloc(arrLen * sizeof(matrix_t));

	/** Initialize buffer with zeros */
	for (int i = 0; i < arrLen; i++) arr[i] = 0;
	return arr;
}


/**
 * Allocates and initializes a 3D FloatMatrix with zeros
 * @param[in] dim1: the plain dimension
 * @param[in] dim2: the row dimension
 * @param[in] dim3: the column dimension
 * @returns the pointer of the allocated and initialized FloatMatrix
 */
FloatMatrix *zero3DFloatMatrix(uint dim1, uint dim2, uint dim3) {
	/** Allocate needed memory for FloatMatrix struct */
	FloatMatrix *res = (FloatMatrix *) malloc(sizeof(FloatMatrix));

	/** Initialize matrix's dimensions */
	res->dimsNum = 3;
	res->dims = (uint *) malloc(3 * sizeof(uint));
	res->dims[0] = dim1;
	res->dims[1] = dim2;
	res->dims[2] = dim3;

	/** Initialize matrix's buffer */
	res->matrix = zero3Dto1DMatrix(dim1, dim2, dim3);
	return res;
}


/**
 * Allocates and initializes dimensions of a 1-dimensional FloatMatrix
 * @param[in] dim: The matrix's size
 * @returns the pointer of the allocated and initialized FloatMatrix
 */
FloatMatrix *create1DFloatMatrix(uint dim) {
	/** Allocate needed memory for FloatMatrix struct */
	FloatMatrix *res = (FloatMatrix *) malloc(sizeof(FloatMatrix));

	/** Initialize matrix's dimensions */
	res->dimsNum = 1;
	res->dims = (uint *) malloc(1 * sizeof(uint));
	res->dims[0] = dim;

	/** Allocate matrix's buffer */
	res->matrix = (matrix_t *) malloc(dim * sizeof(matrix_t));
	return res;
}


/**
 * Prints out a multidimensional matrix into a 1D representation
 * @param[in] x: the matrix to be printed
 */
void printMatrix(FloatMatrix *x) {
	uint xLen = flattenDimensions(x);
	for (int i = 0; i < xLen; i++) printf("index: %u | value: %lf\n", i, x->matrix[i]);
	printf("\n\n\n");
}


/**
 * Calculates and prints out a matrix's Global sum, min and max values
 * @param[in] x: the matrix to get processed
 */
void printMinMaxSum(FloatMatrix *x) {
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
	printf("Sum: %lf, Min: %lf, Max: %lf\n", sum, min, max);
}


/**
 * Frees given FloatMatrix's dims and matrix fields and the given struct pointer
 * @param[in] x: the FloatMatrix to free up
 */
void freeFloatMatrix(FloatMatrix *x) {
	free(x->dims);
	free(x->matrix);
	free(x);
}


/**
 * Converts an Image_t struct to a FloatMatrix
 * @param[in] image: the Image_t struct to get converted
 * @returns a pointer of the given image's new FloatMatrix representation
 */
FloatMatrix *ImageToFloatMatrix(Image *image) {
	/** Allocate needed memory to hold the FloatMatrix struct */
	FloatMatrix *x = (FloatMatrix *) malloc(sizeof(FloatMatrix));

	/** Initialize FloatMatrix's dimensions */
	x->dimsNum = 3;
	x->dims = (uint *) malloc(3 * sizeof(uint));
	x->dims[0] = image->depth;
	x->dims[1] = image->height;
	x->dims[2] = image->width;

	/** Allocate matrix's buffer memory to store the given image */
	x->matrix = (matrix_t *) malloc(image->depth * image->height * image->width * sizeof(matrix_t));

	/** Initialize buffer's index */
	uint index = 0;

	/** For every color channel */
	for (int i = 0; i < image->depth; i++) {
		/** For every row */
		for (int j = 0; j < image->height; j++) {
			/** For every pixel in a row */
			for (int k = 0; k < image->width; k++) {
				/**
				 * Convert pixel with value range [0, 255] to [0, 1] range and
				 * copy to new FloatMatrix's matrix buffer
				 */
				x->matrix[index] = image->channels[i][j][k] / 256.0;

				/** Increase matrix's buffer indedx */
				index++;
			}
		}
	}
	syslog(LOG_INFO, "Done converting image to parameters.");
	return x;
}