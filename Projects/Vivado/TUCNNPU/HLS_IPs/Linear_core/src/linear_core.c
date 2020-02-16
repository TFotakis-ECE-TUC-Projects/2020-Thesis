#define MAX_X_SIZE 9216

/** Matrix data type for easy switching when needed to other types */
typedef float matrix_t;

int Linear_Core(matrix_t *x, matrix_t *weights, matrix_t *bias,
		unsigned int in_features, unsigned int out_features, unsigned int doReLU, matrix_t *res) {
#pragma HLS INTERFACE m_axi depth=1024 port=x offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE m_axi depth=1024 port=weights offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE m_axi depth=1024 port=bias offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE s_axilite port=in_features bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=out_features bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=doReLU bundle=CRTL_BUS
#pragma HLS INTERFACE m_axi depth=1024 port=res offset=slave bundle=MASTER_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS

//	in_features = 9216;
//	out_features = 4096;

	matrix_t xCache[MAX_X_SIZE];
	for (unsigned int k = 0; k < in_features; k++) {
#pragma HLS PIPELINE
		xCache[k] = x[k];
	}

	matrix_t value;
	/** For every output feature */
	Loop_out_features:
	for (unsigned int i = 0; i < out_features; i++) {
		/** Initialize output feature with its bias */
		value = bias[i];

		/** For every input feature */
		Loop_in_features:
		for (unsigned int j = 0; j < in_features; j++) {
			/** Add to the output feature the input feature's weighted value */
			value += xCache[j] * weights[j + i * in_features];
		}

		res[i] = doReLU ? value > 0 ? value : 0 : value;
	}

	return 0;
}
