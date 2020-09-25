#include <stdlib.h>
#include <stdio.h>
#include <hls_stream.h>
#include <string.h>
#include <ap_int.h>

typedef unsigned char u8;
typedef ap_int<8> matrix_t;
typedef unsigned int u32;
typedef ap_uint<128> uint128_t;

typedef struct {
		uint128_t data;
		ap_uint<1> last;
		ap_uint<16> keep;
		ap_uint<8> tdest;
} AXI_VALUE128;
typedef hls::stream<AXI_VALUE128> AXI_STREAM;

int Conv_Core(uint128_t *x, AXI_STREAM &xS, u32 useInStream, int xScale,
		uint128_t *weights, int weightsScale, uint128_t *bias, int biasScale,
		u32 din, u32 hin, u32 win, u32 dout, u32 hout, u32 wout,
		u32 kernel_size, u32 stride, u32 padding, u32 doReLU, uint128_t *res,
		int resTopBit, AXI_STREAM &resS, u8 resDest, u32 useOutStream);


#define MASK 0xFF
#define MAX_KERNEL_SIZE 11
#define MAX_INPUT_CHANNEL 3
#define MAX_INPUT_HEIGHT 224
#define MAX_INPUT_WIDTH 224
#define MAX_X_SIZE 150528
#define MAX_OUTPUT_HEIGHT 55
#define MAX_OUTPUT_WIDTH 55
//#define MAX_OUTPUT_CHANNELS 64
#define MAX_OUTPUT_CHANNELS 384
