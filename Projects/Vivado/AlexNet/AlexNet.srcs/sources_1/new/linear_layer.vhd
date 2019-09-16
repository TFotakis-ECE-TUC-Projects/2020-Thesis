library ieee;
use ieee.std_logic_1164.all;
use work.bus_array_pkg.all;


entity linear_layer is
	Generic (
		input_size: positive := 2;
		output_size: positive := 1
	);
	Port (
		Din: in bus_array(input_size - 1 downto 0);
		Weights: in bus_array(output_size * input_size - 1 downto 0);
		Bias: in bus_array(output_size - 1 downto 0);
		Dout: out bus_array(output_size - 1 downto 0)
	);
end linear_layer;


architecture Structural of linear_layer is
begin
	linear_layer_units:
	for i in 0 to output_size generate
		linear_units:
		entity work.linear_unit
			Generic Map (
				input_size => input_size
			)
			Port Map (
				Din => Din,
				Weights => Weights((i + 1) * input_size - 1 downto i * input_size),
				Bias => Bias(i),
				Dout => Dout(i)
			);
	end generate;
end Structural;
