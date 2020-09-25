#include <stdlib.h>
#include <stdio.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <string.h>
#include <ap_int.h>

typedef unsigned char u8;
typedef ap_int<8> matrix_t;
typedef unsigned int u32;
typedef ap_uint<128> uint128_t;

struct AXI_VALUE128 {
		uint128_t data;
		ap_uint<1> last;
		ap_uint<16> keep;
		ap_uint<8> tdest;
};
typedef hls::stream<AXI_VALUE128> AXI_STREAM;

int Linear_Core(uint128_t *x, AXI_STREAM &xS, u32 useInStream, int xScale,
		uint128_t *weights, int weightsScale, uint128_t *bias, int biasScale,
		u32 in_features, u32 out_features, u32 doReLU, uint128_t *res, int resTopBit,
		AXI_STREAM &resS, u8 resDest, u32 useOutStream);
