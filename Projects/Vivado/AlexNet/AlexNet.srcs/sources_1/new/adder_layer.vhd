library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.all;
-- use ieee.numeric_std.all;
use ieee.std_logic_signed.all;
use work.bus_array_pkg.all;


entity adder_layer is
	Generic (
		use_float: boolean := true;
		input_size: positive := 2;
		output_width: positive := 32
	);
	Port (
		Din: in bus_array (input_size - 1 downto 0);
		Dout: out std_logic_vector (output_width - 1 downto 0)
	);
end adder_layer;


architecture Structural of adder_layer is
	type custom_bus_array is array (natural range <>) of std_logic_vector (output_width - 1 downto 0);
	signal partial: custom_bus_array (input_size - 1 downto 0);
begin
	partial(0)(31 downto 0) <= Din(0);
	partial(0)(output_width - 1 downto 32) <= (others => '0');

	adderTree:
	for i in 1 to input_size - 1 generate
		adder_unit:
		entity work.adder_unit
			Generic Map (
				use_float => use_float,
				output_width => output_width
			)
			Port Map (
				Din(0) => Din(i),
				Din(1) => partial(i - 1)(31 downto 0),
				Dout => partial(i)
			);
	end generate;

	Dout <= partial(input_size - 1);
end Structural;
