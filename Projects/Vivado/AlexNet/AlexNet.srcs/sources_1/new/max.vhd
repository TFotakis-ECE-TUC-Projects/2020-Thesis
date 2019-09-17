library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity max is
	Generic (use_float: boolean);
	Port (
		In0: in STD_LOGIC_VECTOR (31 downto 0);
		In1: in STD_LOGIC_VECTOR (31 downto 0);
		Dout: out STD_LOGIC_VECTOR (31 downto 0)
	);
end max;


architecture Structural of max is
	signal index: std_logic;
begin
	int_comp:
	if not use_float generate
		comp:
		entity work.comp_less_than(Int_Structural_Signed)
		Port Map(
			In0 => In0,
			In1 => In1,
			Dout => index
		);
	end generate;

	float_comp:
	if use_float generate
		comp:
		entity work.comp_less_than(Float_Structural)
		Port Map(
			In0 => In0,
			In1 => In1,
			Dout => index
		);
	end generate;

	with index select
		Dout <= In0 when '0',
				In1 when others;
end Structural;
