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

int MaxPool_Core(matrix_t *x, unsigned int d, unsigned int hin, unsigned int win,
		unsigned int hout, unsigned int wout, unsigned int kernel_size,
		unsigned int stride, matrix_t *res) {
#pragma HLS INTERFACE m_axi depth=1024 port=x offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE s_axilite port=d bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=hin bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=win bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=hout bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=wout bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=kernel_size bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=stride bundle=CRTL_BUS
#pragma HLS INTERFACE m_axi depth=1024 port=res offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS

	/** For every channel */
	for (int i = 0; i < d; i++) {
		/** For every output row */
		for (int j = 0; j < hin; j++) {
			/** For every output row's pixel */
			for (int k = 0; k < win; k++) {
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
						unsigned int index = calc3DIndex(d, hin, win, i, l, m);
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
				unsigned int resIndex = calc3DIndex(d, hout, wout, i, j, k);

				/** Assign found max value to the output matrix */
				res[resIndex] = max;
			}
		}
	}

	return 0;
}
