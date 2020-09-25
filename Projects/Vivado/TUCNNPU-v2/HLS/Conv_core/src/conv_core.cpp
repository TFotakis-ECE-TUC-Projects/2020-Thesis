#include "conv_core.hpp"

static matrix_t xCache[MAX_X_SIZE];
static matrix_t weightsCache[MAX_KERNEL_SIZE][MAX_KERNEL_SIZE];
static int resCache[MAX_OUTPUT_HEIGHT][MAX_OUTPUT_WIDTH];
static int biasCache[MAX_OUTPUT_CHANNELS];

void init_xCache(uint128_t *x, AXI_STREAM &xS, u32 useInStream, u32 din,
		u32 hin, u32 win) {
	int xSize = din * hin * win;

	u32 xci = 0;
	uint128_t tmpX;
	Loop_xCache_init: for (u32 i = 0; i < (xSize >> 4); i++) {
#pragma HLS LOOP_TRIPCOUNT min=384 max=384 avg=384
#pragma HLS PIPELINE
		if (useInStream == 0) {
			tmpX = x[i];
		} else {
			tmpX = xS.read().data;
		}

		Loop_xCache_read: for (u32 i = 0; i < 16; i++) {
#pragma HLS UNROLL
			int tmpMaskedX = (int) ((matrix_t) (tmpX >> (i << 3) & MASK));
			xCache[xci] = tmpMaskedX;
			xci++;
		}
	}
	if (xci < xSize) {
		if (useInStream == 0) {
			tmpX = x[(xSize >> 4)];
		} else {
			tmpX = xS.read().data;
		}
		u32 remaining = xSize - xci;
		Loop_xCache_read_last: for (u32 i = 0; i < remaining; i++) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=16 avg=16
#pragma HLS UNROLL factor=16
			int tmpMaskedX = (int) ((matrix_t) (tmpX >> (i << 3) & MASK));
			xCache[xci] = tmpMaskedX;
			xci++;
		}
	}
}

void init_biasCache(uint128_t *bias, u32 dout, int xScale, int weightsScale,
		int biasScale) {
	int biasShift = xScale + weightsScale - biasScale;
	u32 k = 0;
	uint128_t tmpBias;
	Loop_biasCache_init: for (u32 oc = 0; oc < dout; oc++) {
#pragma HLS LOOP_TRIPCOUNT min=4 max=4 avg=4
#pragma HLS PIPELINE
		if (k == 0) {
			tmpBias = bias[oc];
		}

		int tmpScaledBias = (int) ((matrix_t) (tmpBias >> (k << 3) & MASK));
		tmpScaledBias <<= biasShift;
		biasCache[oc] = tmpScaledBias;
		k = k == 15 ? 0 : k + 1;
	}
}

void init_resCache(u32 hout, u32 wout, u32 oc) {
	Loop_resCache_init_row: for (u32 bh = 0; bh < hout; bh++) {
#pragma HLS LOOP_TRIPCOUNT min=55 max=55 avg=55
#pragma HLS PIPELINE
		Loop_resCache_init_col: for (u32 bc = 0; bc < wout; bc++) {
#pragma HLS LOOP_TRIPCOUNT min=55 max=55 avg=55
			resCache[bh][bc] = biasCache[oc];
		}
	}
}

u32 weightsSize;
void init_weightsCache(uint128_t *weights, u32 kernel_size) {
	uint128_t tmpWeights;
	u32 h = 0;
	u32 w = 0;
	u32 k = 0;
	Loop_weightsCache_init: for (u32 i = 0; i < weightsSize; i++) {
#pragma HLS LOOP_TRIPCOUNT min=121 max=121 avg=121
#pragma HLS PIPELINE
		if (k == 0) {
			tmpWeights = weights[w >> 4];
		}
		int tmpMaskedBias = (int) ((matrix_t) (tmpWeights >> (k << 3) & MASK));
		weightsCache[h][w] = tmpMaskedBias;

		k = k == 15 ? 0 : k + 1;
		w = w == kernel_size - 1 ? 0 : w + 1;
		h = w == 0 ? h + 1 : h;
	}
}

u32 resIndex;
u32 resTmpOffset = 0;
uint128_t resTmp = 0;
int hi;
int lo;
u32 outputs;

void resTmp_write(uint128_t *res, AXI_STREAM &resS, u8 resDest,
		u32 useOutStream) {
	if (useOutStream == 0) {
		res[resIndex] = resTmp;
	} else {
		AXI_VALUE128 tmpOut;
		tmpOut.data = resTmp;
		tmpOut.keep = outputs >> 4;
		tmpOut.last = resIndex == outputs - 1;
		tmpOut.tdest = resDest;
		resS.write(tmpOut);
	}
	resTmp = 0;
	resIndex++;
}

void res_writeback(u32 oc, u32 dout, u32 hout, u32 wout, u32 doReLU,
		uint128_t *res, AXI_STREAM &resS, u8 resDest, u32 useOutStream) {
	Loop_res_h: for (u32 oh = 0; oh < hout; oh++) {
#pragma HLS LOOP_TRIPCOUNT min=55 max=55 avg=55
#pragma HLS PIPELINE
		Loop_res_w: for (u32 ow = 0; ow < wout; ow++) {
#pragma HLS LOOP_TRIPCOUNT min=55 max=55 avg=55
			int value = resCache[oh][ow];
			ap_int<32> valueReLU = ((doReLU != 0) && (value < 0)) ? 0 : value;
			ap_uint<8> valueCompressed = valueReLU.range(hi, lo);
			int resHi = ((resTmpOffset + 1) << 3) - 1;
			int resLo = resTmpOffset << 3;
			resTmp.range(resHi, resLo) = valueCompressed.range(7, 0);
			resTmpOffset = resTmpOffset == 15 ? 0 : resTmpOffset + 1;

			if (resTmpOffset == 0) {
				resTmp_write(res, resS, resDest, useOutStream);
			}
		}
	}
}

int Conv_Core(uint128_t *x, AXI_STREAM &xS, u32 useInStream, int xScale,
		uint128_t *weights, int weightsScale, uint128_t *bias, int biasScale,
		u32 din, u32 hin, u32 win, u32 dout, u32 hout, u32 wout,
		u32 kernel_size, u32 stride, u32 padding, u32 doReLU, uint128_t *res,
		int resTopBit, AXI_STREAM &resS, u8 resDest, u32 useOutStream) {
#pragma HLS INTERFACE m_axi depth=1024 port=x offset=slave
#pragma HLS INTERFACE axis register both port=xS
#pragma HLS INTERFACE s_axilite port=useInStream
#pragma HLS INTERFACE s_axilite port=xScale
#pragma HLS INTERFACE m_axi depth=1024 port=weights offset=slave
#pragma HLS INTERFACE s_axilite port=weightsScale
#pragma HLS INTERFACE m_axi depth=1024 port=bias offset=slave
#pragma HLS INTERFACE s_axilite port=biasScale
#pragma HLS INTERFACE s_axilite port=din
#pragma HLS INTERFACE s_axilite port=hin
#pragma HLS INTERFACE s_axilite port=win
#pragma HLS INTERFACE s_axilite port=dout
#pragma HLS INTERFACE s_axilite port=hout
#pragma HLS INTERFACE s_axilite port=wout
#pragma HLS INTERFACE s_axilite port=kernel_size
#pragma HLS INTERFACE s_axilite port=stride
#pragma HLS INTERFACE s_axilite port=padding
#pragma HLS INTERFACE s_axilite port=doReLU
#pragma HLS INTERFACE m_axi depth=1024 port=res offset=slave
#pragma HLS INTERFACE s_axilite port=resTopBit
#pragma HLS INTERFACE axis register both port=resS
#pragma HLS INTERFACE s_axilite port=resDest
#pragma HLS INTERFACE s_axilite port=useOutStream
#pragma HLS INTERFACE s_axilite port=return

#pragma HLS ARRAY_PARTITION variable=xCache cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=weightsCache complete dim=2
//#pragma HLS ARRAY_PARTITION variable=biasCache complete dim=1

	outputs = dout * hout * wout;

	init_xCache(x, xS, useInStream, din, hin, win);

	init_biasCache(bias, dout, xScale, weightsScale, biasScale);

	hi = resTopBit + 1;
	lo = resTopBit - 6;
	resIndex = 0;
	weightsSize = kernel_size * kernel_size;
	Loop_channel_out: for (u32 oc = 0; oc < dout; oc++) {
#pragma HLS LOOP_TRIPCOUNT min=64 max=64 avg=64
#pragma HLS PIPELINE

		init_resCache(hout, wout, oc);

		Loop_channel_in: for (u32 ic = 0; ic < din; ic++) {
#pragma HLS LOOP_TRIPCOUNT min=3 max=3 avg=3

			init_weightsCache(weights, kernel_size);

			Loop_height_out: for (u32 oh = 0; oh < hout; oh++) {
#pragma HLS LOOP_TRIPCOUNT min=55 max=55 avg=55
				int inStartH = oh * stride - padding;

				Loop_width_out: for (u32 ow = 0; ow < wout; ow++) {
#pragma HLS LOOP_TRIPCOUNT min=55 max=55 avg=55
					int inStartW = ow * stride - padding;

					Loop_height_in: for (u32 kh = 0; kh < kernel_size; kh++) {
#pragma HLS LOOP_TRIPCOUNT min=11 max=11 avg=11
						int ih = kh + inStartH;

						int a[MAX_KERNEL_SIZE];
#pragma HLS ARRAY_PARTITION variable=a complete dim=1

						Loop_width_in: for (u32 kw = 0; kw < MAX_KERNEL_SIZE;
								kw++) {
#pragma HLS LOOP_TRIPCOUNT min=11 max=11 avg=11
#pragma HLS UNROLL
							int iw = kw + inStartW;
							if (kw < kernel_size && ih >= 0 && iw >= 0) {
								int xIndex = ic * hin * win + ih * win + iw;
								a[kw] = ((int) xCache[xIndex])
										* ((int) weightsCache[kh][kw]);
							} else {
								a[kw] = 0;
							}
						}

						int b1 = a[0] + a[1];
						int b2 = a[2] + a[3];
						int b3 = a[4] + a[5];
						int b4 = a[6] + a[7];
						int b5 = a[8] + a[9];

						int c1 = b1 + b2;
						int c2 = b3 + b4;
						int c3 = b5 + a[10];

						int d1 = c1 + c2;
						int d2 = c3 + resCache[oh][ow];

						resCache[oh][ow] = d1 + d2;
					}
				}
			}
		}

		res_writeback(oc, dout, hout, wout, doReLU, res, resS, resDest,
				useOutStream);
	}

	if (resTmpOffset != 0) {
		resTmp_write(res, resS, resDest, useOutStream);
	}

	return 0;
}
