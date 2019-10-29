--Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2018.3.1 (lin64) Build 2489853 Tue Mar 26 04:18:30 MDT 2019
--Date        : Tue Oct 29 18:25:57 2019
--Host        : arch running 64-bit unknown
--Command     : generate_target bd_zynq_wrapper.bd
--Design      : bd_zynq_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity bd_zynq_wrapper is
end bd_zynq_wrapper;

architecture STRUCTURE of bd_zynq_wrapper is
  component bd_zynq is
  end component bd_zynq;
begin
bd_zynq_i: component bd_zynq
 ;
end STRUCTURE;
