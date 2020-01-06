/** Matrix data type for easy switching when needed to other types */
typedef double matrix_t;

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

int Conv_Core(matrix_t *x, matrix_t *weights, matrix_t *bias, unsigned int din,
		unsigned int hin, unsigned int win, unsigned int dout,
		unsigned int hout, unsigned int wout, unsigned int kernel_size,
		unsigned int stride, unsigned int padding, matrix_t *res) {
#pragma HLS INTERFACE m_axi depth=1024 port=x offset=slave bundle=MEM_R
#pragma HLS INTERFACE m_axi depth=1024 port=weights offset=slave bundle=MEM_R
#pragma HLS INTERFACE m_axi depth=1024 port=bias offset=slave bundle=MEM_R
#pragma HLS INTERFACE s_axilite port=din
#pragma HLS INTERFACE s_axilite port=hin
#pragma HLS INTERFACE s_axilite port=win
#pragma HLS INTERFACE s_axilite port=dout
#pragma HLS INTERFACE s_axilite port=hout
#pragma HLS INTERFACE s_axilite port=wout
#pragma HLS INTERFACE s_axilite port=kernel_size
#pragma HLS INTERFACE s_axilite port=stride
#pragma HLS INTERFACE s_axilite port=padding
#pragma HLS INTERFACE m_axi depth=1024 port=res offset=slave bundle=MEM_W
#pragma HLS INTERFACE s_axilite port=return

	/** For every output channel */
	for (unsigned int out_channel = 0; out_channel < dout; out_channel++) {
		/** For every output row */
		for (unsigned int oh = 0; oh < hout; oh++) {
			int imgStartH = oh * stride - padding;

			unsigned int iStart = imgStartH < 0 ? padding : 0;
			unsigned int iEnd =
					imgStartH + kernel_size >= hin ?
							kernel_size - (imgStartH + kernel_size - hin) :
							kernel_size;

			/** For every output row's pixel */
			for (unsigned int ow = 0; ow < wout; ow++) {
				/** Calculate starting coordinates on the padded matrix */
				int imgStartW = ow * stride - padding;

				unsigned int jStart = imgStartW < 0 ? padding : 0;
				unsigned int jEnd =
						imgStartW + kernel_size >= win ?
								kernel_size - (imgStartW + kernel_size - win) :
								kernel_size;

				/** Initialize output pixel */
				matrix_t pixel = bias[out_channel];
				/** For every input channel */
				for (unsigned int in_channel = 0; in_channel < din;
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
							unsigned int weightsIndex = calc4DIndex(dout, din,
									kernel_size, kernel_size, out_channel, in_channel, i, j);

							/**
							 * Calculate the 1-dimensional representation's
							 * index of the padded matrix
							 */
							unsigned int arrIndex = calc3DIndex(din, hin, win,
									in_channel, imgH, imgW);

							/** Calculate dot product of the two matrices */
							pixel += x[arrIndex] * weights[weightsIndex];
						}
					}
				}

				/**
				 * Calculate the 1-dimensional representation's index of the
				 * output matrix
				 */
				unsigned int resIndex = calc3DIndex(dout, hout, wout,
						out_channel, oh, ow);

				/**
				 * Assign biased dot product result on the corresponding
				 * output pixel
				 */
				res[resIndex] = pixel > 0 ? pixel : 0;
			}
		}
	}

	return 0;
}

