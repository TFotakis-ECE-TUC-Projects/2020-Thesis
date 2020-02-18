#ifndef SRC_NN_UTILS_H_
#define SRC_NN_UTILS_H_

#include "platform_conf.h"
#include "math.h"

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
 * @returns a FloatMatrix pointer containing the convolution's result
 */
FloatMatrix *Conv2dReLU(
	FloatMatrix *x,
	FloatMatrix *weights,
	FloatMatrix *bias,
	unsigned int in_channels,
	unsigned int out_channels,
	unsigned int kernel_size,
	unsigned int stride,
	unsigned int padding) {
	/** Allocate and initialize a 3D zeros matrix to store the result */
	FloatMatrix *res = zero3DFloatMatrix(
		out_channels,
		(x->dims[1] + 2 * padding - kernel_size) / stride + 1,
		(x->dims[2] + 2 * padding - kernel_size) / stride + 1);

	/** For every output channel */
	for (unsigned int out_channel = 0; out_channel < out_channels;
		 out_channel++) {
		/** For every output row */
		for (unsigned int oh = 0; oh < res->dims[1]; oh++) {
			int imgStartH = oh * stride - padding;

			unsigned int iStart = imgStartH < 0 ? padding : 0;
			unsigned int iEnd = imgStartH + kernel_size >= x->dims[1] ?
				kernel_size - (imgStartH + kernel_size - x->dims[1]) :
				kernel_size;

			/** For every output row's pixel */
			for (unsigned int ow = 0; ow < res->dims[2]; ow++) {
				/** Calculate starting coordinates on the padded matrix */
				int imgStartW = ow * stride - padding;

				unsigned int jStart = imgStartW < 0 ? padding : 0;
				unsigned int jEnd = imgStartW + kernel_size >= x->dims[2] ?
					kernel_size - (imgStartW + kernel_size - x->dims[2]) :
					kernel_size;

				/** Initialize output pixel */
				matrix_t pixel = bias->matrix[out_channel];
				/** For every input channel */
				for (unsigned int in_channel = 0; in_channel < in_channels;
					 in_channel++) {
					/** For kernel_size rows on padded matrix */
					for (unsigned int i = iStart; i < iEnd; i++) {
						/** For kernel_size pixels on each padded matrix's row
						 */
						for (unsigned int j = jStart; j < jEnd; j++) {
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
				unsigned int resIndex =
					calc3DIndex(res->dims, out_channel, oh, ow);

				/**
				 * Assign biased dot product result on the corresponding
				 * output pixel
				 */
				res->matrix[resIndex] = pixel > 0 ? pixel : 0;
			}
		}
	}

	/** Free the temporary FloatMatrix */
	freeFloatMatrix(x);
	return res;
}

/**
 * Applies 2-dimensional max pooling on the input FloatMatrix.
 * Creates pools of pixels, finds the maximum value for each pool and assigns
 * that value to the output FloatMatrix.
 * @param[in] x: The input FloatMatrix to apply max pooling
 * @param[in] kernel_size: The kernel's dimensions
 * @param[in] stride: The amount of pixels to skip for every pool
 * @returns a FloatMatrix pointer containing the max pooling's result
 */
FloatMatrix *
	MaxPool2d(FloatMatrix *x, unsigned int kernel_size, unsigned int stride) {
	/** Allocate and initialize a 3D zeros matrix to store the result */
	FloatMatrix *res = zero3DFloatMatrix(
		x->dims[0],
		(x->dims[1] - kernel_size) / stride + 1,
		(x->dims[2] - kernel_size) / stride + 1);

	/** For every channel */
	for (unsigned int i = 0; i < x->dims[0]; i++) {
		/** For every output row */
		for (unsigned int j = 0; j < res->dims[1]; j++) {
			/** For every output row's pixel */
			for (unsigned int k = 0; k < res->dims[2]; k++) {
				/** Calculate pools starting coordinates on the input matrix */
				unsigned int a = j * stride;
				unsigned int b = k * stride;

				/** Initialize max value */
				matrix_t max = -100000000;
				/** For every pool's row */
				for (unsigned int l = a; l < a + kernel_size; l++) {
					/** For every pool row's pixel */
					for (unsigned int m = b; m < b + kernel_size; m++) {
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

	/** Free the input FloatMatrix */
	freeFloatMatrix(x);
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
 * @param[in] useReLU: Assign 1 for applying ReLU on output, else assign 0
 * @returns a FloatMatrix pointer containing the linear transformation's result
 */
FloatMatrix *Linear(
	FloatMatrix *x,
	FloatMatrix *weights,
	FloatMatrix *bias,
	unsigned int in_features,
	unsigned int out_features,
	unsigned int useReLU) {
	/** Allocate and create a 1D matrix to store the result */
	FloatMatrix *res = create1DFloatMatrix(out_features);

	/** For every output feature */
	for (unsigned int i = 0; i < weights->dims[0]; i++) {
		/** Initialize output feature with its bias */
		matrix_t output = bias->matrix[i];

		/** For every input feature */
		for (unsigned int j = 0; j < weights->dims[1]; j++) {
			/** Add to the output feature the input feature's weighted value */
			output += x->matrix[j] * weights->matrix[j + i * weights->dims[1]];
		}
		res->matrix[i] = useReLU != 0 && output < 0 ? 0 : output;
	}

	/** Free the input FloatMatrix */
	freeFloatMatrix(x);
	return res;
}

#endif
