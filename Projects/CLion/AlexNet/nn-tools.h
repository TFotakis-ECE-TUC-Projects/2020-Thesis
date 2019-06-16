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
 * @param[in] padding: The circular padding applied to the input before convoluting
 * @param[in] layerName: This layer's name
 * @returns a FloatMatrix pointer containing the convolution's result
 */
FloatMatrix *Conv2d(FloatMatrix *x, FloatMatrix *weights, FloatMatrix *bias,
		uint in_channels, uint out_channels, uint kernel_size, uint stride,
		uint padding, char *layerName) {
	FloatMatrix *tmp = zero3DFloatMatrix(x->dims[0], x->dims[1] + 2 * padding, x->dims[2] + 2 * padding);
#ifdef _OMP_H
#pragma omp parallel for
#endif
	for (int i = 0; i < x->dims[0]; i++) {
		for (int j = 0; j < x->dims[1]; j++) {
			for (int k = 0; k < x->dims[2]; k++) {
				uint index = calc3DIndex(x->dims, i, j, k);
				uint arrIndex = calc3DIndex(tmp->dims, i, j + padding, k + padding);
				tmp->matrix[arrIndex] = x->matrix[index];
			}
		}
	}

	FloatMatrix *res = zero3DFloatMatrix(out_channels, (x->dims[1] + 2 * padding - kernel_size) / stride + 1, (x->dims[2] + 2 * padding - kernel_size) / stride + 1);
	freeFloatMatrix(x);
#ifdef _OMP_H
#pragma omp parallel for
#endif
	for (int out_channel = 0; out_channel < out_channels; out_channel++) {
		for (int oh = 0; oh < res->dims[1]; oh++) {
			for (int ow = 0; ow < res->dims[2]; ow++) {
				uint imgStartH = oh * stride;
				uint imgEndH = imgStartH + kernel_size;
				uint imgStartW = ow * stride;
				uint imgEndW = imgStartW + kernel_size;

				matrix_t pixel = 0;
				for (int in_channel = 0; in_channel < in_channels; in_channel++) {
					for (int i = 0; i < kernel_size; i++) {
						for (int j = 0; j < kernel_size; j++) {
							uint weightsIndex = calc4DIndex(weights->dims, out_channel, in_channel, i, j);
							uint arrIndex = calc3DIndex(tmp->dims, in_channel, i + imgStartH, j + imgStartW);
							pixel += tmp->matrix[arrIndex] * weights->matrix[weightsIndex];
						}
					}
				}
				// Todo: Check if averaging is needed
				uint resIndex = calc3DIndex(res->dims, out_channel, oh, ow);
				res->matrix[resIndex] = pixel + bias->matrix[out_channel];
			}
		}
	}

	freeFloatMatrix(tmp);

	syslog(LOG_INFO, "Conv2D: Done with %u x %u x %u.", res->dims[0], res->dims[1], res->dims[2]);

#ifndef SKIP_CHECKING
	printf("%s | ", layerName);
	printMinMaxSum(res);
#endif

	return res;
}


/**
 * Applies Rectified Linear Unit on the input FloatMatrix, in other words, it
 * zeroes out every negative value.
 * @param[in] x: The input FloatMatrix to activate.
 * @param[in] layerName: This layer's name
 * @returns a FloatMatrix pointer containing the activated values
 */
FloatMatrix *ReLU(FloatMatrix *x, char *layerName) {
	uint xLen = flattenDimensions(x);
	for (int i = 0; i < xLen; i++) {
		x->matrix[i] = x->matrix[i] < 0 ? 0 : x->matrix[i];
	}
	syslog(LOG_INFO, "ReLU: Done.");

#ifndef SKIP_CHECKING
	printf("%s | ", layerName);
	printMinMaxSum(x);
#endif

	return x;
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
FloatMatrix *MaxPool2d(FloatMatrix *x, uint kernel_size, uint stride,
		char *layerName) {
	FloatMatrix *res = zero3DFloatMatrix(x->dims[0], (x->dims[1] - kernel_size) / stride + 1, (x->dims[2] - kernel_size) / stride + 1);
#ifdef _OMP_H
#pragma omp parallel for
#endif
	for (int i = 0; i < x->dims[0]; i++) {
		for (int j = 0; j < res->dims[1]; j++) {
			for (int k = 0; k < res->dims[2]; k++) {
				uint a = j * stride;
				uint b = k * stride;
				matrix_t max = -100000000;
				for (int l = a; l < a + kernel_size; l++) {
					for (int m = b; m < b + kernel_size; m++) {
						uint index = calc3DIndex(x->dims, i, l, m);
						max = max < x->matrix[index] ? x->matrix[index] : max;
					}
				}

				uint resIndex = calc3DIndex(res->dims, i, j, k);
				res->matrix[resIndex] = max;
			}
		}
	}

	freeFloatMatrix(x);

	syslog(LOG_INFO, "MaxPool2D: Done with %u x %u x %u.", res->dims[0], res->dims[1], res->dims[2]);

#ifndef SKIP_CHECKING
	printf("%s | ", layerName);
	printMinMaxSum(res);
#endif

	return res;
}


/**
 * Applies a linear transformation to the input FloatMatrix
 * @param[in] x: The input FloatMatrix
 * @param[in] weights: The weights to use for the linear transformation
 * @param[in] bias: The bias to add to the linear transformation
 * @param[in] in_features: The number of input features (dimension of x)
 * @param[in] out_features: The number of output features (dimension of the result)
 * @param[in] layerName: This layer's name
 * @returns a FloatMatrix pointer containing the linear transformation's result
 */
FloatMatrix *Linear(FloatMatrix *x, FloatMatrix *weights, FloatMatrix *bias,
		uint in_features, uint out_features, char *layerName) {
	FloatMatrix *res = create1DFloatMatrix(out_features);
#ifdef _OMP_H
#pragma omp parallel for
#endif
	for (int i = 0; i < weights->dims[0]; i++) {
		res->matrix[i] = bias->matrix[i];
		for (int j = 0; j < weights->dims[1]; j++) res->matrix[i] += x->matrix[j] * weights->matrix[j + i * weights->dims[1]];
	}

	freeFloatMatrix(x);

	syslog(LOG_INFO, "Linear: Done with %u nodes.", res->dims[0]);

#ifndef SKIP_CHECKING
	printf("%s | ", layerName);
	printMinMaxSum(res);
#endif

	return res;
}


/**
 * Applies the log(SoftMax(x)) function to the input FloatMatrix.
 * @param[in] x: The input FloatMatrix
 * @param[in] layerName: This layer's name
 * @returns a FloatMatrix pointer containing the LogSoftMax's result
 */
FloatMatrix *LogSoftMax(FloatMatrix *x, char *layerName) {
	FloatMatrix *res = create1DFloatMatrix(flattenDimensions(x));
	matrix_t sum = 0;
	for (int i = 0; i < res->dims[0]; i++) {
		sum += exp(x->matrix[i]);
	}
	for (int i = 0; i < res->dims[0]; i++) {
		res->matrix[i] = log(exp(x->matrix[i]) / sum);
	}

	freeFloatMatrix(x);

	syslog(LOG_INFO, "LogSoftMax: Done.");

#ifndef SKIP_CHECKING
	printf("%s | ", layerName);
	printMinMaxSum(res);
#endif

	return res;
}