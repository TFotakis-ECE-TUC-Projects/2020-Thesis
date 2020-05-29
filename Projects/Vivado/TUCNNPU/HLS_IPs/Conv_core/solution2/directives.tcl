############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_array_partition -type complete -dim 1 "Conv_Core/Loop_output_channel" pixels
set_directive_array_partition -type complete -dim 1 "Conv_Core/Loop_kernel" weightsCache
set_directive_pipeline -II 1 "Conv_Core/Loop_weightsCache_init"
set_directive_unroll -factor 11 "Conv_Core/Loop_kernel_pixel"
set_directive_pipeline "Conv_Core/Loop_pixels_row_init"
