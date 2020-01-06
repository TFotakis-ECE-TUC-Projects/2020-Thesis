library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.bus_array_pkg.all;


entity top_maxpool is
	Port (
		Din : in bus_array (8 downto 0);
		Dout: out STD_LOGIC_VECTOR (31 downto 0)
	);
end top_maxpool;


architecture Structural of top_maxpool is
begin
	maxpool:
	entity work.max_layer
		Generic Map (
			use_float => true,
			input_size => 9
		)
		Port Map(
			Din => Din,
			Dout => Dout
		);
end Structural;
