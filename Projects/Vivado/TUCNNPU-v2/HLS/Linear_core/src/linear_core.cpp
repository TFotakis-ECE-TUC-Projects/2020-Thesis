#include "linear_core.hpp"

#define M_AXI_DEPTH 1024
#define UNROLL_FACTOR 32
#define MAX_X_SIZE 9216
#define MAX_RES_SIZE 4096
#define MASK 0xFF

int Linear_Core(uint128_t *x, AXI_STREAM &xS, u32 useInStream, int xScale,
		uint128_t *weights, int weightsScale, uint128_t *bias, int biasScale,
		u32 in_features, u32 out_features, u32 doReLU, uint128_t *res,
		int resTopBit, AXI_STREAM &resS, u8 resDest, u32 useOutStream) {
	// clang-format off
#pragma HLS INTERFACE m_axi depth=1024 port=x offset=slave
#pragma HLS INTERFACE axis register both port=xS
#pragma HLS INTERFACE s_axilite port=useInStream
#pragma HLS INTERFACE s_axilite port=xScale
#pragma HLS INTERFACE m_axi depth=1024 port=weights offset=slave
#pragma HLS INTERFACE s_axilite port=weightsScale
#pragma HLS INTERFACE m_axi depth=1024 port=bias offset=slave
#pragma HLS INTERFACE s_axilite port=biasScale
#pragma HLS INTERFACE s_axilite port=in_features
#pragma HLS INTERFACE s_axilite port=out_features
#pragma HLS INTERFACE s_axilite port=doReLU
#pragma HLS INTERFACE m_axi depth=1024 port=res offset=slave
#pragma HLS INTERFACE s_axilite port=resTopBit
#pragma HLS INTERFACE axis register both port=resS
#pragma HLS INTERFACE s_axilite port=resDest
#pragma HLS INTERFACE s_axilite port=useOutStream
#pragma HLS INTERFACE s_axilite port=return
	// clang-format on

	int biasShift = xScale + weightsScale - biasScale;

	int resCache[MAX_RES_SIZE];
	u32 k = 0;
	Loop_init_resCache_read: for (u32 i = 0; i < (out_features >> 4); i++) {
#pragma HLS LOOP_TRIPCOUNT min=256 max=256 avg=256
#pragma HLS PIPELINE
		uint128_t tmpBias = bias[i];
		Loop_init_resCache_write: for (u32 j = 0; j < 16; j++) {
			int tmpScaledBias = (int) ((matrix_t) (tmpBias >> (j << 3) & MASK));
			tmpScaledBias <<= biasShift;
			resCache[k] = tmpScaledBias;
			k++;
		}
	}

	if (k != out_features) {
		uint128_t tmpBias = bias[(out_features >> 4) + 1];
		Loop_init_resCache_write_last: for (u32 j = k; j < out_features; j++) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=16 avg=16
#pragma HLS PIPELINE
			int tmpScaledBias = (int) ((matrix_t) (tmpBias >> (j << 3) & MASK));
			tmpScaledBias <<= biasShift;
			resCache[k] = tmpScaledBias;
		}
	}

	matrix_t xCache[16];
#pragma HLS ARRAY_PARTITION variable=xCache complete dim=1
	uint128_t xTmp, wTmp;

	Loop_in_features: for (u32 j = 0; j < in_features >> 4; j++) {
#pragma HLS LOOP_TRIPCOUNT min=576 max=576 avg=576
		if (useInStream == 0) {
			xTmp = x[j];
		} else {
			xTmp = xS.read().data;
		}

		Loop_out_features: for (u32 i = 0; i < out_features; i++) {
#pragma HLS LOOP_TRIPCOUNT min=4096 max=4096 avg=4096
#pragma HLS PIPELINE
			wTmp = weights[(i >> 4) + j * (out_features >> 4)];

			int a[16];
#pragma HLS ARRAY_PARTITION variable=a complete dim=1
			Loop_multiplierStage: for (u32 k = 0; k < 16; k++) {
#pragma HLS UNROLL
				a[k] = ((xTmp >> (k << 3)) & MASK)
						* ((wTmp >> (k << 3)) & MASK);
			}

			int b[8];
#pragma HLS ARRAY_PARTITION variable=b complete dim=1
			Loop_adderStage1: for (u32 k = 0; k < 8; k++) {
#pragma HLS UNROLL
				b[k] = a[(k << 1)] + a[(k << 1) + 1];
			}

			int c[4];
#pragma HLS ARRAY_PARTITION variable=c complete dim=1
			Loop_adderStage2: for (u32 k = 0; k < 4; k++) {
#pragma HLS UNROLL
				c[k] = b[(k << 1)] + b[(k << 1) + 1];
			}

			int d = c[0] + c[1];
			int e = c[2] + c[3];
			int f = d + e;

			resCache[i] += f;
		}
	}

	uint128_t resTmp;
	int hi = resTopBit + 1;
	int lo = resTopBit - 6;
	Loop_resPush: for (u32 i = 0; i < (out_features >> 4); i++) {
#pragma HLS LOOP_TRIPCOUNT min=256 max=256 avg=256
#pragma HLS PIPELINE
		resTmp = 0;
		Loop_resTmp: for (u32 k = 0; k < 16; k++) {
			int value = resCache[(i << 4) + k];
			ap_int<32> valueReLU = ((doReLU != 0) && (value < 0)) ? 0 : value;
			ap_uint<8> valueCompressed = valueReLU.range(hi, lo);
			int resHi = ((k + 1) << 3) - 1;
			int resLo = k << 3;
			resTmp.range(resHi, resLo) = valueCompressed.range(7, 0);
		}
		if (useOutStream == 0) {
			res[i] = resTmp;
		} else {
			AXI_VALUE128 tmpOut;
			tmpOut.data = resTmp;
			tmpOut.keep = out_features >> 4;
			tmpOut.last = (i == (out_features >> 4) - 1) ? 1 : 0;
			tmpOut.tdest = resDest;
			resS.write(tmpOut);
		}
	}

	return 0;
}
