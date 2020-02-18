#ifndef SRC_FLOATMATRIX_UTILS_H_
#define SRC_FLOATMATRIX_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <xstatus.h>
#include "platform_conf.h"
#include "terminal_colors.h"

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

/**
 * Struct to store a multi-dimensional matrix of type matrix_t in a buffer-like
 * representation.
 */
typedef struct {
	unsigned int dimsNum; /**< Size of dims array */
	unsigned int *dims;   /**< Stores the size of each dimension */
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
unsigned int flattenDimensions(FloatMatrix *x) {
	/** Multiplies every dimension size to get the total cells */
	unsigned int xLen = 1;
	for (unsigned int i = 0; i < x->dimsNum; i++) xLen *= x->dims[i];
	return xLen;
}

/**
 * Finds the max value in a matrix and returns its index as if it was 1D
 * @param[in] x
 * @returns the 1-dimensional index of the highest valued cell
 */
unsigned int argmax(FloatMatrix *x) {
	/** Get the total number of cells in matrix x */
	unsigned int xLen = flattenDimensions(x);

	/** Initialize max to the matrix's first cell */
	matrix_t max = x->matrix[0];

	/** Initialize the highest valued max index to be the first of the matrix */
	unsigned int index = 0;
	for (unsigned int i = 1; i < xLen; i++) {
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
unsigned int calc3DIndex(
	unsigned int *dims,
	unsigned int i,
	unsigned int j,
	unsigned int k) {
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
unsigned int calc4DIndex(
	unsigned int *dims,
	unsigned int i,
	unsigned int j,
	unsigned int k,
	unsigned int l) {
	return l + k * dims[3] + j * dims[2] * dims[3] +
		i * dims[1] * dims[2] * dims[3];
}

/**
 * Allocates and initializes with zeros a 3D matrix's matrix buffer with
 * dimensions: dim1 x dim2 x dim3
 * @param[in] dim1: the plain dimension
 * @param[in] dim2: the row dimension
 * @param[in] dim3: the column dimension
 * @returns the pointer of the allocated and initialized matrix buffer
 */
matrix_t *
	zero3Dto1DMatrix(unsigned int dim1, unsigned int dim2, unsigned int dim3) {
	/** Calculate the buffer's size */
	unsigned int arrLen = dim1 * dim2 * dim3;

	/** Allocate needed memory */
	matrix_t *arr = (matrix_t *) malloc(arrLen * sizeof(matrix_t));
	if (arr == NULL) {
		printf(
			"%sError. Not enough memory for zero3Dto1DMatrix arr.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	/** Initialize buffer with zeros */
	for (unsigned int i = 0; i < arrLen; i++) arr[i] = 0;
	return arr;
}

/**
 * Allocates and initializes a 3D FloatMatrix with zeros
 * @param[in] dim1: the plain dimension
 * @param[in] dim2: the row dimension
 * @param[in] dim3: the column dimension
 * @returns the pointer of the allocated and initialized FloatMatrix
 */
FloatMatrix *
	zero3DFloatMatrix(unsigned int dim1, unsigned int dim2, unsigned int dim3) {
	/** Allocate needed memory for FloatMatrix struct */
	FloatMatrix *res = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	if (res == NULL) {
		printf(
			"%sError. Not enough memory for zero3DFloatMatrix res.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	/** Initialize matrix's dimensions */
	res->dimsNum = 3;
	res->dims = (unsigned int *) malloc(3 * sizeof(unsigned int));
	if (res->dims == NULL) {
		printf(
			"%sError. Not enough memory for zero3DFloatMatrix res->dims.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

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
FloatMatrix *create1DFloatMatrix(unsigned int dim) {
	/** Allocate needed memory for FloatMatrix struct */
	FloatMatrix *res = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	if (res == NULL) {
		printf(
			"%sError. Not enough memory for create1DFloatMatrix res.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	/** Initialize matrix's dimensions */
	res->dimsNum = 1;
	res->dims = (unsigned int *) malloc(1 * sizeof(unsigned int));
	if (res->dims == NULL) {
		printf(
			"%sError. Not enough memory for create1DFloatMatrix res->dims.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	res->dims[0] = dim;

	/** Allocate matrix's buffer */
	res->matrix = (matrix_t *) malloc(dim * sizeof(matrix_t));
	if (res->matrix == NULL) {
		printf(
			"%sError. Not enough memory for create1DFloatMatrix "
			"res->matrix.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	return res;
}

/**
 * Prints out a multidimensional matrix into a 1D representation
 * @param[in] x: the matrix to be printed
 */
void printMatrix(FloatMatrix *x) {
	unsigned int xLen = flattenDimensions(x);
	for (unsigned int i = 0; i < xLen; i++) {
		printf("index: %u | value: %lf\n", i, x->matrix[i]);
	}
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
	for (unsigned int i = 0; i < flattenDimensions(x); i++) {
		sum += x->matrix[i] < 0 ? -x->matrix[i] : x->matrix[i];
		min = min < x->matrix[i] ? min : x->matrix[i];
		max = max > x->matrix[i] ? max : x->matrix[i];
	}
	printf("Sum: %.16lf, Min: %.16lf, Max: %.16lf\n", sum, min, max);
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
	if (x == NULL) {
		printf(
			"%sError. Not enough memory for ImageToFloatMatrix x.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	/** Initialize FloatMatrix's dimensions */
	x->dimsNum = 3;
	x->dims = (unsigned int *) malloc(3 * sizeof(unsigned int));
	if (x->dims == NULL) {
		printf(
			"%sError. Not enough memory for ImageToFloatMatrix x->dims.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	x->dims[0] = image->depth;
	x->dims[1] = image->height;
	x->dims[2] = image->width;

	/** Allocate matrix's buffer memory to store the given image */
	x->matrix = (matrix_t *) malloc(
		image->depth * image->height * image->width * sizeof(matrix_t));
	if (x->matrix == NULL) {
		printf(
			"%sError. Not enough memory for ImageToFloatMatrix x->matrix.%s\n",
			KRED,
			KNRM);
		exit(XST_FAILURE);
	}

	/** Initialize buffer's index */
	unsigned int index = 0;

	/** For every color channel */
	for (unsigned int i = 0; i < image->depth; i++) {
		/** For every row */
		for (unsigned int j = 0; j < image->height; j++) {
			/** For every pixel in a row */
			for (unsigned int k = 0; k < image->width; k++) {
				/**
				 * Convert pixel with value range [0, 255] to [0, 1] range and
				 * copy to new FloatMatrix's matrix buffer
				 */
				x->matrix[index] = image->channels[i][j][k] / 256.0;

				/** Increase matrix's buffer index */
				index++;
			}
		}
	}
	return x;
}

#endif
