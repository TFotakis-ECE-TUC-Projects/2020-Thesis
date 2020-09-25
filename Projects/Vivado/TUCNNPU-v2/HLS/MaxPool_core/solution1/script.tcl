############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project MaxPool_core
set_top MaxPool_Core
add_files MaxPool_core/src/maxpool_core.cpp
add_files MaxPool_core/src/maxpool_core.hpp
add_files -tb MaxPool_core/src/maxpool_core_tb.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xczu9eg-ffvb1156-2-e}
create_clock -period 2 -name default
config_export -format ip_catalog -rtl verilog
#source "./MaxPool_core/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
