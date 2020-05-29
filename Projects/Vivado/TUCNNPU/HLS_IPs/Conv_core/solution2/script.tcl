############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project Conv_core
set_top Conv_Core
add_files Conv_core/src/conv_core.cpp
open_solution "solution2"
set_part {xczu9eg-ffvb1156-2-e}
create_clock -period 1 -name default
config_export -format ip_catalog -rtl vhdl -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
config_sdx -optimization_level none -target none
source "./Conv_core/solution2/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -rtl vhdl -format ip_catalog
