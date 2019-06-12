FloatMatrix *Conv2d(FloatMatrix *x, FloatMatrix weights, FloatMatrix bias, uint in_channels, uint out_channels, uint kernel_size, uint stride, uint padding) {
//	syslog(LOG_INFO, "Conv2D: Setting up.");

	uint Hout = (x->dims[1] + 2 * padding - kernel_size) / stride + 1;
	uint Wout = (x->dims[2] + 2 * padding - kernel_size) / stride + 1;
//	syslog(LOG_INFO, "Conv2D: Done setting Hout, Wout.");

	float *arr = zero3Dto1DMatrix(x->dims[0], x->dims[1] + 2 * padding, x->dims[2] + 2 * padding);
	uint *arrDims = (uint *) malloc(3 * sizeof(uint));
	arrDims[0] = x->dims[0];
	arrDims[1] = x->dims[1] + 2 * padding;
	arrDims[2] = x->dims[2] + 2 * padding;
//	syslog(LOG_INFO, "Conv2D: Done zeroing arr.");

#pragma omp parallel for
	for (int i = 0; i < x->dims[0]; i++) {
		for (int j = 0; j < x->dims[1]; j++) {
			for (int k = 0; k < x->dims[2]; k++) {
				uint index = calc3DIndex(x->dims, i, j, k);
				uint arrIndex = calc3DIndex(arrDims, i, j + padding, k + padding);
				arr[arrIndex] = x->matrix[index];
			}
		}
	}
//	syslog(LOG_INFO, "Conv2D: Done copying image data to arr.");

	float *res = zero3Dto1DMatrix(out_channels, Hout, Wout);
	uint *resDims = (uint *) malloc(3 * sizeof(uint));
	resDims[0] = out_channels;
	resDims[1] = Hout;
	resDims[2] = Wout;
//	syslog(LOG_INFO, "Conv2D: Done zeroing res.");

#pragma omp parallel for
	for (int out_channel = 0; out_channel < out_channels; out_channel++) {
		for (int oh = 0; oh < Hout; oh++) {
			for (int ow = 0; ow < Wout; ow++) {
				uint imgStartH = oh * stride;
				uint imgEndH = imgStartH + kernel_size;
				uint imgStartW = ow * stride;
				uint imgEndW = imgStartW + kernel_size;

				float pixel = 0;
				for (int in_channel = 0; in_channel < in_channels; in_channel++) {

					for (int i = imgStartH; i < imgEndH; i++) {
						for (int j = imgStartW; j < imgEndW; j++) {
							uint weightsIndex = calc4DIndex(weights.dims, out_channel, in_channel, i, j);
							uint arrIndex = calc3DIndex(arrDims, in_channel, i, j);
							pixel += arr[arrIndex] * weights.matrix[weightsIndex];
						}
					}
				}
				// Todo: check if averaging is needed
				uint resIndex = calc3DIndex(resDims, out_channel, oh, ow);
				res[resIndex] = pixel + bias.matrix[out_channel];
			}
		}
	}
//	syslog(LOG_INFO, "Conv2D: Done convoluting.");

	FloatMatrix *final = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	final->dimsNum = 3;
	final->dims = (uint *) malloc(3 * sizeof(uint));
	final->dims[0] = out_channels;
	final->dims[1] = Hout;
	final->dims[2] = Wout;
	final->matrix = (float *) res;

	free(x->matrix);
	free(x);
	free(arr);

	syslog(LOG_INFO, "Conv2D: Done with %u x %u x %u.", final->dims[0], final->dims[1], final->dims[2]);
	return final;
}


FloatMatrix *ReLU(FloatMatrix *x) {
	uint xLen = flattenDimensions(x);
	for (int i = 0; i < xLen; i++) {
		x->matrix[i] = x->matrix[i] < 0 ? 0 : x->matrix[i];
	}

	syslog(LOG_INFO, "ReLU: Done.");
	return x;
}


FloatMatrix *MaxPool2d(FloatMatrix *x, uint kernel_size, uint stride) {
	uint Hout = (x->dims[1] - kernel_size) / stride + 1;
	uint Wout = (x->dims[2] - kernel_size) / stride + 1;

	float *res = zero3Dto1DMatrix(x->dims[0], Hout, Wout);
	uint *resDims = (float *) malloc(3 * sizeof(float));
	resDims[0] = x->dims[0];
	resDims[1] = Hout;
	resDims[2] = Wout;

#pragma omp parallel for
	for (int k = 0; k < x->dims[0]; k++) {
		for (int i = 0; i < Hout; i++) {
			for (int j = 0; j < Wout; j++) {
				uint a = i * stride;
				uint b = j * stride;
				float max = -100000000;
				for (int l = a; l < a + kernel_size; l++) {
					for (int m = b; m < b + kernel_size; m++) {
						uint index = calc3DIndex(x->dims, k, l, m);
						if (max < x->matrix[index]) max = x->matrix[index];
					}
				}

				uint resIndex = calc3DIndex(resDims, k, i, j);
				res[resIndex] = max;
			}
		}
	}

	FloatMatrix *final = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	final->dimsNum = 3;
	final->dims = (uint *) malloc(3 * sizeof(uint));
	final->dims[0] = x->dims[0];
	final->dims[1] = Hout;
	final->dims[2] = Wout;
	final->matrix = (float *) res;

	free(x->matrix);
	free(x);

	syslog(LOG_INFO, "MaxPool2D: Done with %u x %u x %u.", final->dims[0], final->dims[1], final->dims[2]);
	return final;
}


FloatMatrix *Linear(FloatMatrix *x, FloatMatrix weights, FloatMatrix bias, uint in_features, uint out_features) {
//	syslog(LOG_INFO, "Linear: x: %u x %u x %u, matrix: %u x %u.", x->dims[0], x->dims[1], x->dims[2], matrix.dims[0], matrix.dims[1]);

	float *res = (float *) malloc(out_features * sizeof(float));

#pragma omp parallel for
	for (int i = 0; i < weights.dims[0]; i++) {
		float node = 0;

		for (int j = 0; j < weights.dims[1]; j++) {
			node += x->matrix[j] * weights.matrix[j + i * weights.dims[1]];
		}

		res[i] = node + bias.matrix[i];
	}

	FloatMatrix *final = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	final->dimsNum = 1;
	final->dims = (uint *) malloc(1 * sizeof(uint));
	final->dims[0] = out_features;
	final->matrix = res;

	free(x->matrix);
	free(x);

	syslog(LOG_INFO, "Linear: Done with %u nodes.", final->dims[0]);
	return final;
}


FloatMatrix *LogSoftMax(FloatMatrix *x, uint dim) {
	uint xLen = flattenDimensions(x);

	float sum = 0;
	for (int i = 0; i < xLen; i++) {
		sum += exp(x->matrix[i]);
	}

	float *res = (float *) malloc(xLen * sizeof(float));
	for (int i = 0; i < xLen; i++) {
		res[i] = log(exp(x->matrix[i]) / sum);
//		syslog(LOG_INFO, "Class %u: %f\n", i, res[i]);
	}

	FloatMatrix *final = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	final->dimsNum = 1;
	final->dims = (uint *) malloc(1 * sizeof(uint));
	final->dims[0] = xLen;
	final->matrix = res;

	free(x->matrix);
	free(x);

	syslog(LOG_INFO, "LogSoftMax: Done.");
	return final;
}


FloatMatrix *ImageToFloatMatrix(Image *image) {
	FloatMatrix *x = (FloatMatrix *) malloc(sizeof(FloatMatrix));
	x->dimsNum = 3;
	x->dims = (uint *) malloc(3 * sizeof(uint));
	x->dims[0] = image->depth;
	x->dims[1] = image->height;
	x->dims[2] = image->width;
	x->matrix = (float *) malloc(image->depth * image->height * image->width * sizeof(float));
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