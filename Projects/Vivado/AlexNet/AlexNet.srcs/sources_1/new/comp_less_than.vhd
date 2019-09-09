library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity comp_less_than is
	Port (
		In0: in STD_LOGIC_VECTOR (31 downto 0);
		In1: in STD_LOGIC_VECTOR (31 downto 0);
		Dout: out STD_LOGIC
	);
end comp_less_than;


architecture Int_Behavioral of comp_less_than is
begin
	DOUT <= '1' when (In0<In1)	 else '0';
end Int_Behavioral;


architecture int_Structural of comp_less_than is
begin
	less_than:
	entity work.comp32
		Port Map (
			a_32 => In0,
			b_32 => In1,
			l_32 => Dout
		);
end int_Structural;


architecture Float_Structural of comp_less_than is
	signal tmp: STD_LOGIC_VECTOR(7 DOWNTO 0);
	signal tmp_valid: STD_LOGIC;
begin
	ip_float_less_than: 
	entity work.float_less_than 
		Port Map(
			s_axis_a_tvalid => '1',
			s_axis_a_tdata => In0,
			s_axis_b_tvalid => '1',
			s_axis_b_tdata => In1,
			m_axis_result_tvalid => tmp_valid,
			m_axis_result_tdata => tmp
		);
	
	
	Dout <= tmp(0);
end Float_Structural;
