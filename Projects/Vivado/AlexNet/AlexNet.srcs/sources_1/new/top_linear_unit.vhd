library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.bus_array_pkg.all;


entity top_linear_unit is
	Port (
		Din : in bus_array (1 downto 0);
		Weights : in bus_array (1 downto 0);
		Bias: in STD_LOGIC_VECTOR (31 downto 0);
		Dout: out STD_LOGIC_VECTOR (31 downto 0)
	);
end top_linear_unit;


architecture Structural of top_linear_unit is
begin
	linear:
	entity work.linear_unit
		Generic Map (
			use_float => true,
			input_size => 2
		)
		Port Map (
			Din => Din,
			Weights => Weights,
			Bias => Bias,
			Dout => Dout
		);
end Structural;
