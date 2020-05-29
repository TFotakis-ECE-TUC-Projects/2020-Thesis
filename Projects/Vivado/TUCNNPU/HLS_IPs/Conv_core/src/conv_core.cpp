#include <string.h>

#define MAX_X_SIZE 193600

typedef unsigned int u32;

/** Matrix data type for easy switching when needed to other types */
 typedef float matrix_t;
//#define AP_INT_MAX_W 16
//#include <ap_fixed.h>
//typedef ap_fixed<16, 2, AP_RND> matrix_t;

/**
 * Calculates the index on a 1D buffer representation of a 3D matrix
 * @param[in] dim0 - dim2: the sizes of the matrix's dimensions
 * @param[in] i: The plain dimension
 * @param[in] j: The row dimension
 * @param[in] k: The column dimension
 * @returns the 1D representation's index of a 3D matrix
 */
#define calc3DIndex(dim0, dim1, dim2, i, j, k) (k + j * dim2 + i * dim1 * dim2)

/**
 * Calculates the index on a 1D buffer representation of a 4D matrix
 * @param[in] dim0 - dim3: the sizes of the matrix's dimensions
 * @param[in] i: The forth dimension
 * @param[in] j: The plain dimension
 * @param[in] k: The row dimension
 * @param[in] l: The column dimension
 * @returns the 1D representation's index of a 4D matrix
 */
#define calc4DIndex(dim0, dim1, dim2, dim3, i, j, k, l) \
	(l + k * dim3 + j * dim2 * dim3 + i * dim1 * dim2 * dim3)

int Conv_Core(
	matrix_t *x,
	matrix_t *weights,
	matrix_t *bias,
	u32 din,
	u32 hin,
	u32 win,
	u32 dout,
	u32 hout,
	u32 wout,
	u32 kernel_size,
	u32 stride,
	u32 padding,
	matrix_t *res) {
#pragma HLS INTERFACE m_axi depth = 1024 port = x offset = slave bundle = \
	MASTER_BUS
#pragma HLS INTERFACE m_axi depth = 1024 port = weights offset = \
	slave bundle = MASTER_BUS
#pragma HLS INTERFACE m_axi depth = 1024 port = bias offset = slave bundle = \
	MASTER_BUS
#pragma HLS INTERFACE s_axilite port = din bundle = CRTL_BUS
#pragma HLS INTERFACE s_axilite port = hin bundle = CRTL_BUS
#pragma HLS INTERFACE s_axilite port = win bundle = CRTL_BUS
#pragma HLS INTERFACE s_axilite port = dout bundle = CRTL_BUS
#pragma HLS INTERFACE s_axilite port = hout bundle = CRTL_BUS
#pragma HLS INTERFACE s_axilite port = wout bundle = CRTL_BUS
#pragma HLS INTERFACE s_axilite port = kernel_size bundle = CRTL_BUS
#pragma HLS INTERFACE s_axilite port = stride bundle = CRTL_BUS
#pragma HLS INTERFACE s_axilite port = padding bundle = CRTL_BUS
#pragma HLS INTERFACE m_axi depth = 1024 port = res offset = slave bundle = \
	MASTER_BUS
#pragma HLS INTERFACE s_axilite port = return bundle = CRTL_BUS
	//	u32 size = dout * hout * wout;
	//	if (size > 100000000)
	//		return -1;
	//	for (u32 i = 0; i < size; i++) {
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
	memcpy(xCache, x, din * hin * win * sizeof(matrix_t));

Loop_output_channel:
	/** For every output channel */
	for (u32 out_channel = 0; out_channel < dout; out_channel++) {
		// Todo: Fix hardcoded dims
		matrix_t pixels[55][55];

		matrix_t biasCache = bias[out_channel];
	Loop_pixels_row_init:
		for (u32 i = 0; i < hout; i++) {
		Loop_pixels_pixel_init:
			for (u32 j = 0; j < wout; j++) { pixels[i][j] = biasCache; }
		}

	Loop_kernel:
		/** For every input channel */
		for (u32 in_channel = 0; in_channel < din; in_channel++) {
			// Todo: Fix hardcoded dims
			matrix_t weightsCache[11 * 11];
			memcpy(
				weightsCache,
				weights +
					(out_channel * din * kernel_size * kernel_size +
					 in_channel * kernel_size * kernel_size),
				11 * 11 * sizeof(matrix_t));

		Loop_output_row:
			/** For every output row */
			for (u32 oh = 0; oh < hout; oh++) {
				int imgStartH = oh * stride - padding;

			Loop_output_pixel:
				/** For every output row's pixel */
				for (u32 ow = 0; ow < wout; ow++) {
					/** Calculate starting coordinates on the padded matrix */
					int imgStartW = ow * stride - padding;

				Loop_kernel_row:
					/** For kernel_size rows on padded matrix */
					for (u32 i = 0; i < kernel_size; i++) {
					/** For kernel_size pixels on each padded matrix's row */
					Loop_kernel_pixel:
						for (u32 j = 0; j < kernel_size; j++) {
							int imgH = i + imgStartH;
							int imgW = j + imgStartW;

							if (imgH < 0 || imgH >= hin || imgW < 0 ||
								imgW >= win)
								continue;
							/**
							 * Calculate the 1-dimensional representation's
							 * index of the padded matrix
							 */
							u32 arrIndex = calc3DIndex(
								din, hin, win, in_channel, imgH, imgW);

							/** Calculate dot product of the two matrices */
							pixels[oh][ow] +=
								xCache[arrIndex] * weightsCache[i * 11 + j];
						}
					}
				}
			}
		}

		memcpy(
			res + (out_channel * hout * wout),
			pixels,
			hout * wout * sizeof(matrix_t));
	}

	return 0;
}
