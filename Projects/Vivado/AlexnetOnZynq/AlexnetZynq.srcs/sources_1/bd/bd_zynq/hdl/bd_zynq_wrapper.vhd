--Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2019.1 (lin64) Build 2552052 Fri May 24 14:47:09 MDT 2019
--Date        : Sat Feb 15 12:45:04 2020
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
