#define MAX_X_SIZE 193600

/** Matrix data type for easy switching when needed to other types */
typedef float matrix_t;

/**
 * Calculates the index on a 1D buffer representation of a 3D matrix
 * @param[in] dims: the sizes of the matrix's dimensions
 * @param[in] i: The plain dimension
 * @param[in] j: The row dimension
 * @param[in] k: The column dimension
 * @returns the 1D representation's index of a 3D matrix
 */
unsigned int calc3DIndex(unsigned int dim0, unsigned int dim1,
		unsigned int dim2, unsigned int i, unsigned int j, unsigned int k) {
	return k + j * dim2 + i * dim1 * dim2;
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
unsigned int calc4DIndex(unsigned int dim0, unsigned int dim1,
		unsigned int dim2, unsigned int dim3, unsigned int i, unsigned int j,
		unsigned int k, unsigned int l) {
	return l + k * dim3 + j * dim2 * dim3 + i * dim1 * dim2 * dim3;
}

int Conv_Core(volatile matrix_t *x, volatile matrix_t *weights,
		volatile matrix_t *bias, unsigned int din, unsigned int hin,
		unsigned int win, unsigned int dout, unsigned int hout,
		unsigned int wout, unsigned int kernel_size, unsigned int stride,
		unsigned int padding, volatile matrix_t *res) {
#pragma HLS INTERFACE m_axi depth=1024 port=x offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE m_axi depth=1024 port=weights offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE m_axi depth=1024 port=bias offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE s_axilite port=din bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=hin bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=win bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=dout bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=hout bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=wout bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=kernel_size bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=stride bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=padding bundle=CRTL_BUS
#pragma HLS INTERFACE m_axi depth=1024 port=res offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
//	unsigned int size = dout * hout * wout;
//	if (size > 100000000)
//		return -1;
//	for (unsigned int i = 0; i < size; i++) {
//		res[i] = 2;
//	}
//	return 0;

//	din = 3;
//	hin = 224;
//	win = 224;
//	dout = 64;
//	hout = 55;
//	wout = 55;
//	kernel_size = 11;
//	stride = 4;
//	padding = 2;

	matrix_t xCache[MAX_X_SIZE];
	for (unsigned int i = 0; i < din * hin * win; i++) {
#pragma HLS PIPELINE II=1
		xCache[i] = x[i];
	}

	Loop_output_channel:
	/** For every output channel */
	for (unsigned int out_channel = 0; out_channel < dout; out_channel++) {

		// Todo: Fix hardcoded dims
		matrix_t pixels[55][55];

		matrix_t biasCache = bias[out_channel];
		Loop_pixels_row_init:
		for (unsigned int i = 0; i < hout; i++) {
		Loop_pixels_pixel_init:
			for (unsigned int j = 0; j < wout; j++) {
				pixels[i][j] = biasCache;
			}
		}

		Loop_kernel:
		/** For every input channel */
		for (unsigned int in_channel = 0; in_channel < din; in_channel++) {

			// Todo: Fix hardcoded dims
			matrix_t weightsCache[11][11];

		Loop_weightsCache_row_init:
			for (unsigned int i = 0; i < kernel_size; i++) {
			Loop_weightsCache_pixel_init:
				for (unsigned int j = 0; j < kernel_size; j++) {
					/* Calculate the 1-dimensional representation's
					 * index of the kernel's matrix
					 */
					unsigned int weightsIndex = calc4DIndex(dout, din,
							kernel_size, kernel_size, out_channel, in_channel,
							i, j);
					weightsCache[i][j] = weights[weightsIndex];
				}
			}

			Loop_output_row:
			/** For every output row */
			for (unsigned int oh = 0; oh < hout; oh++) {
				int imgStartH = oh * stride - padding;

				Loop_output_pixel:
				/** For every output row's pixel */
				for (unsigned int ow = 0; ow < wout; ow++) {
					/** Calculate starting coordinates on the padded matrix */
					int imgStartW = ow * stride - padding;

					Loop_kernel_row:
					/** For kernel_size rows on padded matrix */
					for (unsigned int i = 0; i < kernel_size; i++) {

						/** For kernel_size pixels on each padded matrix's row */
						Loop_kernel_pixel: for (unsigned int j = 0;
								j < kernel_size; j++) {
							int imgH = i + imgStartH;
							int imgW = j + imgStartW;

							if (imgH < 0 || imgH >= hin || imgW < 0 || imgW >= win)
								continue;
							/**
							 * Calculate the 1-dimensional representation's
							 * index of the padded matrix
							 */
							unsigned int arrIndex = calc3DIndex(din, hin, win,
									in_channel, imgH, imgW);

							/** Calculate dot product of the two matrices */
//							pixels[oh][ow] += x[arrIndex] * weightsCache[i][j];
							pixels[oh][ow] += xCache[arrIndex] * weightsCache[i][j];
						}
					}
				}
			}
		}

		Loop_output_row_write:
		for (unsigned int oh = 0; oh < hout; oh++) {
		Loop_output_pixel_write:
			for (unsigned int ow = 0; ow < wout; ow++) {
				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				unsigned int resIndex =
					calc3DIndex(dout, hout, wout, out_channel, oh, ow);

				/**
				 * Assign biased dot product result on the corresponding
				 * output pixel
				 */
				res[resIndex] = pixels[oh][ow] > 0 ? pixels[oh][ow] : 0;
			}
		}
	}

	return 0;
}
