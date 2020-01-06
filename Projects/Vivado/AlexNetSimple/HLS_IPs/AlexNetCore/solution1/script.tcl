############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project AlexNetCore
set_top AlexNetCore
add_files AlexNetCore/src/alexnetCore.c
add_files AlexNetCore/src/AlexNetParams.h
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-i} -tool vivado
create_clock -period 10 -name default
config_export -format ip_catalog -rtl vhdl
#source "./AlexNetCore/solution1/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -rtl vhdl -format ip_catalog
