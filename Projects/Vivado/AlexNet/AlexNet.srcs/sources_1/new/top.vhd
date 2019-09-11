library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.bus_array_pkg.all;


entity top is
	Port (
		Din: in STD_LOGIC_VECTOR (31 downto 0);
--		Din : in bus_array (8 downto 0)(31 downto 0);
		Dout: out STD_LOGIC_VECTOR (31 downto 0)
	);
end top;


architecture Structural of top is
	signal m: bus_array(8 downto 0)(31 downto 0);
	signal maxpool1_out, maxpool2_out: std_logic_vector(31 downto 1);
begin
	m(0) <= Din;

	DinGen:
	for i in 1 to 8 generate
		m(i) <= std_logic_vector(to_unsigned(i, 32));
	end generate;

	maxpool1:
	entity work.max_layer
		Generic Map (input_size => 9)
		Port Map(Din => m, Dout => maxpool1_out);

	maxpool2:
	entity work.max_layer
		Generic Map (input_size => 9)
		Port Map(Din => m, Dout => maxpool2_out);

	linear1:
	entity work.linear_layer
		Generic (
			input_size => 2,
			output_size => 1
		)
		Port (
			Din(0) => maxpool1_out,
			Din(1) => maxpool2_out,
			Weights => Din(1 downto 0),
			Bias => Din(1),
			Dout => Dout
		);
end Structural;
