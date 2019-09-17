library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.math_real.all;
use work.bus_array_pkg.all;


entity max_layer is
	Generic	(
		use_float: boolean := true;
		input_size: positive := 9
	);
	Port (
		Din: in bus_array(input_size - 1 downto 0);
		Dout: out STD_LOGIC_VECTOR (31 downto 0)
	);
end max_layer;


architecture Structural of max_layer is
	constant modules: positive := integer(floor(real(input_size) / 2.0));
	constant outputs: positive := integer(ceil(real(input_size) / 2.0));

	signal m: bus_array(outputs - 1 downto 0);
begin
	Max_mod:
	for i in 0 to modules - 1 generate
		max_x:
		entity work.max
			Generic Map (use_float => use_float)
			Port Map (
				In0 => Din(i * 2),
				In1 => Din(i * 2 + 1),
				Dout => m(i)
			);
	end generate;

	Odd_Input:
	if input_size mod 2 = 1 generate
		m(outputs - 1) <= Din(input_size - 1);
	end generate;

	Next_Layer:
	if input_size /= 2 generate
		next_max_layer:
		entity work.max_layer
			Generic Map (
				use_float => use_float,
				input_size => outputs
			)
			Port Map (
				Din => m, Dout=>Dout
			);
	end generate;

	Last_Layer:
	if input_size = 2 generate
		Dout <= m(0);
	end generate;
end Structural;
