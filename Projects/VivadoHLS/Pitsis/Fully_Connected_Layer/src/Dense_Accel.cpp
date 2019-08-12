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
#include "Dense.h"

/*
 *  Function: FC_Accel
 *
 *  This the main function of the accelerator that performs I/O communication and enables our wrapper.
 *  Inputs :
 *			 inStream : Input stream that transfers all the dense data (codebook, indexes, bias)
 *			 Conv_Stream1 : Input Stream (FIFO 512 bit width) that transfers first 16 results packed from 3 Convolutional Layers.
 *			 Conv_Stream1 : Input Stream (FIFO 512 bit width) that transfers second 16 results packed from 3 Convolutional Layers.
 *
 *  Inputs/Outputs :
 *		        	 outStream : Output stream (32 bit width) that transfers results 400 Classes.
 */
void FC_Accel (AXI_STREAM1 &inStream, AXI_STREAM_ext &Conv_Stream1, AXI_STREAM_ext &Conv_Stream2, AXI_STREAM2 &outStream)
{
#pragma HLS INTERFACE s_axilite port=return bundle=CONTROL_BUS
#pragma HLS INTERFACE axis      port=outStream
#pragma HLS INTERFACE axis register both port=inStream
#pragma HLS INTERFACE axis register both port=Conv_Stream1
#pragma HLS INTERFACE axis register both port=Conv_Stream2

	wrapped_Dense_hw<float,28480,400>(inStream,Conv_Stream1,Conv_Stream2,outStream);
	return;
}

