############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project Linear
set_top Linear_Core
add_files Linear/src/linear_core.c
open_solution "solution2_loop_in_feature_unroll"
set_part {xczu9eg-ffvb1156-2-e}
create_clock -period 10 -name default
config_export -format ip_catalog -rtl verilog
source "./Linear/solution2_loop_in_feature_unroll/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -rtl verilog -format ip_catalog
