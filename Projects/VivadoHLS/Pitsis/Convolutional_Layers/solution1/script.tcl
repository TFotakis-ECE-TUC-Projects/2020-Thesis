############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project Convolutional_Layers
set_top Conv_Accel
add_files Convolutional_Layers/HLS_code/Conv_Accel.cpp
add_files Convolutional_Layers/HLS_code/Conv_def.h
add_files Convolutional_Layers/HLS_code/conv.h
add_files -tb Convolutional_Layers/HLS_code/Conv_Accel_Test.cpp
add_files -tb Convolutional_Layers/Input_Data_Files
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 3.3 -name default
#source "./Convolutional_Layers/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
