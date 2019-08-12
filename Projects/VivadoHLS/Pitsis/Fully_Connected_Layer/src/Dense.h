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
#include <assert.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <stdint.h>
#include <stdio.h>
#include "Dense_def.h"

/*
  Struct for the input stream
*/
struct AXI_VALUE1{
  ap_uint<128> data;
  ap_uint<1> last;
  ap_uint<16> keep;
};

/*
  Struct for the input stream (output of 3 Convolutional Layers)
*/
struct AXI_VALUE_extended{
  ap_uint<512> data;
  ap_uint<1> last;
  ap_uint<64> keep;
};

/*
  Struct for the output stream
*/
struct AXI_VALUE2{
  ap_uint<32> data;
  ap_uint<1> last;
  ap_uint<4> keep;
};
typedef hls::stream<AXI_VALUE1> AXI_STREAM1;
typedef hls::stream<AXI_VALUE2> AXI_STREAM2;
typedef hls::stream<AXI_VALUE_extended> AXI_STREAM_ext;

void FC_Accel (AXI_STREAM1 &inStream, AXI_STREAM_ext &Conv_Stream1, AXI_STREAM_ext &Conv_Stream2, AXI_STREAM2 &outStream);

/*
 *  Function: Dense_hw
 *	This function performs a Convolutional Layer 2-D with 16 different kernel size of 16*8 each.
 *	The convolution operation is valid (non-zero padding)
 *
 * Typename T : Support any type of variable (float in our case)
 * DIM1 : 28480
 * DIM2 : 400
 *
 * Inputs :
 *	 	     dense_bias : bias for every Class (400)
 *			 inStream : Input stream that transfers all the dense data (codebook, indexes, bias)
 *			 Conv_Stream1 : Input Stream (FIFO 512 bit width) that transfers first 16 results packed from 3 Convolutional Layers.
 *			 Conv_Stream1 : Input Stream (FIFO 512 bit width) that transfers second 16 results packed from 3 Convolutional Layers.
 *
 * Inputs/Outputs :
 *	 	     		 Classes : 400 Classes of the dense
 *
 */
template <typename T, int DIM1,int DIM2>
void Dense_hw(AXI_STREAM_ext &Conv_Stream1, AXI_STREAM_ext &Conv_Stream2,
			  T dense_bias[DIM2],AXI_STREAM1 &inStream,T Classes[DIM2])
{
	union {float a; uint32_t b;} Data_tmpIn,Data_tmpIn1,Data_tmpIn2,Data_tmpIn3,Data_tmpIn4;
 	#pragma HLS INLINE
	static int FADD_LAT=10;
 	T sumReg[800];

	uint32_t a,b,c,d;
	ap_uint<128> temp1,temp2,temp3;
  	AXI_VALUE1 AXI_tmpIn;
 	AXI_VALUE_extended AXI_Conv1,AXI_Conv2;

 	/*
 	 * Codebook with 16 centroids (32-bit float) creating 16 instances
 	 */
 	T CodeBook1[16],CodeBook2[16],CodeBook3[16],CodeBook4[16],CodeBook5[16],CodeBook6[16],CodeBook7[16],CodeBook8[16],CodeBook9[16];
    T CodeBook10[16],CodeBook11[16],CodeBook12[16],CodeBook13[16],CodeBook14[16],CodeBook15[16],CodeBook16[16];

 	/*
 	 * data -> 4-bit indexes (read from the input stream) that targets to the codebook
 	 */
    uint32_t data0,data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15;
 	uint32_t data16,data17,data18,data19,data20,data21,data22,data23,data24,data25,data26,data27,data28,data29,data30,data31;

 	/*
 	 * conv_input -> 32 results from the three Convolutional Layers (read from the stream (512 bit)
 	 */
 	T conv_input0,conv_input1,conv_input2,conv_input3,conv_input4,conv_input5,conv_input6,conv_input7,conv_input8,conv_input9,conv_input10,conv_input11,conv_input12,conv_input13,conv_input14,conv_input15;
 	T conv_input16,conv_input17,conv_input18,conv_input19,conv_input20,conv_input21,conv_input22,conv_input23,conv_input24,conv_input25,conv_input26,conv_input27,conv_input28,conv_input29,conv_input30,conv_input31;

 	/*
 	 * Read from the input stream and store to the Codebook and instatiate 16 times
 	 */
	Dense_hw_L1: for (int i=0; i<16; i+=4){
		#pragma HLS PIPELINE II=2

		AXI_tmpIn= inStream.read();
		temp1=AXI_tmpIn.data;

		a = (uint32_t)((temp1)  & 0xFFFFFFFF);
		b = (uint32_t)((temp1>>32)  & 0xFFFFFFFF);
		c = (uint32_t)((temp1>>64)  & 0xFFFFFFFF);
		d = (uint32_t)(temp1>>96 &  0xFFFFFFFF);

		Data_tmpIn1.b = a;
		CodeBook1[i]= Data_tmpIn1.a;
		CodeBook2[i]= Data_tmpIn1.a;
		CodeBook3[i]= Data_tmpIn1.a;
		CodeBook4[i]= Data_tmpIn1.a;
		CodeBook5[i]= Data_tmpIn1.a;
		CodeBook6[i]= Data_tmpIn1.a;
		CodeBook7[i]= Data_tmpIn1.a;
		CodeBook8[i]= Data_tmpIn1.a;
		CodeBook9[i]= Data_tmpIn1.a;
		CodeBook10[i]= Data_tmpIn1.a;
		CodeBook11[i]= Data_tmpIn1.a;
		CodeBook12[i]= Data_tmpIn1.a;
		CodeBook13[i]= Data_tmpIn1.a;
		CodeBook14[i]= Data_tmpIn1.a;
		CodeBook15[i]= Data_tmpIn1.a;
		CodeBook16[i]= Data_tmpIn1.a;


		Data_tmpIn2.b = b;
		CodeBook1[i+1]= Data_tmpIn2.a;
		CodeBook2[i+1]= Data_tmpIn2.a;
		CodeBook3[i+1]= Data_tmpIn2.a;
		CodeBook4[i+1]= Data_tmpIn2.a;
		CodeBook5[i+1]= Data_tmpIn2.a;
		CodeBook6[i+1]= Data_tmpIn2.a;
		CodeBook7[i+1]= Data_tmpIn2.a;
		CodeBook8[i+1]= Data_tmpIn2.a;
		CodeBook9[i+1]= Data_tmpIn2.a;
		CodeBook10[i+1]= Data_tmpIn2.a;
		CodeBook11[i+1]= Data_tmpIn2.a;
		CodeBook12[i+1]= Data_tmpIn2.a;
		CodeBook13[i+1]= Data_tmpIn2.a;
		CodeBook14[i+1]= Data_tmpIn2.a;
		CodeBook15[i+1]= Data_tmpIn2.a;
		CodeBook16[i+1]= Data_tmpIn2.a;

		Data_tmpIn3.b = c;
		CodeBook1[i+2]= Data_tmpIn3.a;
		CodeBook2[i+2]= Data_tmpIn3.a;
		CodeBook3[i+2]= Data_tmpIn3.a;
		CodeBook4[i+2]= Data_tmpIn3.a;
		CodeBook5[i+2]= Data_tmpIn3.a;
		CodeBook6[i+2]= Data_tmpIn3.a;
		CodeBook7[i+2]= Data_tmpIn3.a;
		CodeBook8[i+2]= Data_tmpIn3.a;
		CodeBook9[i+2]= Data_tmpIn3.a;
		CodeBook10[i+2]= Data_tmpIn3.a;
		CodeBook11[i+2]= Data_tmpIn3.a;
		CodeBook12[i+2]= Data_tmpIn3.a;
		CodeBook13[i+2]= Data_tmpIn3.a;
		CodeBook14[i+2]= Data_tmpIn3.a;
		CodeBook15[i+2]= Data_tmpIn3.a;
		CodeBook16[i+2]= Data_tmpIn3.a;

		Data_tmpIn4.b = d;
		CodeBook1[i+3]= Data_tmpIn4.a;
		CodeBook2[i+3]= Data_tmpIn4.a;
		CodeBook3[i+3]= Data_tmpIn4.a;
		CodeBook4[i+3]= Data_tmpIn4.a;
		CodeBook5[i+3]= Data_tmpIn4.a;
		CodeBook6[i+3]= Data_tmpIn4.a;
		CodeBook7[i+3]= Data_tmpIn4.a;
		CodeBook8[i+3]= Data_tmpIn4.a;
		CodeBook9[i+3]= Data_tmpIn4.a;
		CodeBook10[i+3]= Data_tmpIn4.a;
		CodeBook11[i+3]= Data_tmpIn4.a;
		CodeBook12[i+3]= Data_tmpIn4.a;
		CodeBook13[i+3]= Data_tmpIn4.a;
		CodeBook14[i+3]= Data_tmpIn4.a;
		CodeBook15[i+3]= Data_tmpIn4.a;
		CodeBook16[i+3]= Data_tmpIn4.a;

	}

 	/*
 	 * Store to the Classes dense bias
 	 */
  	Dense_hw_L2:for (int k=0; k<DIM2; k+=2)
	{
		#pragma HLS PIPELINE II=1
 		Classes[k]=dense_bias[k];
 		Classes[k+1]=dense_bias[k+1];
	}

  	/*
  	 * Stream Bus=128 bit
  	 * In every stream read we send the indexes for the codebook
  	 * Weight footprint -> 4 bit
  	 * Stream read -> 128/4 = 32 weights
  	 */
  	Dense_hw_L3:for (int i=0; i<DIM1/32; i++)
	{
		AXI_Conv1=Conv_Stream1.read();
		ap_uint<512> Conv_input1=AXI_Conv1.data;

		AXI_Conv2=Conv_Stream2.read();
		ap_uint<512> Conv_input2=AXI_Conv2.data;

		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift0))& 0xFFFFFFFF);
		conv_input0=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift1))& 0xFFFFFFFF);
		conv_input1=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift2))& 0xFFFFFFFF);
		conv_input2=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift3))& 0xFFFFFFFF);
		conv_input3=Data_tmpIn.a;

		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift4 ))& 0xFFFFFFFF);
		conv_input4=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift5)) & 0xFFFFFFFF);
		conv_input5=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift6)) & 0xFFFFFFFF);
		conv_input6=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift7)) & 0xFFFFFFFF);
		conv_input7=Data_tmpIn.a;

		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift8)) & 0xFFFFFFFF);
		conv_input8=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift9)) & 0xFFFFFFFF);
		conv_input9=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift10))& 0xFFFFFFFF);
		conv_input10=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift11))& 0xFFFFFFFF);
		conv_input11=Data_tmpIn.a;

		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift12))& 0xFFFFFFFF);
		conv_input12=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift13))& 0xFFFFFFFF);
		conv_input13=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift14))& 0xFFFFFFFF);
		conv_input14=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input1>>(conv_shift15))& 0xFFFFFFFF);
		conv_input15=Data_tmpIn.a;

		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift0))& 0xFFFFFFFF);
		conv_input16=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift1))& 0xFFFFFFFF);
		conv_input17=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift2))& 0xFFFFFFFF);
		conv_input18=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift3))& 0xFFFFFFFF);
		conv_input19=Data_tmpIn.a;

		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift4))& 0xFFFFFFFF);
		conv_input20=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift5))& 0xFFFFFFFF);
		conv_input21=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift6))& 0xFFFFFFFF);
		conv_input22=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift7))& 0xFFFFFFFF);
		conv_input23=Data_tmpIn.a;

		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift8)) & 0xFFFFFFFF);
		conv_input24=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift9)) & 0xFFFFFFFF);
		conv_input25=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift10))& 0xFFFFFFFF);
		conv_input26=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift11))& 0xFFFFFFFF);
		conv_input27=Data_tmpIn.a;

		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift12))& 0xFFFFFFFF);
		conv_input28=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift13))& 0xFFFFFFFF);
		conv_input29=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift14))& 0xFFFFFFFF);
		conv_input30=Data_tmpIn.a;
		Data_tmpIn.b = (ap_uint<32>)((Conv_input2>>(conv_shift15))& 0xFFFFFFFF);
		conv_input31=Data_tmpIn.a;
	/*
	 * Accumulator in MAC operation delay is 10 cycles (This could be change depending on the target FPGA)
	 * Performing MAC operation would lead to an overall Pipeline = 10
	 * We perform a Memory Layout Transformation to the dense weighs. (This could lead to an overall Pipeline 1)
	 * From 28480x800 to (32x400)x890 (890-> 28480/32)
	 * Stream starts sending first 32 weights-block for every class (400) continuing to the rest (889x400) of the 32 block-weights
	 *
	 * More specifically in 10 cycles we read 10 times from the stream and perform 10*32=320 MAC operation in parallel
	 * To create a pipeline from the previous 3 Convolutional Layers we calculate partial results for every class (400).
	 */
	for (int k=0; k<DIM2; k+=FADD_LAT)
	{
		#pragma HLS PIPELINE II=10
		Dense_hw_L4:for (int j=0; j<FADD_LAT; j++)
			{
				AXI_tmpIn= inStream.read();
				temp1=AXI_tmpIn.data;

				/*
				 * Read from the Input stream the indexes of 32 weights
				 */
				data0 = (ap_uint<4>)((temp1>>(shift0)) & 0x1F);
				data1 = (ap_uint<4>)((temp1>>(shift1)) & 0x1F);
				data2 = (ap_uint<4>)((temp1>>(shift2)) & 0x1F);
				data3 = (ap_uint<4>)((temp1>>(shift3)) & 0x1F);

				data4 = (ap_uint<4>)((temp1>>(shift4 ))& 0x1F);
				data5 = (ap_uint<4>)((temp1>>(shift5)) & 0x1F);
				data6 = (ap_uint<4>)((temp1>>(shift6)) & 0x1F);
				data7 = (ap_uint<4>)((temp1>>(shift7)) & 0x1F);

				data8 = (ap_uint<4>)((temp1>>(shift8))  & 0x1F);
				data9 = (ap_uint<4>)((temp1>>(shift9))  & 0x1F);
				data10 = (ap_uint<4>)((temp1>>(shift10))& 0x1F);
				data11 = (ap_uint<4>)((temp1>>(shift11))& 0x1F);

				data12 = (ap_uint<4>)((temp1>>(shift12))& 0x1F);
				data13 = (ap_uint<4>)((temp1>>(shift13))& 0x1F);
				data14 = (ap_uint<4>)((temp1>>(shift14))& 0x1F);
				data15 = (ap_uint<4>)((temp1>>(shift15))& 0x1F);

				data16 = (ap_uint<4>)((temp1>>(shift16))& 0x1F);
				data17 = (ap_uint<4>)((temp1>>(shift17))& 0x1F);
				data18 = (ap_uint<4>)((temp1>>(shift18))& 0x1F);
				data19 = (ap_uint<4>)((temp1>>(shift19))& 0x1F);

				data20 = (ap_uint<4>)((temp1>>(shift20))& 0x1F);
				data21 = (ap_uint<4>)((temp1>>(shift21))& 0x1F);
				data22 = (ap_uint<4>)((temp1>>(shift22))& 0x1F);
				data23 = (ap_uint<4>)((temp1>>(shift23))& 0x1F);

				data24 = (ap_uint<4>)((temp1>>(shift24))& 0x1F);
				data25 = (ap_uint<4>)((temp1>>(shift25))& 0x1F);
				data26 = (ap_uint<4>)((temp1>>(shift26))& 0x1F);
				data27 = (ap_uint<4>)((temp1>>(shift27))& 0x1F);

				data28 = (ap_uint<4>)((temp1>>(shift28))& 0x1F);
				data29 = (ap_uint<4>)((temp1>>(shift29))& 0x1F);
				data30 = (ap_uint<4>)((temp1>>(shift30))& 0x1F);
				data31 = (ap_uint<4>)((temp1>>(shift31))& 0x1F);

				/*
				 * Calculate inner products for the 32 weights-Block
				 * Calculate partial result for every Class
				 */
				T inner_prod1 = CodeBook1[data0]*conv_input0;
				T inner_prod2 = CodeBook2[data1]*conv_input1;
				T inner_prod3 = CodeBook3[data2]*conv_input2;
				T inner_prod4 = CodeBook4[data3]*conv_input3;

				T inner_prod5 = CodeBook5[data4]*conv_input4;
				T inner_prod6 = CodeBook6[data5]*conv_input5;
				T inner_prod7 = CodeBook7[data6]*conv_input6;
				T inner_prod8 = CodeBook8[data7]*conv_input7;

				T inner_prod9 = CodeBook9[data8]*conv_input8;
				T inner_prod10 = CodeBook10[data9]*conv_input9;
				T inner_prod11 = CodeBook11[data10]*conv_input10;
				T inner_prod12 = CodeBook12[data11]*conv_input11;

				T inner_prod13 = CodeBook13[data12]*conv_input12;
				T inner_prod14 = CodeBook14[data13]*conv_input13;
				T inner_prod15 = CodeBook15[data14]*conv_input14;
				T inner_prod16 = CodeBook16[data15]*conv_input15;

				T sum1_1=inner_prod1+inner_prod2+inner_prod3+inner_prod4;
				T sum1_2=inner_prod5+inner_prod6+inner_prod7+inner_prod8;
				T sum1_3=inner_prod9+inner_prod10+inner_prod11+inner_prod12;
				T sum1_4=inner_prod13+inner_prod14+inner_prod15+inner_prod16;
				T sum1=sum1_1+sum1_2+sum1_3+sum1_4;

				T inner_prod17 = CodeBook1[data16]*conv_input16;
				T inner_prod18 = CodeBook2[data17]*conv_input17;
				T inner_prod19 = CodeBook3[data18]*conv_input18;
				T inner_prod20 = CodeBook4[data19]*conv_input19;

				T inner_prod21 = CodeBook5[data20]*conv_input20;
				T inner_prod22 = CodeBook6[data21]*conv_input21;
				T inner_prod23 = CodeBook7[data22]*conv_input22;
				T inner_prod24 = CodeBook8[data23]*conv_input23;

				T inner_prod25 = CodeBook9[data24]*conv_input24;
				T inner_prod26 = CodeBook10[data25]*conv_input25;
				T inner_prod27 = CodeBook11[data26]*conv_input26;
				T inner_prod28 = CodeBook12[data27]*conv_input27;

				T inner_prod29 = CodeBook13[data28]*conv_input28;
				T inner_prod30 = CodeBook14[data29]*conv_input29;
				T inner_prod31 = CodeBook15[data30]*conv_input30;
				T inner_prod32 = CodeBook16[data31]*conv_input31;

				T sum2_1=inner_prod17+inner_prod18+inner_prod19+inner_prod20;
				T sum2_2=inner_prod21+inner_prod22+inner_prod23+inner_prod24;
				T sum2_3=inner_prod25+inner_prod26+inner_prod27+inner_prod28;
				T sum2_4=inner_prod29+inner_prod30+inner_prod31+inner_prod32;
				T sum2=sum2_1+sum2_2+sum2_3+sum2_4;

				/*
				 * Store partial result in the Classes
				 */
				Classes[j+k]+=sum1+sum2;
			}
		}
	}

	return;
}

/*
 *  Function: wrapped_Dense_hw
 *
 *  This the main function of the accelerator that performs I/O communication and enables our wrapper.
 *  Inputs :
 *			 inStream : Input stream that transfers all the dense data (codebook, indexes, bias)
 *			 Conv_Stream1 : Input Stream (FIFO 512 bit width) that transfers first 16 results packed from 3 Convolutional Layers.
 *			 Conv_Stream1 : Input Stream (FIFO 512 bit width) that transfers second 16 results packed from 3 Convolutional Layers.
 *  Inputs/Outputs :
 *		        	 outStream : Output stream (32 bit width) that transfers results 400 Classes.
 */
template <typename T, int DIM1,int DIM2>
void wrapped_Dense_hw(
		AXI_STREAM1 &inStream,AXI_STREAM_ext &Conv_Stream1, AXI_STREAM_ext &Conv_Stream2, AXI_STREAM2 &outStream)
{

	union {float a; uint32_t b;} Data_tmpIn ;
	union {float a; uint32_t b;}Data_tmpOut;
	AXI_VALUE1 AXI_tmpIn ;
	AXI_VALUE2 AXI_tmpOut;
	#pragma HLS INLINE

	T Classes[DIM2];
	T dense_bias[DIM2];
	ap_uint<128> temp;
	uint32_t a,b,c,d;

	/*
	 * Read dense bias from the input stream
	 */
	wrapped_L2:for(int i=0; i<DIM2; i+=4)
	{
		#pragma HLS PIPELINE II=2
		AXI_tmpIn= inStream.read();
		temp=AXI_tmpIn.data;

		a = (uint32_t)((temp)  & 0xFFFFFFFF);
		b = (uint32_t)((temp>>32)  & 0xFFFFFFFF);
		c = (uint32_t)((temp>>64)  & 0xFFFFFFFF);
		d = (uint32_t)(temp>>96 &  0xFFFFFFFF);

		Data_tmpIn.b = a;
		dense_bias[i]= Data_tmpIn.a;

		Data_tmpIn.b = b;
		dense_bias[i+1]= Data_tmpIn.a;

		Data_tmpIn.b = c;
		dense_bias[i+2]= Data_tmpIn.a;

		Data_tmpIn.b = d;
		dense_bias[i+3]= Data_tmpIn.a;
	}

	Dense_hw<T, DIM1,DIM2>(Conv_Stream1, Conv_Stream2,dense_bias,inStream,Classes);
	/*
	 * Write results of 400 Classes to the output stream
	 */
	wrapped_L3:for (int i=0; i<DIM2; i++)
	{
	#pragma HLS PIPELINE II=1
		Data_tmpOut.a = Classes[i];
		AXI_tmpOut.data = Data_tmpOut.b;
		AXI_tmpOut.keep=31;
		AXI_tmpOut.last = (i==DIM2-1)? 1 : 0;
		outStream.write(AXI_tmpOut);
 	}
}
