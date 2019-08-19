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
#include "conv.h"

typedef float T;
int const DIM1 = 1800;
int const DIM2 = 16;
int const DIM3 = 8;


void Conv_Accel (AXI_STREAM &inStream, AXI_STREAM_ext &StreamOut_Inst1_1, AXI_STREAM_ext &StreamOut_Inst1_2, AXI_STREAM_ext &StreamOut_Inst2_1, AXI_STREAM_ext &StreamOut_Inst2_2);


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
float* TestRead(const char *file,int size){
	FILE *input_file;
	float * data =new float[size];
	input_file = fopen(file,"rb");
	fread(data,size,4,input_file);
	fclose(input_file);

	return data;
}


 int main(void)
{
	union {float a; uint32_t b;}  Data_tmp_change,Data_tmp_change2;

	AXI_VALUE tempIn;
	AXI_VALUE_extended tempOut,tempOut2;

	T image[DIM1];
	T kernel_0[DIM2*DIM3];
	T kernel_1[DIM2*DIM2*DIM3];
	T kernel_2[DIM2*DIM2*DIM3];
	T bias_0[DIM2];
	T bias_1[DIM2];
	T bias_2[DIM2];
	T conv_hw[(DIM1-3*DIM3+3)*DIM2];

 	/*
 	 * Paths for image kernels and bias
 	 *
 	 */	
 	const char *fimage , *fkernel_0, *fbias_0;

	/*
	 *  Image
	 *  kernel 0
	 *  bias 0
	 */

	fimage = "Input_Data_Files/fimage.txt";
	float *timage= TestRead(fimage,DIM1);

	fkernel_0 = "Input_Data_Files/kernel_0.txt";
	float *tkernel_0= TestRead(fkernel_0,DIM2*DIM3);

	fbias_0 = "Input_Data_Files/bias_0.txt";
	float *tbias_0= TestRead(fbias_0,DIM2);


	for(int i = 0; i<DIM1; i++)
		image[i]=(float)timage[i];

	for(int i = 0; i<DIM2*DIM3; i++)
		kernel_0[i]=(float)tkernel_0[i];


	for(int i = 0; i<DIM2; i++)
		bias_0[i]=(float)tbias_0[i];

	printf("Image, kernel 0 and bias 0 read sucessfully \r\n");

	const char *fkernel_1, *fbias_1;
	/*
 	 *  kernel 1
	 *  bias 1
	 */
	fkernel_1 = "Input_Data_Files/kernel_1.txt";
	float *tkernel_1= TestRead(fkernel_1,DIM2*DIM2*DIM3);

	fbias_1 = "Input_Data_Files/bias_1.txt";
	float *tbias_1= TestRead(fbias_1,DIM2);

	for(int i = 0; i<DIM2*DIM2*DIM3; i++)
		kernel_1[i]=(float)tkernel_1[i];

	for(int i = 0; i<DIM2; i++)
		bias_1[i]=(float)tbias_1[i];

	printf("kernel 1 and bias 1 read sucessfully \r\n");

	const char *fkernel_2, *fbias_2;
	/*
	 *  kernel 2
	 *  bias 2
	 */
	fkernel_2 = "Input_Data_Files/kernel_2.txt";
	float *tkernel_2= TestRead(fkernel_2,DIM2*DIM2*DIM3);

	fbias_2 = "Input_Data_Files/bias_2.txt";
	float *tbias_2= TestRead(fbias_2,DIM2);

	for(int i = 0; i<DIM2*DIM2*DIM3; i++)
		kernel_2[i]=(float)tkernel_2[i];

	for(int i = 0; i<DIM2; i++)
		bias_2[i]=(float)tbias_2[i];

	printf("kernel 2 and bias 2 read sucessfully \r\n");

	AXI_STREAM inp_stream;
	/*
	 * StreamOut_Inst2_1,StreamOut_Inst2_2 are instance of the StreamOut_Inst1_1, StreamOut_Inst1_2 respectively.
	 * They transfers results of Convolutional Layers to the output to target the two different Fully Connected Modules
	 *
	 */
 	AXI_STREAM_ext StreamOut_Inst1_1, StreamOut_Inst1_2,StreamOut_Inst2_1,StreamOut_Inst2_2;

	/*
	 * Send all kernels data through stream
	 */

	for(int i=0; i<DIM2*DIM3; i++)
	{
 		Data_tmp_change.a=kernel_0[i];
		tempIn.data = Data_tmp_change.b;
		tempIn.keep=127;
		tempIn.last =0;
		inp_stream.write(tempIn);
	}

	for(int i=0; i<DIM2*DIM2*DIM3; i++)
	{
 		Data_tmp_change.a=kernel_1[i];
		tempIn.data = Data_tmp_change.b;
		tempIn.keep=127;
		tempIn.last =0;
		inp_stream.write(tempIn);
	}

	for(int i=0; i<DIM2*DIM2*DIM3; i++)
	{
 		Data_tmp_change.a=kernel_2[i];
		tempIn.data = Data_tmp_change.b;
		tempIn.keep=127;
		tempIn.last =0;
		inp_stream.write(tempIn);

	}

	/*
	 * Send all bias data through stream
	 */

	for(int i=0; i<DIM2; i++)
	{
 		Data_tmp_change.a=bias_0[i];
		tempIn.data = Data_tmp_change.b;
		tempIn.keep=127;
		tempIn.last =0;
		inp_stream.write(tempIn);
	}

	for(int i=0; i<DIM2; i++)
	{
		Data_tmp_change.a=bias_1[i];
		tempIn.data = Data_tmp_change.b;
		tempIn.keep=127;
		tempIn.last =0;
		inp_stream.write(tempIn);
	}


	for(int i=0; i<DIM2; i++)
	{
 		Data_tmp_change.a=bias_2[i];
		tempIn.data = Data_tmp_change.b;
		tempIn.keep=127;
		tempIn.last =0;
		inp_stream.write(tempIn);
	}

	/*
	 * Send image data through stream
	 */
	for(int i=0; i<DIM1; i++)
	{
 		Data_tmp_change.a=image[i];
		tempIn.data = Data_tmp_change.b;
		tempIn.keep=127;
		tempIn.last =(i==DIM1-1)? 1 : 0;
		inp_stream.write(tempIn);
 	}

	/*
	 * Call the Accelerator
	 */
	 Conv_Accel (inp_stream,StreamOut_Inst1_1, StreamOut_Inst1_2,StreamOut_Inst2_1,StreamOut_Inst2_2);


	/*
	 * Read from the 4 different output streams and store to the conv_hw
	 * Actually they two Instatiate of the two Streams that carry the output data
	 *
	 */

 	for(int j=0; j<890; j++){
		tempOut=StreamOut_Inst1_1.read();
		tempOut2=StreamOut_Inst1_2.read();
		ap_uint<512> temp1= tempOut.data;
		ap_uint<512> temp2= tempOut2.data;

		for(int i=0; i<16; i++){
			Data_tmp_change.b=(uint32_t)(((temp1)>> 512-32*(i+1)) & 0xFFFFFFFF);
			conv_hw[j*32+i]=Data_tmp_change.a;
			Data_tmp_change2.b=(uint32_t)(((temp2)>> 512-32*(i+1)) & 0xFFFFFFFF);
			conv_hw[j*32+i+16]=Data_tmp_change2.a;
		}
 	}

 	for(int j=0; j<890; j++){
		tempOut=StreamOut_Inst2_1.read();
		tempOut2=StreamOut_Inst2_2.read();
		ap_uint<512> temp1= tempOut.data;
		ap_uint<512> temp2= tempOut2.data;

		for(int i=0; i<16; i++){
			Data_tmp_change.b=(uint32_t)(((temp1)>> 512-32*(i+1)) & 0xFFFFFFFF);
			conv_hw[j*32+i]=Data_tmp_change.a;
			Data_tmp_change2.b=(uint32_t)(((temp2)>> 512-32*(i+1)) & 0xFFFFFFFF);
			conv_hw[j*32+i+16]=Data_tmp_change2.a;
		}
 	}

	for(int i=0; i<(DIM1-3*DIM3+3)*DIM2; i++)
	{
		printf("conv[%d] = %.7f\n" ,i,conv_hw[i]);
	}

return 0;

}




