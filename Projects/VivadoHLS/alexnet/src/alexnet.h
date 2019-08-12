#ifndef _ALEXNET_H_
#define _ALEXNET_H_

typedef int din_t;
typedef int dout_t;

/**
 * Calculates the 2-dimensional convolution of din_t x and filters stored
 * in din_t weights, biased by din_t bias.
 * @param[in] x: The input din_t to convolute
 * @param[in] weights: The kernels to convolute with
 * @param[in] bias: The bias of the convolution
 * @param[in] in_channels: The number of x's channels
 * @param[in] out_channels: The number of output channels
 * @param[in] kernel_size: The kernels' dimensions
 * @param[in] stride: The amount of pixels to skip for each convolution
 * @param[in] padding: The circular padding applied to the input before
 * convoluting
 * @param[in] layerName: This layer's name
 * @returns a din_t pointer containing the convolution's result
 */
void _Conv2dReLU(din_t *x, dout_t *o, din_t *weights, din_t *bias,
				 const int in_channels, const int out_channels,
				 const int kernel_size, const int stride, const int padding,
				 const int inDims[3], const int outDims[3]);

/**
 * Applies 2-dimensional max pooling on the input din_t.
 * Creates pools of pixels, finds the maximum value for each pool and assigns
 * that value to the output din_t.
 * @param[in] x: The input din_t to apply max pooling
 * @param[in] kernel_size: The kernel's dimensions
 * @param[in] stride: The amount of pixels to skip for every pool
 * @param[in] layerName: This layer's name
 * @returns a din_t pointer containing the max pooling's result
 */
void _MaxPool(din_t *x, dout_t *o, int kernel_size, int stride,
			  const int *inDims, const int *outDims);
/** */
void _LinearReLU(din_t *x, din_t *w, din_t *b, dout_t *o, int inSize,
				 int outSize);
/** */
void _Linear(din_t *x, din_t *w, din_t *b, dout_t *o, int inSize, int outSize);
/**
 * AlexNet implementation
 */
void Network(din_t *x, dout_t *o);

#endif
