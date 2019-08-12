############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project Fully_Connected_Layer
set_top FC_Accel
add_files Fully_Connected_Layer/src/Dense_def.h
add_files Fully_Connected_Layer/src/Dense_Accel.cpp
add_files -tb Fully_Connected_Layer/Input_Data_Files -cflags "-Wno-unknown-pragmas"
add_files -tb Fully_Connected_Layer/src/Dense_Accel_Test.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 3.3 -name default
#source "./Fully_Connected_Layer/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl vhdl -format ip_catalog
