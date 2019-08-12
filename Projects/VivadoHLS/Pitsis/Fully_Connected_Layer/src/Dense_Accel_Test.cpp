/*
******************************************************************************
  Vivado HLS (R) is a trademark of The Xilinx (R) Corporation

  Filename:    Conv_Accel_Test.h
  Author:      Pitsis Giorgos
  Version:     1.0
  Date:        September 29, 2018
  Copyright:   (c) 2018 by Pitsis Giorgos, All Rights Reserved
******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <stdint.h>
#include "Dense.h"
#include "Dense_def.h"

typedef float T;
int const DIM1 = 28480;
int const DIM2 = 400;


void FC_Accel (AXI_STREAM1 &inStream, AXI_STREAM_ext &Conv_Stream1, AXI_STREAM_ext &Conv_Stream2, AXI_STREAM2 &outStream);

/*
 *  Function: TestRead
 *
 *  This function reads from a specific file data.
 *  Inputs :
 *			file : Path of file to read
 *			size : Size of 32-bit data we'll read
 *  Inputs/Outputs :
 *		        	 data : The data read from the file
 */
template <typename T>
T* TestRead(const char *file,int size,int bytes_read){
	FILE *input_file;
	T *data=(T *)malloc(size*(sizeof(T)));

	//T * data =new T[size];
	input_file = fopen(file,"rb");
	fread(data,size,bytes_read,input_file);
	fclose(input_file);

	return data;
}


 int main(void)
{
	union {float a; uint32_t b;}  Data_tmp_change1,Data_tmp_change2,Data_tmp_change3,Data_tmp_change4;

	AXI_VALUE1 tempIn;
	AXI_VALUE2 tempOut;
	AXI_VALUE_extended tempInConv1,tempInConv2;

	T input[DIM1];
 	uint32_t input_uint[DIM1];
 	T dense_bias[DIM2];
	T *dense_codebook = (T *)malloc(sizeof(T)*16);
 	uint32_t *dense_data=(uint32_t *)malloc(DIM1*DIM2/2);
	T Classes_hw[DIM2];

 	/*
 	 * Paths for conv results, dense codebook, indexes and bias
 	 *
 	 */	
	const char *finput , *fdense_codebook,*fdense_data, *fdense_bias;

	/*
	 * 	Convolutional Layers results
	 *	Dense Codebook	
	 *	Dense Indexes
	 *  Dense bias
	 */

	finput = "Input_Data_Files/Convolutional_Layers_Output.txt";
	float *tinput = TestRead<float>(finput ,DIM1,4);
	printf("Output of three Convolutional Layers read sucessfully \r\n");

	fdense_codebook = "Input_Data_Files/Dense_Codebook.txt";
	float *tdense_codebook= TestRead<float>(fdense_codebook,16,4);
	printf("Dense Codebook read sucessfully \r\n");

	fdense_data = "Input_Data_Files/Dense_Data_MLT400.txt";
	uint32_t *tdense_data= TestRead<uint32_t>(fdense_data,DIM1*DIM2/8,4);
	printf("Dense data read sucessfully \r\n");

	fdense_bias = "Input_Data_Files/dense_bias.txt";
	float *tdense_bias= TestRead<float>(fdense_bias,DIM2,4);
	printf("Dense bias read sucessfully \r\n");

	for(int i = 0; i<DIM1; i++){
		input[i]=(float)tinput[i];
		Data_tmp_change1.a=input[i];
		input_uint[i]=Data_tmp_change1.b;
	}

	for(int i = 0; i<16; i++){
		dense_codebook[i]=(float)tdense_codebook[i];
	}

	for(int i = 0; i<DIM1*DIM2/8; i++){
		dense_data[i]=tdense_data[i];
	}


	for(int i = 0; i<DIM2; i++){
		dense_bias[i]=(float)tdense_bias[i];
	}

	/*
	 * Conv_Stream1,Conv_Stream2 transfers results of Convolutional Layers
	 */
	AXI_STREAM1 inp_stream;
	AXI_STREAM2 out_stream;
	AXI_STREAM_ext  Conv_Stream1,Conv_Stream2;

	/*
	 * Send all kernels data through ConvStream1 and ConvStream2
	 */
	for(int i=0; i<DIM1; i+=32)
	{
		ap_uint<512> float32_1 = (ap_uint<512>)input_uint[i]<<480 |(ap_uint<480>)input_uint[i+1]<<448 |(ap_uint<448>)input_uint[i+2]<<416 |(ap_uint<416>)input_uint[i+3]<<384
								|(ap_uint<384>)input_uint[i+4]<<352 |(ap_uint<352>)input_uint[i+5]<<320 |(ap_uint<320>)input_uint[i+6]<<288 |(ap_uint<288>)input_uint[i+7]<<256
								|(ap_uint<256>)input_uint[i+8]<<224 |(ap_uint<224>)input_uint[i+9]<<192 |(ap_uint<192>)input_uint[i+10]<<160|(ap_uint<160>)input_uint[i+11]<<128
								|(ap_uint<128>)input_uint[i+12]<<96 | (ap_uint<96>)input_uint[i+13]<<64 |((uint64_t)input_uint[i+14]) << 32 | input_uint[i+15] ;

		ap_uint<512> float32_2 = (ap_uint<512>)input_uint[i+16]<<480 |(ap_uint<480>)input_uint[i+17]<<448 |(ap_uint<448>)input_uint[i+18]<<416 |(ap_uint<416>)input_uint[i+19]<<384
								|(ap_uint<384>)input_uint[i+20]<<352 |(ap_uint<352>)input_uint[i+21]<<320 |(ap_uint<320>)input_uint[i+22]<<288 |(ap_uint<288>)input_uint[i+23]<<256
								|(ap_uint<256>)input_uint[i+24]<<224 |(ap_uint<224>)input_uint[i+25]<<192 |(ap_uint<192>)input_uint[i+26]<<160 |(ap_uint<160>)input_uint[i+27]<<128
								|(ap_uint<128>)input_uint[i+28]<<96  | (ap_uint<96>)input_uint[i+29]<<64  |((uint64_t)input_uint[i+30]) << 32  | input_uint[i+31] ;

		tempInConv1.data = float32_1;
  		tempInConv1.keep=131071;
  		tempInConv1.last =0;
		Conv_Stream1.write(tempInConv1);

		tempInConv2.data = float32_2;
  		tempInConv2.keep=131071;
  		tempInConv2.last =0;
		Conv_Stream2.write(tempInConv2);
	}


	/*
	 * Send all dense bias through ConvStream1 and ConvStream2
	 */
	for(int i=0; i<DIM2; i+=4)
	{
 		Data_tmp_change1.a=dense_bias[i+3];
 		Data_tmp_change2.a=dense_bias[i+2];
		Data_tmp_change3.a=dense_bias[i+1];
 		Data_tmp_change4.a=dense_bias[i];

 		ap_uint<128> float4 =(ap_uint<128>)Data_tmp_change1.b<<96|(ap_uint<96>)Data_tmp_change2.b<<64|((uint64_t)Data_tmp_change3.b) << 32 | Data_tmp_change4.b ;
		tempIn.data = float4;
		tempIn.keep=131071;
		tempIn.last =0;
		inp_stream.write(tempIn);
	}

	/*
	 * Send all dense codebook through ConvStream1 and ConvStream2
	 */
	for(int i=0; i<16; i+=4){
		Data_tmp_change1.a=dense_codebook[i+3];
		Data_tmp_change2.a=dense_codebook[i+2];
		Data_tmp_change3.a=dense_codebook[i+1];
		Data_tmp_change4.a=dense_codebook[i];
 		ap_uint<128> float4 =(ap_uint<128>)Data_tmp_change1.b<<96|(ap_uint<96>)Data_tmp_change2.b<<64|((uint64_t)Data_tmp_change3.b) << 32 | Data_tmp_change4.b ;

 		tempIn.data = float4;
 		tempIn.keep=131071;
 		tempIn.last = 0;
 		inp_stream.write(tempIn);
	}

	/*
	 * Send all dense data through ConvStream1 and ConvStream2
	 */
	for(int i=0; i<DIM1*DIM2/8; i+=4)
	{
   		ap_uint<128> float4 =(ap_uint<128>)dense_data[i+3]<<96|(ap_uint<96>)dense_data[i+2]<<64|((uint64_t)dense_data[i+1]) << 32 | dense_data[i] ;
		tempIn.data = float4;
		tempIn.keep=131071;
		tempIn.last = (i==DIM1*DIM2/8-4)? 1 : 0;
		inp_stream.write(tempIn);
	}

	/*
	 * Call the Accelerator
	 */
	FC_Accel (inp_stream,Conv_Stream1,Conv_Stream2, out_stream);

	/*
	 * Print results from the Accelerator
	 */
	for(int i=0; i<DIM2; i++)
	{
		tempOut=out_stream.read();
		Data_tmp_change1.b= tempOut.data;
		Classes_hw[i]=Data_tmp_change1.a;
		printf("Classes[%d] = %f\n" ,i,Classes_hw[i]);
	}

	return 0;
}

