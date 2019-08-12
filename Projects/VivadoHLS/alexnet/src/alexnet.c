#include "alexnet.h"

/**
 * Calculates the index on a 1D buffer representation of a 3D matrix
 * @param[in] dims: the sizes of the matrix's dimensions
 * @param[in] i: The plain dimension
 * @param[in] j: The row dimension
 * @param[in] k: The column dimension
 * @returns the 1D representation's index of a 3D matrix
 */
int _calc3DIndex(const int *dims, int i, int j, int k) {
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
int _calc4DIndex(const int *dims, int i, int j, int k, int l) {
	return l + k * dims[3] + j * dims[2] * dims[3] +
		   i * dims[1] * dims[2] * dims[3];
}

void _Conv2dReLU(din_t *x, dout_t *o, din_t *weights, din_t *bias,
				 const int in_channels, const int out_channels,
				 const int kernel_size, const int stride, const int padding,
				 const int inDims[3], const int outDims[3]) {
	int weightsDims[] = {out_channels, in_channels, kernel_size, kernel_size};
	/** For every output channel */
	for (int out_channel = 0; out_channel < out_channels; out_channel++) {
		/** For every output row */
		for (int oh = 0; oh < outDims[1]; oh++) {
			int imgStartH = oh * stride - padding;

			int iStart = imgStartH < 0 ? padding : 0;
			int iEnd = imgStartH + kernel_size >= inDims[1]
						   ? kernel_size - (imgStartH + kernel_size - inDims[1])
						   : kernel_size;

			/** For every output row's pixel */
			for (int ow = 0; ow < outDims[2]; ow++) {
				/** Calculate starting coordinates on the padded matrix */
				int imgStartW = ow * stride - padding;

				int jStart = imgStartW < 0 ? padding : 0;
				int jEnd =
					imgStartW + kernel_size >= inDims[2]
						? kernel_size - (imgStartW + kernel_size - inDims[2])
						: kernel_size;

				/** Initialize output pixel */
				din_t pixel = bias[out_channel];
				/** For every input channel */
				for (int in_channel = 0; in_channel < in_channels;
					 in_channel++) {
					/** For kernel_size rows on padded matrix */
					for (int i = iStart; i < iEnd; i++) {
						/** For kernel_size pixels on each padded matrix's row
						 */
						for (int j = jStart; j < jEnd; j++) {
							int imgH = i + imgStartH;
							int imgW = j + imgStartW;

							/**
							 * Calculate the 1-dimensional representation's
							 * index of the kernel's matrix
							 */
							int weightsIndex = _calc4DIndex(
								weightsDims, out_channel, in_channel, i, j);

							/**
							 * Calculate the 1-dimensional representation's
							 * index of the padded matrix
							 */
							int arrIndex =
								_calc3DIndex(inDims, in_channel, imgH, imgW);

							/** Calculate dot product of the two matrices */
							pixel += x[arrIndex] * weights[weightsIndex];
						}
					}
				}
				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				int resIndex = _calc3DIndex(outDims, out_channel, oh, ow);

				/**
				 * Assign biased dot product result on the corresponding
				 * output pixel
				 */
				o[resIndex] = pixel > 0 ? pixel : 0;
			}
		}
	}
}

void _MaxPool(din_t *x, dout_t *o, int kernel_size, int stride,
			  const int *inDims, const int *outDims) {
	/** For every channel */
	for (int i = 0; i < inDims[0]; i++) {
		/** For every output row */
		for (int j = 0; j < outDims[1]; j++) {
			/** For every output row's pixel */
			for (int k = 0; k < outDims[2]; k++) {
				/** Calculate pools starting coordinates on the input matrix */
				int a = j * stride;
				int b = k * stride;

				/** Initialize max value */
				din_t max = -100000000;
				/** For every pool's row */
				for (int l = 0; l < kernel_size; l++) {
					/** For every pool row's pixel */
					for (int m = 0; m < kernel_size; m++) {
						/**
						 * Calculate the 1-dimensional representation's index of
						 * the input matrix
						 */
						int index = _calc3DIndex(inDims, i, l + a, m + b);
						/**
						 * Check if current value is greater than max and
						 * update it
						 */
						max = max < x[index] ? x[index] : max;
					}
				}

				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				int resIndex = _calc3DIndex(outDims, i, j, k);

				/** Assign found max value to the output matrix */
				o[resIndex] = max;
			}
		}
	}
}

void _LinearReLU(din_t *x, din_t *w, din_t *b, dout_t *o, int inSize,
				 int outSize) {
	int i, j;
OUTPUT_LOOP:
	for (i = 0; i < outSize; i++) {
		int acc = b[i];
	INPUT_LOOP:
		for (j = 0; j < inSize; j++) {
			acc += x[j] * w[i * inSize + j];
		}
		if (acc <= 0)
			o[i] = 0;
		else
			o[i] = acc;
	}
}

void _Linear(din_t *x, din_t *w, din_t *b, dout_t *o, int inSize, int outSize) {
	int i, j;
OUTPUT_LOOP:
	for (i = 0; i < outSize; i++) {
		int acc = b[i];
	INPUT_LOOP:
		for (j = 0; j < inSize; j++) {
			acc += x[j] * w[i * inSize + j];
		}
		o[i] = acc;
	}
}

void Network(din_t *x, dout_t *o) {
	// din_t x[3][224][224];
	// _Conv2dReLU(x, o, w, b, C1_IN[0], C1_OUT[0], C1_KERNEL, C1_KERNEL, C1_PADDING, C1_IN, C1_OUT);
	// _MaxPool(x, o, M1_KERNEL, M1_STRIDE, M1_IN, M1_OUT);
	// _MaxPool(x, o, M2_KERNEL, M2_STRIDE, M2_IN, M2_OUT);
	// _MaxPool(x, o, M3_KERNEL, M3_STRIDE, M3_IN, M3_OUT);
	// _LinearReLU(x, w, b, o, L1_IN, L1_OUT);
	// _LinearReLU(x, w, b, o, L2_IN, L2_OUT);
	// _Linear(x, w, b, o, L3_IN, L3_OUT);
}
