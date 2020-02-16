############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project Linear_core
set_top Linear_Core
add_files Linear_core/src/linear_core.c
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-e} -tool vivado
create_clock -period 10 -name default
#source "./Linear_core/solution1/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -rtl vhdl -format ip_catalog
