############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project Conv1
set_top Conv_Core
add_files Conv1/src/conv_core.c
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-e} -tool vivado
create_clock -period 10 -name default
config_export -format ip_catalog -rtl verilog
source "./Conv1/solution1/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -rtl verilog -format ip_catalog
