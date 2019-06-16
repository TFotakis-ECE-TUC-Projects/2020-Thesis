FloatMatrix *Conv2d(FloatMatrix *x, FloatMatrix *weights, FloatMatrix *bias, uint in_channels, uint out_channels, uint kernel_size, uint stride, uint padding, char *layerName) {
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


FloatMatrix *MaxPool2d(FloatMatrix *x, uint kernel_size, uint stride, char *layerName) {
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


FloatMatrix *Linear(FloatMatrix *x, FloatMatrix *weights, FloatMatrix *bias, uint in_features, uint out_features, char *layerName) {
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