/*
******************************************************************************
  Vivado HLS (R) is a trademark of The Xilinx (R) Corporation

  Filename:    conv.h
  Author:      Pitsis Giorgos
  Version:     1.0
  Date:        September 29, 2018
  Copyright:   (c) 2018 by Pitsis Giorgos, All Rights Reserved
******************************************************************************
*/

#include <ap_axi_sdata.h>
#include <assert.h>
#include <hls_stream.h>
#include <stdint.h>
#include "Conv_def.h"

/*
  Struct for the input stream
*/

struct AXI_VALUE {
	ap_uint<32> data;
	ap_uint<1> last;
	ap_uint<4> keep;
};

/*
  Struct for the output stream (output of 3 Convolutional Layers
*/
struct AXI_VALUE_extended {
	ap_uint<512> data;
	ap_uint<1> last;
	ap_uint<64> keep;
};

/*
  Struct for the FIFOs to communicate modules Conv1, Conv2, Conv3 between them.
 */
struct AXI_FIFO {
	ap_uint<512> data;
};

typedef hls::stream<AXI_VALUE> AXI_STREAM;
typedef hls::stream<AXI_VALUE_extended> AXI_STREAM_ext;

/*
 * Num_K0 : Length of Kernel 0
 * Num_K1 : Length of Kernel 1 and Kernel_2
 * Num_B : Length of bias
 * Num_Im : Length of Image
 */
#define Num_K0 128
#define Num_K1 2048
#define Num_B 16
#define Num_Im 1800

// function prototypes
void Conv_Accel(AXI_STREAM &inStream, AXI_STREAM_ext &StreamOut_Inst1_1,
				AXI_STREAM_ext &StreamOut_Inst1_2,
				AXI_STREAM_ext &StreamOut_Inst2_1,
				AXI_STREAM_ext &StreamOut_Inst2_2);

/*
 * Function: Convolution2 (Uses 2 two times as Conv2 and Conv3 through template)
 *	This function performs a Convolutional Layer 2-D with 16 different kernel
 *size of 16*8 each. The convolution operation is valid (non-zero padding)
 *
 * Typename T : Support any type of variable (float in our case)
 * DIM1 : 1800
 * DIM2 : 16
 * DIM3 : 8
 *
 * Inputs :
 *	 	     kernel : 3 Dimension Array (16,16,8) contains weights for the
 *convolution bias : Array (16) added after convolution operation ImageStream :
 *FIFO 32 bit width that streams in the module the Image ShiftedFifoIn : Shifted
 *FIFO 512 bit width (Packed 16 results from previous layer). Reg_Conv :
 *Register 512 bit width (Packed 16 previous layer results read from FifoOut)
 *contains 8 elements (overall the necessary 16*8 elements to produce a result
 *in this stage)
 *
 * Inputs/Outputs :
 *	 	   			 Conv1Fifo : FIFO 32 bit width that passes every Conv result
 *to the next module FifoOut :
 */
template <typename T, int DIM1, int DIM2, int DIM3>
void Convolution2(hls::stream<AXI_FIFO> &ShiftedFifoIn,
				  T kernel[DIM2][DIM2][DIM3], hls::stream<float> &ConvFifo,
				  T bias[DIM2], ap_uint<512> Reg_Conv[DIM3]) {
#pragma HLS INLINE

	float inner_prod;
	float sum = 0;

	union {
		float a;
		uint32_t b;
	} Data_tmpIn;
#pragma HLS ARRAY_PARTITION variable = kernel block factor = 8 dim = 2
#pragma HLS RESOURCE variable = sum core = FAddSub_fulldsp
#pragma HLS RESOURCE variable = inner_prod core = FMul_fulldsp
	int ping_pong = 0;
Conv2_L2:
	for (int i = 0; i < (DIM1 - DIM3 + 1); i++) {
		ping_pong = 0;
	Conv2_L1:
		for (int k = 0; k < DIM2; k++) {
#pragma HLS PIPELINE II = 8
			sum = bias[k];
		Conv2_L4:
			for (int j = 0; j < DIM3; j++) {
				/*
				 * Read i*j times from the stream (only for k=0)
				 * for k=1:DIM2 (rest of kernels) read from Reg_Conv
				 */

				if (ping_pong == 0) {
#pragma HLS occurrence cycle = DIM3
					AXI_FIFO temp2 = ShiftedFifoIn.read();
					ap_uint<512> temp = temp2.data;
					Reg_Conv[j] = temp;
				}

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift0) & 0xFFFFFFFF);
				T inner_prod1 = Data_tmpIn.a * kernel[k][0][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift1) & 0xFFFFFFFF);
				T inner_prod2 = Data_tmpIn.a * kernel[k][1][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift2) & 0xFFFFFFFF);
				T inner_prod3 = Data_tmpIn.a * kernel[k][2][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift3) & 0xFFFFFFFF);
				T inner_prod4 = Data_tmpIn.a * kernel[k][3][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift4) & 0xFFFFFFFF);
				T inner_prod5 = Data_tmpIn.a * kernel[k][4][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift5) & 0xFFFFFFFF);
				T inner_prod6 = Data_tmpIn.a * kernel[k][5][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift6) & 0xFFFFFFFF);
				T inner_prod7 = Data_tmpIn.a * kernel[k][6][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift7) & 0xFFFFFFFF);
				T inner_prod8 = Data_tmpIn.a * kernel[k][7][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift8) & 0xFFFFFFFF);
				T inner_prod9 = Data_tmpIn.a * kernel[k][8][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift9) & 0xFFFFFFFF);
				T inner_prod10 = Data_tmpIn.a * kernel[k][9][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift10) & 0xFFFFFFFF);
				T inner_prod11 = Data_tmpIn.a * kernel[k][10][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift11) & 0xFFFFFFFF);
				T inner_prod12 = Data_tmpIn.a * kernel[k][11][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift12) & 0xFFFFFFFF);
				T inner_prod13 = Data_tmpIn.a * kernel[k][12][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift13) & 0xFFFFFFFF);
				T inner_prod14 = Data_tmpIn.a * kernel[k][13][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift14) & 0xFFFFFFFF);
				T inner_prod15 = Data_tmpIn.a * kernel[k][14][j];

				Data_tmpIn.b =
					(uint32_t)(((Reg_Conv[j]) >> conv_shift15) & 0xFFFFFFFF);
				T inner_prod16 = Data_tmpIn.a * kernel[k][15][j];
				T sum1 = inner_prod1 + inner_prod2 + inner_prod3;

				T sum2 = inner_prod4 + inner_prod5 + inner_prod6 + inner_prod7;
				T sum3 =
					inner_prod8 + inner_prod9 + inner_prod10 + inner_prod11;
				T sum4 = inner_prod12 + inner_prod13 + inner_prod14 +
						 inner_prod15 + inner_prod16;
				sum += sum1 + sum2 + sum3 + sum4;
			}
			ping_pong = 1;
			// ReLU
			if (sum < 0) sum = 0;
			ConvFifo.write(sum);
		}
	}
}

/*
 *  Function : Convolution1
 *	This function performs a Convolutional Layer 1-D with 16 different kernel
 *size of 1*8 each. The convolution operation is valid (non-zero padding)
 *
 * Typename T : Support any type of variable (float in our case)
 * DIM1 : 1800
 * DIM2 : 16
 * DIM3 : 8
 *
 * Inputs :
 *	 	    kernel_0 : 3 Dimension Array (16,16,8) contains weights for the
 *convolution bias : Array (16) added after convolution operation ImageStream :
 *FIFO 32 bit width that streams in the module the Image RegImage : Shift
 *register (32 bit width) 8 elements
 *
 * Inputs/Outputs :
 *	 	  			Conv1Fifo : FIFO 32 bit width that passes every Conv result
 *to the next module
 */
template <typename T, int DIM1, int DIM2, int DIM3>
void Convolution1(hls::stream<float> &ImageStream, T kernel[DIM2][DIM3],
				  T bias[DIM2], hls::stream<float> &Conv1Fifo,
				  float RegImage[DIM3]) {
	int const FACTOR = 16;
#pragma HLS INLINE
	float inner_prod;
	float sum;
	union {
		float a;
		uint32_t b;
	} Data_tmpIn;
	AXI_VALUE AXI_tmpIn;

#pragma HLS RESOURCE variable = sum core = FAddSub_fulldsp
#pragma HLS RESOURCE variable = inner_prod core = FMul_fulldsp

Conv_L12:
	for (int i = 0; i < DIM3 - 1; i++) {
#pragma HLS PIPELINE II = 1
		/*
		 * Read Image
		 */
		RegImage[i] = ImageStream.read();
	}

Conv_L1:
	for (int i = 0; i < (DIM1 - DIM3 + 1); i++) {
#pragma HLS PIPELINE II = 1
		/*
		 * Read Image and perform shift register behavior
		 */
		RegImage[(i + DIM3 - 1) % DIM3] = ImageStream.read();
	Conv_L2:
		for (int k = 0; k < DIM2; k++) {
			sum = bias[k];
		Conv_L3:
			for (int j = 0; j < DIM3; j++) {
				inner_prod = RegImage[(i + j) % DIM3] * kernel[k][j];
				sum += inner_prod;
			}
			// ReLU
			if (sum < 0) sum = 0;
			Conv1Fifo.write(sum);
		}
	}
	return;
}

/*
 * Function: Fifo_To_PackedFifo
 *	This function is fed results from FifoIn and when it reaches first 32 it
 *packed this data in 2 streams read and send the through FIFOs.
 *
 * Typename T : Support any type of variable (float in our case)
 * DIM1 : 1800
 * DIM2 : 16
 * DIM3 : 8
 *
 * Inputs :
 *	 	    FifoIn : FIFO 32 bit width that transfers result from previous
 *Convolutional Layers PackedReg : Array that contains 32 results of the three
 *Convolutional Layers to packed to the two ouput FIFOs
 *
 * Inputs/Outputs :
 *	 	  			FifoOut1 = FIFO 512 bit width that exports first 16 data of
 *3 conv_layers FifoOut2 = FIFO 512 bit width that exports second 16 data of 3
 *conv_layers
 */
template <typename T, int DIM1, int DIM2, int DIM3>
void PackedFifo(hls::stream<float> &FifoIn, AXI_STREAM_ext &StreamOut_Inst1_1,
				AXI_STREAM_ext &StreamOut_Inst1_2,
				AXI_STREAM_ext &StreamOut_Inst2_1,
				AXI_STREAM_ext &StreamOut_Inst2_2, uint32_t PackedReg[32]) {
	union {
		float a;
		uint32_t b;
	} Data_tmpIn;

/*
 * float32_1 are the first 16 results that packed and send through stream
 * float32_2 are the second 16 results that packed and send through stream
 *
 */
l1:
	for (int i = 0; i < 889; i++) {
#pragma HLS PIPELINE II = 32
	l2:
		for (int j = 0; j < 32; j++) {
			Data_tmpIn.a = FifoIn.read();
			PackedReg[j] = Data_tmpIn.b;
		}

		ap_uint<512> float32_1 = (ap_uint<512>)PackedReg[0] << 480 |
								 (ap_uint<480>)PackedReg[1] << 448 |
								 (ap_uint<448>)PackedReg[2] << 416 |
								 (ap_uint<416>)PackedReg[3] << 384 |
								 (ap_uint<384>)PackedReg[4] << 352 |
								 (ap_uint<352>)PackedReg[5] << 320 |
								 (ap_uint<320>)PackedReg[6] << 288 |
								 (ap_uint<288>)PackedReg[7] << 256 |
								 (ap_uint<256>)PackedReg[8] << 224 |
								 (ap_uint<224>)PackedReg[9] << 192 |
								 (ap_uint<192>)PackedReg[10] << 160 |
								 (ap_uint<160>)PackedReg[11] << 128 |
								 (ap_uint<128>)PackedReg[12] << 96 |
								 (ap_uint<96>)PackedReg[13] << 64 |
								 ((uint64_t)PackedReg[14]) << 32 |
								 PackedReg[15];

		ap_uint<512> float32_2 = (ap_uint<512>)PackedReg[16] << 480 |
								 (ap_uint<480>)PackedReg[17] << 448 |
								 (ap_uint<448>)PackedReg[18] << 416 |
								 (ap_uint<416>)PackedReg[19] << 384 |
								 (ap_uint<384>)PackedReg[20] << 352 |
								 (ap_uint<352>)PackedReg[21] << 320 |
								 (ap_uint<320>)PackedReg[22] << 288 |
								 (ap_uint<288>)PackedReg[23] << 256 |
								 (ap_uint<256>)PackedReg[24] << 224 |
								 (ap_uint<224>)PackedReg[25] << 192 |
								 (ap_uint<192>)PackedReg[26] << 160 |
								 (ap_uint<160>)PackedReg[27] << 128 |
								 (ap_uint<128>)PackedReg[28] << 96 |
								 (ap_uint<96>)PackedReg[29] << 64 |
								 ((uint64_t)PackedReg[30]) << 32 |
								 PackedReg[31];

		AXI_VALUE_extended tempOut1, tempOut2;
		tempOut1.data = float32_1;
		tempOut1.keep = 8192;
		tempOut1.last = 0;
		StreamOut_Inst1_1.write(tempOut1);
		// to stream gia to deutero instance
		StreamOut_Inst2_1.write(tempOut1);

		tempOut2.data = float32_2;
		tempOut2.keep = 8192;
		tempOut2.last = 0;
		StreamOut_Inst1_2.write(tempOut2);
		// to stream gia to deutero instance
		StreamOut_Inst2_2.write(tempOut2);
	}

/*
 * Zero-padding last 16 to reach 28480 elements to send to the output
 *
 */
l3:
	for (int j = 0; j < 16; j++) {
		Data_tmpIn.a = FifoIn.read();
		PackedReg[j] = Data_tmpIn.b;
	}

l5:
	for (int j = 16; j < 32; j++) {
		PackedReg[j] = 0;
	}
	ap_uint<512> float32_1 =
		(ap_uint<512>)PackedReg[0] << 480 | (ap_uint<480>)PackedReg[1] << 448 |
		(ap_uint<448>)PackedReg[2] << 416 | (ap_uint<416>)PackedReg[3] << 384 |
		(ap_uint<384>)PackedReg[4] << 352 | (ap_uint<352>)PackedReg[5] << 320 |
		(ap_uint<320>)PackedReg[6] << 288 | (ap_uint<288>)PackedReg[7] << 256 |
		(ap_uint<256>)PackedReg[8] << 224 | (ap_uint<224>)PackedReg[9] << 192 |
		(ap_uint<192>)PackedReg[10] << 160 |
		(ap_uint<160>)PackedReg[11] << 128 | (ap_uint<128>)PackedReg[12] << 96 |
		(ap_uint<96>)PackedReg[13] << 64 | ((uint64_t)PackedReg[14]) << 32 |
		PackedReg[15];

	ap_uint<512> float32_2 = (ap_uint<512>)PackedReg[16] << 480 |
							 (ap_uint<480>)PackedReg[17] << 448 |
							 (ap_uint<448>)PackedReg[18] << 416 |
							 (ap_uint<416>)PackedReg[19] << 384 |
							 (ap_uint<384>)PackedReg[20] << 352 |
							 (ap_uint<352>)PackedReg[21] << 320 |
							 (ap_uint<320>)PackedReg[22] << 288 |
							 (ap_uint<288>)PackedReg[23] << 256 |
							 (ap_uint<256>)PackedReg[24] << 224 |
							 (ap_uint<224>)PackedReg[25] << 192 |
							 (ap_uint<192>)PackedReg[26] << 160 |
							 (ap_uint<160>)PackedReg[27] << 128 |
							 (ap_uint<128>)PackedReg[28] << 96 |
							 (ap_uint<96>)PackedReg[29] << 64 |
							 ((uint64_t)PackedReg[30]) << 32 | PackedReg[31];

	AXI_VALUE_extended tempOut1, tempOut2;
	tempOut1.data = float32_1;
	tempOut1.keep = 8192;
	tempOut1.last = 0;
	StreamOut_Inst1_1.write(tempOut1);
	// to stream gia to deutero instance
	StreamOut_Inst2_1.write(tempOut1);

	tempOut2.data = float32_2;
	tempOut2.keep = 8192;
	tempOut2.last = 0;
	StreamOut_Inst1_2.write(tempOut2);
	// to stream gia to deutero instance
	StreamOut_Inst2_2.write(tempOut2);
}

/*
 * Function: ShiftedFifo
 *    This function is fed results from FifoIn and when it reaches first 16*8 it
 *packed this data in 8 Fifo write and send them to the next layer. After this
 *stage it flushes the last 16 packages shift the left and import the new 16
 *results from the previous layer, performing a Shift Register behavior
 *alongside with FIFO.
 *
 * Typename T : Support any type of variable (float in our case)
 * DIM1 : 1800
 * DIM2 : 16
 * DIM3 : 8
 *
 * Inputs :
 *	 	    FifoIn : FIFO 32-bit width that transfers result from previous
 *Convolutional Layers FifoReg : 2 Dimensional Array (16,8) that contains
 *information suitable to send to the next layer and calculate one convolution
 *result
 *
 * Inputs/Outputs :
 *	 	  			FifoOut = FIFO that exports packed 16 result to the next
 *layer performing a shift register behavior
 */
template <typename T, int DIM1, int DIM2, int DIM3>
void ShiftedFifo(hls::stream<float> &FifoIn, hls::stream<AXI_FIFO> &FifoOut,
				 int FifoReg[DIM2][DIM3]) {
	AXI_FIFO temp2;
	union {
		float a;
		uint32_t b;
	} Data_tmpIn;

/*
 * Read from FifoIn and store to the FifoReg first 16*7 results from previous
 * layer
 */
l1:
	for (int i = 0; i < DIM3 - 1; i++) {
#pragma HLS PIPELINE II = 16
	l2:
		for (int j = 0; j < DIM2; j++) {
			Data_tmpIn.a = FifoIn.read();
			FifoReg[j][i] = Data_tmpIn.b;
		}
	}

	/*
	 * Read from FifoIn and store to the FifoReg first 16*7 results from
	 * previous layer
	 *
	 *
	 * header of the shift register. Replace old 16 results with the new.
	 * pos is the position that stream starts to send to the next layer
	 *
	 */

	int header = 0;
l3:
	for (int i = 0; i < DIM1 - 2 * DIM3 + 2; i++) {
#pragma HLS PIPELINE II = 16
		header = (i + DIM3 - 1) % DIM3;

	l4:
		for (int j = 0; j < DIM2; j++) {
			Data_tmpIn.a = FifoIn.read();
			FifoReg[j][header] = Data_tmpIn.b;
		}
	l5:
		for (int p = 0; p < DIM3; p++) {
			int pos = (header + p + 1) % DIM3;
			AXI_FIFO temp3;
			ap_uint<512> float16 = (ap_uint<512>)FifoReg[0][pos] << 480 |
								   (ap_uint<480>)FifoReg[1][pos] << 448 |
								   (ap_uint<448>)FifoReg[2][pos] << 416 |
								   (ap_uint<416>)FifoReg[3][pos] << 384 |
								   (ap_uint<384>)FifoReg[4][pos] << 352 |
								   (ap_uint<352>)FifoReg[5][pos] << 320 |
								   (ap_uint<320>)FifoReg[6][pos] << 288 |
								   (ap_uint<288>)FifoReg[7][pos] << 256 |
								   (ap_uint<256>)FifoReg[8][pos] << 224 |
								   (ap_uint<224>)FifoReg[9][pos] << 192 |
								   (ap_uint<192>)FifoReg[10][pos] << 160 |
								   (ap_uint<160>)FifoReg[11][pos] << 128 |
								   (ap_uint<128>)FifoReg[12][pos] << 96 |
								   (ap_uint<96>)FifoReg[13][pos] << 64 |
								   ((uint64_t)FifoReg[14][pos]) << 32 |
								   FifoReg[15][pos];
			temp3.data = float16;
			FifoOut.write(temp3);
		}
	}
}

/*
 * Function: ParallelTask
 *    This function performs the task level parallelism between the 3
 *Convolution entities through(ShiftedFifo and PackedFifo entities) * Typename T
 *: Support any type of variable (float in our case) DIM1 : 1800 DIM2 : 16 DIM3
 *: 8
 *
 * Inputs :
 *			kernel_0 : 2 Dimension Array (16,8) that contains weights for the
 *1st convolution kernel_1 : 3 Dimension Array (16,16,8) that contains weights
 *for the 2nd convolution kernel_2 : 3 Dimension Array (16,16,8) that contains
 *weights for the 3rd convolution bias_0 : bias for the 1st convolution bias_1 :
 *bias for the 2nd convolution bias_2 : bias for the 3rd convolution
 *			Stream_to_Im: FIFO 32 bit width that transfer the image
 * Inputs/Outputs :
 *		        	StreamOut1 : Streams (512 bit width) that transfers first 16
 *results packed from 3 Convolutional Layers. StreamOut2 : Streams (512 bit
 *width) that transfers second 16 results packed from 3 Convolutional Layers.
 */
template <typename T, int DIM1, int DIM2, int DIM3>
void ParallelTask(hls::stream<float> &Stream_to_Im,
				  AXI_STREAM_ext &StreamOut_Inst1_1,
				  AXI_STREAM_ext &StreamOut_Inst1_2,
				  AXI_STREAM_ext &StreamOut_Inst2_1,
				  AXI_STREAM_ext &StreamOut_Inst2_2, T kernel_0[DIM2][DIM3],
				  T kernel_1[DIM2][DIM2][DIM3], T kernel_2[DIM2][DIM2][DIM3],
				  T bias_0[DIM2], T bias_1[DIM2], T bias_2[DIM2]) {
#pragma HLS DATAFLOW

	union {
		float a;
		uint32_t b;
	} Data_tmpIn;

	// Whether value you put to depth HLS will provide you the best for your
	// FIFO behavior

	// FIFO 32-bit that transfers results from conv1
	static hls::stream<float> Conv1Fifo;
#pragma HLS STREAM variable = Conv1Fifo depth = 3000 dim = 1

	// FIFO 512-bit that transfers packed results from conv3
	static hls::stream<AXI_FIFO> FifoOut;
#pragma HLS STREAM variable = FifoOut depth = 2 * 16 * 8 dim = 1

	// FIFO 32-bit that transfers results from conv2
	static hls::stream<float> Conv2Fifo;
#pragma HLS STREAM variable = Conv2Fifo depth = 3000 dim = 1

	// FIFO 512-bit that transfers packed results from conv3
	static hls::stream<AXI_FIFO> FifoOut2;
#pragma HLS STREAM variable = FifoOut2 depth = 2 * 16 * 8 dim = 1

	// FIFO 32-bit that transfers results from conv3
	static hls::stream<float> Conv3Fifo;
#pragma HLS STREAM variable = Conv3Fifo depth = 3000 dim = 1

	int FifoReg[DIM2][DIM3];
	int FifoReg2[DIM2][DIM3];
	uint32_t PackedReg[32];

	float Reg_Image[DIM3];
	ap_uint<512> Reg_conv[DIM3];
	ap_uint<512> Reg_conv2[DIM3];

	Convolution1<T, DIM1, DIM2, DIM3>(Stream_to_Im, kernel_0, bias_0, Conv1Fifo,
									  Reg_Image);
	ShiftedFifo<T, DIM1, DIM2, DIM3>(Conv1Fifo, FifoOut, FifoReg);
	Convolution2<T, DIM1 - DIM3 + 1, DIM2, DIM3>(FifoOut, kernel_1, Conv2Fifo,
												 bias_1, Reg_conv);
	ShiftedFifo<T, DIM1 - DIM3 + 1, DIM2, DIM3>(Conv2Fifo, FifoOut2, FifoReg2);
	Convolution2<T, DIM1 - 2 * DIM3 + 2, DIM2, DIM3>(
		FifoOut2, kernel_2, Conv3Fifo, bias_2, Reg_conv2);
	PackedFifo<T, DIM1, DIM2, DIM3>(Conv3Fifo, StreamOut_Inst1_1,
									StreamOut_Inst1_2, StreamOut_Inst2_1,
									StreamOut_Inst2_2, PackedReg);
}

/*
 *  Function: Read_Data
 *
 *  This function is performing the task level parallelism between the 3
 *Convolution modules through(ShiftedFifo and PackedFifo). More specifically
 *presents a workaround to achieve sequential and parallel processing using HLS
 *DATAFLOW with MUTEX behavior. When DATAFLOW is enabled the tool tries to
 *perform task level parallelism using every FIFO and Stream that we used. The
 *workaround that we implement is that we force in a specific Stream sequential
 *processing because we want to store different B-RAMS from this using if
 *protection (MUTEX behavior) this stream.
 *
 * Typename T : Support any type of variable (float in our case)
 * DIM1 : 1800
 * DIM2 : 16
 * DIM3 : 8
 *
 * Inputs :
 *			kernel_0 : 2 Dimension Array (16,8) that contains weights for the
 *1st convolution kernel_1 : 3 Dimension Array (16,16,8) that contains weights
 *for the 2nd convolution kernel_2 : 3 Dimension Array (16,16,8) that contains
 *weights for the 3rd convolution bias_0 : bias for the 1st convolution bias_1 :
 *bias for the 2nd convolution bias_2 : bias for the 3rd convolution inStream :
 *Input stream that transfers all the data (image, kernels, bias)
 *
 * Inputs/Outputs :
 *     			    Stream_to_Im: FIFO that transfer the image
 *		        	StreamOut1 : Streams (512 bit width) that transfers first 16
 *results packed from 3 Convolutional Layers. StreamOut2 : Streams (512 bit
 *width) that transfers second 16 results packed from 3 Convolutional Layers.
 */
template <typename T, int DIM1, int DIM2, int DIM3>
void Read_Data(AXI_STREAM &inStream, hls::stream<float> &Stream_to_Im,
			   T kernel_0[DIM2][DIM3], T kernel_1[DIM2][DIM2][DIM3],
			   T kernel_2[DIM2][DIM2][DIM3], T bias_0[DIM2], T bias_1[DIM2],
			   T bias_2[DIM2]) {
	union {
		float a;
		uint32_t b;
	} Data_tmpIn;
	AXI_VALUE AXI_tmpIn;

	const int Data_Length = Num_K0 + Num_K1 * 2 + Num_B * 3 +
							Num_Im;  // Length of the in Stream data

	/*
	 * Creating on Task (for loop) and with MUTEX protection (if) we read from
	 * the streams and store the data to the suitable B-RAM
	 */
	for (int i = 0; i < Data_Length; i++) {
#pragma HLS PIPELINE II = 1
		AXI_tmpIn = inStream.read();
		Data_tmpIn.b = AXI_tmpIn.data;
		if (i < Num_K0) {
			kernel_0[i % DIM2][i / DIM2] = Data_tmpIn.a;
		} else if (i < Num_K0 + Num_K1) {
			int k1 = i - Num_K0;
			kernel_1[k1 % DIM2][(k1 / DIM2) % DIM2][k1 / (DIM2 * DIM2)] =
				Data_tmpIn.a;
		} else if (i < Num_K0 + Num_K1 * 2) {
			int k2 = i - (Num_K0 + Num_K1);
			kernel_2[k2 % DIM2][(k2 / DIM2) % DIM2][k2 / (DIM2 * DIM2)] =
				Data_tmpIn.a;
		} else if (i < Num_K0 + Num_K1 * 2 + Num_B) {
			int b0 = i - (Num_K0 + Num_K1 * 2);
			bias_0[b0 % DIM2] = Data_tmpIn.a;
		} else if (i < Num_K0 + Num_K1 * 2 + Num_B * 2) {
			int b1 = i - (Num_K0 + Num_K1 * 2 + Num_B);
			bias_1[b1 % DIM2] = Data_tmpIn.a;
		} else if (i < Num_K0 + Num_K1 * 2 + Num_B * 3) {
			int b2 = i - (Num_K0 + Num_K1 * 2 + Num_B * 2);
			bias_2[b2 % DIM2] = Data_tmpIn.a;
		} else
			Stream_to_Im.write(Data_tmpIn.a);
	}
}

/*
 *  Function: wrapped_Conv_hw
 *	This function enables every module of the accelerator.
 *
 * Typename T : Support any type of variable (float in our case)
 * DIM1 : 1800
 * DIM2 : 16
 * DIM3 : 8
 *
 * Inputs :
 *			inStream : Input stream that transfers
 *
 * Inputs/Outputs :
 *		        	StreamOut1 : Streams (512 bit width) that transfers first 16
 *results packed from 3 Convolutional Layers. StreamOut2 : Streams (512 bit
 *width) that transfers second 16 results packed from 3 Convolutional Layers.
 */
template <typename T, int DIM1, int DIM2, int DIM3>
void wrapped_Conv_hw(AXI_STREAM &inStream, AXI_STREAM_ext &StreamOut_Inst1_1,
					 AXI_STREAM_ext &StreamOut_Inst1_2,
					 AXI_STREAM_ext &StreamOut_Inst2_1,
					 AXI_STREAM_ext &StreamOut_Inst2_2) {
	static hls::stream<float> Stream_to_Im;
#pragma HLS STREAM variable = Stream_to_Im depth = 3000 dim = 1

#pragma HLS INLINE

	// Bind B-RAM for kernel and bias for every Convolutional Layer
	T kernel_0[DIM2][DIM3];
	T bias_0[DIM2];
	T kernel_1[DIM2][DIM2][DIM3];
	T bias_1[DIM2];
	T kernel_2[DIM2][DIM2][DIM3];
	T bias_2[DIM2];

	// Read the Data from stream and store them to B-RAM
	Read_Data<T, DIM1, DIM2, DIM3>(inStream, Stream_to_Im, kernel_0, kernel_1,
								   kernel_2, bias_0, bias_1, bias_2);
	ParallelTask<T, DIM1, DIM2, DIM3>(Stream_to_Im, StreamOut_Inst1_1,
									  StreamOut_Inst1_2, StreamOut_Inst2_1,
									  StreamOut_Inst2_2, kernel_0, kernel_1,
									  kernel_2, bias_0, bias_1, bias_2);
}
