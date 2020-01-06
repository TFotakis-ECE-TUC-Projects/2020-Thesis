#include "AlexNetParams.h"

/** Matrix data type for easy switching when needed to other types */
typedef double matrix_t;

/**
 * Struct to store a multi-dimensional matrix of type matrix_t in a buffer-like
 * representation.
 */
typedef struct floatMatrix_t {
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

/** Structure to store the network's parameters */
typedef struct params_t {
	unsigned int len;			  /**< The number of parameter matrices */
	FloatMatrix **matrix; /**< The 1D array of FloatMartix cells to store the
							 parameters */
} Params;

unsigned int tzalloc(unsigned int size) {
	static unsigned int offset = 0x0;
	unsigned int p = offset;
	offset += size;
	return p;
}

/**
 * Calculates the total cells number of a multi-dimensional matrix
 * @param[in] x: The matrix to calculate its total cells number
 * @returns The total cells
 */
unsigned int flattenDimensions(FloatMatrix x) {
	/** Multiplies every dimension size to get the total cells */
	unsigned int xLen = 1;
	for (int i = 0; i < x.dimsNum; i++) xLen *= x.dims[i];
	return xLen;
}

/**
 * Finds the max value in a matrix and returns its index as if it was 1D
 * @param[in] x
 * @returns the 1-dimensional index of the highest valued cell
 */
unsigned int argmax(FloatMatrix x) {
	/** Get the total number of cells in matrix x */
	unsigned int xLen = flattenDimensions(x);

	/** Initialize max to the matrix's first cell */
	matrix_t max = x.matrix[0];

	/** Initialize the highest valued max index to be the first of the matrix */
	unsigned int index = 0;
	for (int i = 1; i < xLen; i++) {
		/** If max greater than the current value ignore and continue */
		if (max >= x.matrix[i]) continue;

		/** If current max is less than current value update it and its index */
		max = x.matrix[i];
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
unsigned int calc3DIndex(unsigned int *dims, unsigned int i, unsigned int j, unsigned int k) {
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
unsigned int calc4DIndex(unsigned int *dims, unsigned int i, unsigned int j, unsigned int k, unsigned int l) {
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
matrix_t *zero3Dto1DMatrix(unsigned int dim1, unsigned int dim2, unsigned int dim3) {
	/** Calculate the buffer's size */
	unsigned int arrLen = dim1 * dim2 * dim3;

	/** Allocate needed memory */
	matrix_t *arr = (matrix_t *)tzalloc(arrLen * sizeof(matrix_t));

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
FloatMatrix *zero3DFloatMatrix(unsigned int dim1, unsigned int dim2, unsigned int dim3) {
	/** Allocate needed memory for FloatMatrix struct */
	FloatMatrix *res = (FloatMatrix *)tzalloc(sizeof(FloatMatrix));

	/** Initialize matrix's dimensions */
	res->dimsNum = 3;
	res->dims = (unsigned int *)tzalloc(3 * sizeof(unsigned int));
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
	FloatMatrix *res = (FloatMatrix *)tzalloc(sizeof(FloatMatrix));

	/** Initialize matrix's dimensions */
	res->dimsNum = 1;
	res->dims = (unsigned int *)tzalloc(1 * sizeof(unsigned int));
	res->dims[0] = dim;

	/** Allocate matrix's buffer */
	res->matrix = (matrix_t *)tzalloc(dim * sizeof(matrix_t));
	return res;
}

/**
 * Calculates the 2-dimensional convolution of FloatMatrix x and filters stored
 * in FloatMatrix weights, biased by FloatMatrix bias.
 * @param[in] x: The input FloatMatrix to convolute
 * @param[in] weights: The kernels to convolute with
 * @param[in] bias: The bias of the convolution
 * @param[in] in_channels: The number of x's channels
 * @param[in] out_channels: The number of output channels
 * @param[in] kernel_size: The kernels' dimensions
 * @param[in] stride: The amount of pixels to skip for each convolution
 * @param[in] padding: The circular padding applied to the input before
 * convoluting
 * @param[in] layerName: This layer's name
 * @returns a FloatMatrix pointer containing the convolution's result
 */
FloatMatrix *Conv2dReLU(FloatMatrix *x, FloatMatrix *weights, FloatMatrix *bias,
						unsigned int in_channels, unsigned int out_channels, unsigned int kernel_size,
						unsigned int stride, unsigned int padding) {
	/** Allocate and initialize a 3D zeros matrix to store the result */
	FloatMatrix *res = zero3DFloatMatrix(
		out_channels, (x->dims[1] + 2 * padding - kernel_size) / stride + 1,
		(x->dims[2] + 2 * padding - kernel_size) / stride + 1);

	/** For every output channel */
	for (int out_channel = 0; out_channel < out_channels; out_channel++) {
		/** For every output row */
		for (int oh = 0; oh < res->dims[1]; oh++) {
			int imgStartH = oh * stride - padding;

			unsigned int iStart = imgStartH < 0 ? padding : 0;
			unsigned int iEnd =
				imgStartH + kernel_size >= x->dims[1]
					? kernel_size - (imgStartH + kernel_size - x->dims[1])
					: kernel_size;

			/** For every output row's pixel */
			for (int ow = 0; ow < res->dims[2]; ow++) {
				/** Calculate starting coordinates on the padded matrix */
				int imgStartW = ow * stride - padding;

				unsigned int jStart = imgStartW < 0 ? padding : 0;
				unsigned int jEnd =
					imgStartW + kernel_size >= x->dims[2]
						? kernel_size - (imgStartW + kernel_size - x->dims[2])
						: kernel_size;

				/** Initialize output pixel */
				matrix_t pixel = bias->matrix[out_channel];
				/** For every input channel */
				for (int in_channel = 0; in_channel < in_channels;
					 in_channel++) {
					/** For kernel_size rows on padded matrix */
					for (int i = iStart; i < iEnd; i++) {
						/** For kernel_size pixels on each padded matrix's row
						 */
						for (int j = jStart; j < jEnd; j++) {
							unsigned int imgH = i + imgStartH;
							unsigned int imgW = j + imgStartW;

							/**
							 * Calculate the 1-dimensional representation's
							 * index of the kernel's matrix
							 */
							unsigned int weightsIndex = calc4DIndex(
								weights->dims, out_channel, in_channel, i, j);

							/**
							 * Calculate the 1-dimensional representation's
							 * index of the padded matrix
							 */
							unsigned int arrIndex =
								calc3DIndex(x->dims, in_channel, imgH, imgW);

							/** Calculate dot product of the two matrices */
							pixel += x->matrix[arrIndex] *
									 weights->matrix[weightsIndex];
						}
					}
				}
				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				unsigned int resIndex = calc3DIndex(res->dims, out_channel, oh, ow);

				/**
				 * Assign biased dot product result on the corresponding
				 * output pixel
				 */
				res->matrix[resIndex] = pixel > 0 ? pixel : 0;
			}
		}
	}

	return res;
}

/**
 * Applies 2-dimensional max pooling on the input FloatMatrix.
 * Creates pools of pixels, finds the maximum value for each pool and assigns
 * that value to the output FloatMatrix.
 * @param[in] x: The input FloatMatrix to apply max pooling
 * @param[in] kernel_size: The kernel's dimensions
 * @param[in] stride: The amount of pixels to skip for every pool
 * @param[in] layerName: This layer's name
 * @returns a FloatMatrix pointer containing the max pooling's result
 */
FloatMatrix *MaxPool2d(FloatMatrix *x, unsigned int kernel_size, unsigned int stride) {
	/** Allocate and initialize a 3D zeros matrix to store the result */
	FloatMatrix *res =
		zero3DFloatMatrix(x->dims[0], (x->dims[1] - kernel_size) / stride + 1,
						  (x->dims[2] - kernel_size) / stride + 1);

	/** For every channel */
	for (int i = 0; i < x->dims[0]; i++) {
		/** For every output row */
		for (int j = 0; j < res->dims[1]; j++) {
			/** For every output row's pixel */
			for (int k = 0; k < res->dims[2]; k++) {
				/** Calculate pools starting coordinates on the input matrix */
				unsigned int a = j * stride;
				unsigned int b = k * stride;

				/** Initialize max value */
				matrix_t max = -100000000;
				/** For every pool's row */
				for (int l = a; l < a + kernel_size; l++) {
					/** For every pool row's pixel */
					for (int m = b; m < b + kernel_size; m++) {
						/**
						 * Calculate the 1-dimensional representation's index of
						 * the input matrix
						 */
						unsigned int index = calc3DIndex(x->dims, i, l, m);
						/**
						 * Check if current value is greater than max and
						 * update it
						 */
						max = max < x->matrix[index] ? x->matrix[index] : max;
					}
				}

				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				unsigned int resIndex = calc3DIndex(res->dims, i, j, k);

				/** Assign found max value to the output matrix */
				res->matrix[resIndex] = max;
			}
		}
	}

	return res;
}

/**
 * Applies a linear transformation to the input FloatMatrix
 * @param[in] x: The input FloatMatrix
 * @param[in] weights: The weights to use for the linear transformation
 * @param[in] bias: The bias to add to the linear transformation
 * @param[in] in_features: The number of input features (dimension of x)
 * @param[in] out_features: The number of output features (dimension of the
 * result)
 * @param[in] layerName: This layer's name
 * @returns a FloatMatrix pointer containing the linear transformation's result
 */
FloatMatrix *Linear(FloatMatrix *x, FloatMatrix *weights, FloatMatrix *bias,
					unsigned int in_features, unsigned int out_features) {
	/** Allocate and create a 1D matrix to store the result */
	FloatMatrix *res = create1DFloatMatrix(out_features);

	/** For every output feature */
	for (int i = 0; i < weights->dims[0]; i++) {
		/** Initialize output feature with its bias */
		res->matrix[i] = bias->matrix[i];

		/** For every input feature */
		for (int j = 0; j < weights->dims[1]; j++) {
			/** Add to the output feature the input feature's weighted value */
			res->matrix[i] +=
				x->matrix[j] * weights->matrix[j + i * weights->dims[1]];
		}
	}

	return res;
}

/**
 * Applies a linear transformation to the input FloatMatrix
 * @param[in] x: The input FloatMatrix
 * @param[in] weights: The weights to use for the linear transformation
 * @param[in] bias: The bias to add to the linear transformation
 * @param[in] in_features: The number of input features (dimension of x)
 * @param[in] out_features: The number of output features (dimension of the
 * result)
 * @param[in] layerName: This layer's name
 * @returns a FloatMatrix pointer containing the linear transformation's result
 */
FloatMatrix *LinearReLU(FloatMatrix *x, FloatMatrix *weights, FloatMatrix *bias,
						unsigned int in_features, unsigned int out_features) {
	/** Allocate and create a 1D matrix to store the result */
	FloatMatrix *res = create1DFloatMatrix(out_features);

	/** For every output feature */
	for (int i = 0; i < weights->dims[0]; i++) {
		/** Initialize output feature with its bias */
		res->matrix[i] = bias->matrix[i];

		/** For every input feature */
		for (int j = 0; j < weights->dims[1]; j++) {
			/** Add to the output feature the input feature's weighted value */
			res->matrix[i] +=
				x->matrix[j] * weights->matrix[j + i * weights->dims[1]];
		}

		res->matrix[i] = res->matrix[i] > 0 ? res->matrix[i] : 0;
	}

	return res;
}

/**
 * Runs the network's forward pass
 * @param[in] params: the network's parameters sets
 * @param[in] x: the input image to get classified
 * @returns a FloatMatrix containing the network's classification estimates,
 * that is an array of matrix_t numbers, one for each class, which shows how
 * likely it is for the input image to be of a class in a logarithmic scale.
 */
FloatMatrix *forward(FloatMatrix *x, Params *params) {
	FloatMatrix *x[LAYERS_NUMBER + 1];
	x[0] = x;
	for (int i = 0; i < LAYERS_NUMBER; i++){
		switch (LAYER_TYPE[i]) {
		case 'C':
			x[i + 1] = Conv2dReLU(x[i], params->matrix[0], params->matrix[1], 3, 64, 11, 4, 2);
			break;
		case 'M':
			x[i + 1] = MaxPool2d(x[i], 3, 2);
			break;
		case 'L':
			x[i + 1] = LinearReLU(x[i], params->matrix[10], params->matrix[11], 256 * 6 * 6, 4096);
			break;
		}
		case 'l':
			x[i + 1] = Linear(x[i], params->matrix[10], params->matrix[11], 256 * 6 * 6, 4096);
			break;
		}
	}
	return x[LAYERS_NUMBER];
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
int AlexNetCore(volatile matrix_t *xIn, volatile matrix_t *paramsIn, unsigned int MemBaseAddr) {
#pragma HLS INTERFACE m_axi depth=1024 port=paramsIn offset=slave bundle=params
#pragma HLS INTERFACE m_axi depth=1024 port=xIn offset=slave bundle=img
#pragma HLS INTERFACE s_axilite port=MemBaseAddr
#pragma HLS INTERFACE s_axilite port=return

//	tzalloc(MemBaseAddr);

	/** Pass the image through the network */
//	FloatMatrix *x1 = forward(x, params);
	FloatMatrix x1;

	/** Find the class with the greatest likelihood and print its label */
	unsigned int topClass = argmax(x1);

	return topClass;
}
