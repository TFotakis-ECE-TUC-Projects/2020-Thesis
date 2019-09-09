library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity max is
	Port (
		In0: in STD_LOGIC_VECTOR (31 downto 0);
		In1: in STD_LOGIC_VECTOR (31 downto 0);
		Dout: out STD_LOGIC_VECTOR (31 downto 0)
	);
end max;


architecture Structural of max is
	signal index: std_logic;
begin
	comp: 
--	entity work.comp_less_than(Int_Structural) 
--	entity work.comp_less_than(Int_Behavioral) 
	entity work.comp_less_than(Float_Structural) 
		Port Map(
			In0 => In0,
			In1 => In1,
			Dout => index
		);
	
	
	with index select
		Dout <= In0 when '0',
				In1 when others;
end Structural;
