/*
******************************************************************************
  Vivado HLS (R) is a trademark of The Xilinx (R) Corporation

  Filename:    Conv_Accel.h
  Author:      Pitsis Giorgos
  Version:     1.0
  Date:        September 29, 2018
  Copyright:   (c) 2018 by Pitsis Giorgos, All Rights Reserved
******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <stdint.h>
#include "conv.h"

/*
 *  Function: Conv_Accel
 *
 *  This the main function of the accelerator that performs I/O communication and enables our wrapper.
 *  Inputs :
 *			inStream : Input stream that transfers all the data (image, kernels, bias)
 *
 *  Inputs/Outputs :
 *		        	StreamOut_Inst1_1 : Streams (512 bit width) that transfers first 16 results packed from 3 Convolutional Layers.
 *			        StreamOut_Inst1_2 : Streams (512 bit width) that transfers second 16 results packed from 3 Convolutional Layers.
 *			        StreamOut_Inst2_1 : Streams (512 bit width) that transfers first 16 results packed from 3 Convolutional Layers (Instance).
 *			        StreamOut_Inst2_2 : Streams (512 bit width) that transfers first 16 results packed from 3 Convolutional Layers (Instance).
 */
 void Conv_Accel (AXI_STREAM &inStream,AXI_STREAM_ext &StreamOut_Inst1_1, AXI_STREAM_ext &StreamOut_Inst1_2, AXI_STREAM_ext &StreamOut_Inst2_1, AXI_STREAM_ext &StreamOut_Inst2_2)
{

#pragma HLS INTERFACE s_axilite port=return     bundle=CONTROL_BUS
#pragma HLS INTERFACE axis      port=StreamOut_Inst1_1
#pragma HLS INTERFACE axis      port=StreamOut_Inst1_2
#pragma HLS INTERFACE axis      port=StreamOut_Inst2_1
#pragma HLS INTERFACE axis      port=StreamOut_Inst2_2
#pragma HLS INTERFACE axis      port=inStream


	wrapped_Conv_hw<float,1800,16,8>(inStream,StreamOut_Inst1_1,StreamOut_Inst1_2,StreamOut_Inst2_1,StreamOut_Inst2_2);

	return;
}


