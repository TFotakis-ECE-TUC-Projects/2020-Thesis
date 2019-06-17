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
                    uint in_channels, uint out_channels, uint kernel_size,
                    uint stride, uint padding, char *layerName) {
	/**
	 * Allocate and initialize a 3D zeros matrix to store the circularly
	 * padded input
	 */
	FloatMatrix *tmp = zero3DFloatMatrix(x->dims[0], x->dims[1] + 2 * padding,
	                                     x->dims[2] + 2 * padding);

	/** If OpenMP is defined parallelize the for loop */
#if ENABLE_OPENMP == 1
#pragma omp parallel for
#endif
	/** For every channel */
	for (int i = 0; i < x->dims[0]; i++) {
		/** For every row */
		for (int j = 0; j < x->dims[1]; j++) {
			/** For every row's pixel */
			for (int k = 0; k < x->dims[2]; k++) {
				/**
				 * Calculate the 1-dimensional representation's index of
				 * the input matrix
				 */
				uint index = calc3DIndex(x->dims, i, j, k);

				/**
				 * Calculate the 1-dimensional representation's index of
				 * the padded matrix
				 */
				uint arrIndex = calc3DIndex(tmp->dims, i, j + padding,
				                            k + padding);

				/** Copy input's pixel to padded matrix */
				tmp->matrix[arrIndex] = x->matrix[index];
			}
		}
	}

	/** Allocate and initialize a 3D zeros matrix to store the result */
	FloatMatrix *res = zero3DFloatMatrix(
			out_channels,
			(x->dims[1] + 2 * padding - kernel_size) / stride + 1,
			(x->dims[2] + 2 * padding - kernel_size) / stride + 1);

	/** Free the input FloatMatrix */
	freeFloatMatrix(x);

	/** If OpenMP is defined parallelize the for loop */
#if ENABLE_OPENMP == 1
#pragma omp parallel for
#endif
	/** For every output channel */
	for (int out_channel = 0; out_channel < out_channels; out_channel++) {
		/** For every output row */
		for (int oh = 0; oh < res->dims[1]; oh++) {
			/** For every output row's pixel */
			for (int ow = 0; ow < res->dims[2]; ow++) {
				/** Calculate starting coordinates on the padded matrix */
				uint imgStartH = oh * stride;
				uint imgStartW = ow * stride;

				/** Initialize output pixel */
				matrix_t pixel = 0;
				/** For every input channel */
				for (int in_channel = 0; in_channel < in_channels; in_channel++) {
					/** For kernel_size rows on padded matrix */
					for (int i = 0; i < kernel_size; i++) {
						/** For kernel_size pixels on each padded matrix's row */
						for (int j = 0; j < kernel_size; j++) {
							/**
							 * Calculate the 1-dimensional representation's
							 * index of the kernel's matrix
							 */
							uint weightsIndex = calc4DIndex(
									weights->dims,
									out_channel, in_channel, i, j);

							/**
							 * Calculate the 1-dimensional representation's
							 * index of the padded matrix
							 */
							uint arrIndex = calc3DIndex(
									tmp->dims, in_channel,
									i + imgStartH, j + imgStartW);

							/** Calculate dot product of the two matrices */
							pixel += tmp->matrix[arrIndex] * weights->matrix[weightsIndex];
						}
					}
				}
				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				uint resIndex = calc3DIndex(res->dims, out_channel, oh, ow);

				// Todo: Check if averaging is needed
				/**
				 * Assign biased dot product result on the corresponding
				 * output pixel
				 */
				res->matrix[resIndex] = pixel + bias->matrix[out_channel];
			}
		}
	}

	/** Free the temporary FloatMatrix */
	freeFloatMatrix(tmp);

	syslog(LOG_INFO, "Conv2D: Done with %u x %u x %u.", res->dims[0],
	       res->dims[1], res->dims[2]);

#ifdef ENABLE_CHECKING
	/** Print min max and sum of the whole matrix for debugging */
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
	/** Get the matrix's 1-dimensional representation size */
	uint xLen = flattenDimensions(x);
	/** For every value on the matrix */
	for (int i = 0; i < xLen; i++) {
		/** If value is negative, assign zero */
		x->matrix[i] = x->matrix[i] < 0 ? 0 : x->matrix[i];
	}
	syslog(LOG_INFO, "ReLU: Done.");


#ifdef ENABLE_CHECKING
	/** Print min max and sum of the whole matrix for debugging */
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
	/** Allocate and initialize a 3D zeros matrix to store the result */
	FloatMatrix *res = zero3DFloatMatrix(
			x->dims[0],
			(x->dims[1] - kernel_size) / stride + 1,
			(x->dims[2] - kernel_size) / stride + 1);

	/** If OpenMP is defined parallelize the for loop */
#if ENABLE_OPENMP == 1
#pragma omp parallel for
#endif
	/** For every channel */
	for (int i = 0; i < x->dims[0]; i++) {
		/** For every output row */
		for (int j = 0; j < res->dims[1]; j++) {
			/** For every output row's pixel */
			for (int k = 0; k < res->dims[2]; k++) {
				/** Calculate pools starting coordinates on the input matrix */
				uint a = j * stride;
				uint b = k * stride;

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
						uint index = calc3DIndex(x->dims, i, l, m);
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
				uint resIndex = calc3DIndex(res->dims, i, j, k);

				/** Assign found max value to the output matrix */
				res->matrix[resIndex] = max;
			}
		}
	}

	/** Free the input FloatMatrix */
	freeFloatMatrix(x);

	syslog(LOG_INFO, "MaxPool2D: Done with %u x %u x %u.", res->dims[0],
	       res->dims[1], res->dims[2]);

#ifdef ENABLE_CHECKING
	/** Print min max and sum of the whole matrix for debugging */
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
	/** Allocate and create a 1D matrix to store the result */
	FloatMatrix *res = create1DFloatMatrix(out_features);

	/** If OpenMP is defined parallelize the for loop */
#if ENABLE_OPENMP == 1
#pragma omp parallel for
#endif
	/** For every output feature */
	for (int i = 0; i < weights->dims[0]; i++) {
		/** Initialize output feature with its bias */
		res->matrix[i] = bias->matrix[i];

		/** For every input feature */
		for (int j = 0; j < weights->dims[1]; j++) {
			/** Add to the output feature the input feature's weighted value */
			res->matrix[i] += x->matrix[j] * weights->matrix[j + i * weights->dims[1]];
		}
	}

	/** Free the input FloatMatrix */
	freeFloatMatrix(x);

	syslog(LOG_INFO, "Linear: Done with %u nodes.", res->dims[0]);

#ifdef ENABLE_CHECKING
	/** Print min max and sum of the whole matrix for debugging */
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
	/** Allocate and create a 1D matrix to store the result */
	FloatMatrix *res = create1DFloatMatrix(flattenDimensions(x));

	/** Initialize sum */
	matrix_t sum = 0;

	/** Sum every value on the matrix */
	for (int i = 0; i < res->dims[0]; i++) {
		sum += exp(x->matrix[i]);
	}

	/** Calculate the log ( softmax(x) ), where x is every value on the matrix */
	for (int i = 0; i < res->dims[0]; i++) {
		res->matrix[i] = log(exp(x->matrix[i]) / sum);
	}

	/** Free the input FloatMatrix */
	freeFloatMatrix(x);

	syslog(LOG_INFO, "LogSoftMax: Done.");

#ifdef ENABLE_CHECKING
	/** Print min max and sum of the whole matrix for debugging */
	printf("%s | ", layerName);
	printMinMaxSum(res);
#endif

	return res;
}