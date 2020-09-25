#include "maxpool_core.hpp"

#define MAX_X_SIZE 55
#define MASK 0xFF

int MaxPool_Core(uint128_t *x, AXI_STREAM &xS, u32 useInStream, u32 d, u32 hin,
		u32 win, u32 hout, u32 wout, u32 kernel_size, u32 stride,
		uint128_t *res, AXI_STREAM &resS, u8 resDest, u32 useOutStream) {
	// clang-format off
#pragma HLS INTERFACE m_axi depth=1024 port=x offset=slave
#pragma HLS INTERFACE axis register both port=xS
#pragma HLS INTERFACE s_axilite port=useInStream
#pragma HLS INTERFACE s_axilite port=d
#pragma HLS INTERFACE s_axilite port=hin
#pragma HLS INTERFACE s_axilite port=win
#pragma HLS INTERFACE s_axilite port=hout
#pragma HLS INTERFACE s_axilite port=wout
#pragma HLS INTERFACE s_axilite port=kernel_size
#pragma HLS INTERFACE s_axilite port=stride
#pragma HLS INTERFACE m_axi depth=1024 port=res offset=slave
#pragma HLS INTERFACE axis register both port=resS
#pragma HLS INTERFACE s_axilite port=resDest
#pragma HLS INTERFACE s_axilite port=useOutStream
#pragma HLS INTERFACE s_axilite port=return
	// clang-format on

	matrix_t xCache[MAX_X_SIZE][MAX_X_SIZE];
#pragma HLS ARRAY_PARTITION variable=xCache complete dim=1

	u32 inputs = hin * win;
	u32 outputs = d * hout * wout;
	u32 resIndex = 0;
	u32 resTmpOffset = 0;
	uint128_t resTmp = 0;

	Loop_every_channel: for (u32 i = 0; i < d; i++) {
#pragma HLS LOOP_TRIPCOUNT min=64 max=64 avg=64
#pragma HLS PIPELINE

		u32 h = 0;
		u32 w = 0;
		u32 k = 0;
		uint128_t xTmp;
		Loop_init_xCache: for (u32 j = 0; j < inputs; j++) {
#pragma HLS LOOP_TRIPCOUNT min=3025 max=3025 avg=3025
#pragma HLS PIPELINE
			if (k == 0) {
				if (useInStream == 0) {
					xTmp = x[j >> 4];
				} else {
					xTmp = xS.read().data;
				}
			}

			matrix_t xMasked = (int) ((matrix_t) (xTmp >> (k << 3) & MASK));
			xCache[h][w] = xMasked;

			k = k == 15 ? 0 : k + 1;
			w = w == win ? 0 : w + 1;
			h = w == 0 ? h + 1 : h;
		}

		matrix_t max;
		Loop_output_row: for (u32 j = 0; j < hout; j++) {
#pragma HLS LOOP_TRIPCOUNT min=27 max=27 avg=27
#pragma HLS PIPELINE
			Loop_output_pixel: for (u32 k = 0; k < wout; k++) {
#pragma HLS LOOP_TRIPCOUNT min=27 max=27 avg=27
#pragma HLS PIPELINE
				u32 rS = j * stride;
				u32 cS = k * stride;

				matrix_t n1 = xCache[rS][cS];
				matrix_t n2 = xCache[rS][cS + 1];
				matrix_t n3 = xCache[rS][cS + 2];
				matrix_t n4 = xCache[rS + 1][cS];
				matrix_t n5 = xCache[rS + 1][cS + 1];
				matrix_t n6 = xCache[rS + 1][cS + 2];
				matrix_t n7 = xCache[rS + 2][cS];
				matrix_t n8 = xCache[rS + 2][cS + 1];
				matrix_t n9 = xCache[rS + 2][cS + 2];

				matrix_t a1 = n1 > n2 ? n1 : n2;
				matrix_t a2 = n3 > n4 ? n3 : n4;
				matrix_t a3 = n5 > n6 ? n5 : n6;
				matrix_t a4 = n7 > n8 ? n7 : n8;

				matrix_t b1 = a1 > a2 ? a1 : a2;
				matrix_t b2 = a3 > a4 ? a3 : a4;

				matrix_t c = b1 > b2 ? b1 : b2;

				max = c > n9 ? c : n9;

				uint128_t max128 = ((uint128_t) max) & MASK;
				uint128_t maxShifted = max128 << (resTmpOffset << 3);
				resTmp |= maxShifted;

				resTmpOffset = resTmpOffset == 15 ? 0 : resTmpOffset + 1;
				if (resTmpOffset == 0) {
					if (useOutStream == 0) {
						res[resIndex] = resTmp;
						resIndex++;
					} else {
						AXI_VALUE128 tmpOut;
						tmpOut.data = resTmp;
						tmpOut.keep = outputs >> 4;
						tmpOut.last = (i == (outputs >> 4) - 1) ? 1 : 0;
						tmpOut.tdest = resDest;
						resS.write(tmpOut);
					}
					resTmp = 0;
				}
			}
		}
	}

	if (resTmpOffset != 0) {
		if (useOutStream == 0) {
			res[resIndex] = resTmp;
			resIndex++;
		} else {
			AXI_VALUE128 tmpOut;
			tmpOut.data = resTmp;
			tmpOut.keep = outputs >> 4;
			tmpOut.last = 1;
			tmpOut.tdest = resDest;
			resS.write(tmpOut);
		}
	}
	return 0;
}
