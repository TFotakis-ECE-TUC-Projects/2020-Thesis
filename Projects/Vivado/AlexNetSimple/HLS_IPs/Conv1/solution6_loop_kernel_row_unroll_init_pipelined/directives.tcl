############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_unroll "Conv_Core/Loop_kernel_pixel"
set_directive_unroll "Conv_Core/Loop_kernel_row"
set_directive_pipeline -II 1 "Conv_Core/Loop_output_row_write"
set_directive_pipeline -II 1 "Conv_Core/Loop_output_pixel_write"
set_directive_pipeline -II 1 "Conv_Core/Loop_weightsCache_row_init"
set_directive_pipeline -II 1 "Conv_Core/Loop_weightsCache_pixel_init"
set_directive_unroll "Conv_Core/Loop_pixels_row_init"
set_directive_unroll "Conv_Core/Loop_pixels_pixel_init"
