library ieee;
use ieee.std_logic_1164.all;


entity ReLU is
	Port (
		Din : in STD_LOGIC_VECTOR (31 downto 0);
		Dout : out STD_LOGIC_VECTOR (31 downto 0)
	);
end ReLU;


architecture Int of ReLU is
	signal DinLessThanZero: std_logic;
begin
	comp: 
	entity work.comp_less_than(Int_Structural)
		Port Map (
			In0 => Din,
			In1 => (others => '0'),
			Dout => DinLessThanZero
		);


	with DinLessThanZero select
		Dout <= Din when '0',
				(others => '0') when others;
end Int;


architecture Float of ReLU is
	signal DinLessThanZero: std_logic;
begin
	comp: 
	entity work.comp_less_than(Float_Structural)
		Port Map (
			In0 => Din,
			In1 => (others => '0'),
			Dout => DinLessThanZero
		);

	with DinLessThanZero select
		Dout <= Din when '0',
				(others => '0') when others;
end Float;
