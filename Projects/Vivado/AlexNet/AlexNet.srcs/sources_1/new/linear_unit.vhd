library ieee;
use ieee.std_logic_1164.all;
use work.bus_array_pkg.all;


entity linear_unit is
	Generic (
		input_size: positive := 1
	);
	Port (
		Din: in bus_array(input_size - 1 downto 0)(31 downto 0);
		Weights: in bus_array(input_size - 1 downto 0)(31 downto 0);
		Bias: in std_logic_vector(31 downto 0);
		Dout: out std_logic_vector(31 downto 0) 
	);
end linear_unit;


architecture Structural_no_ReLU of linear_unit is
	signal mul_out: bus_array((input_size / 2) - 1 downto 0);
begin
	multipliers:
	entity work.Multiplier
		Generic Map (
			input_size => input_size,
			output_width => 64
		)
		Port Map (
			Din => Din,
			Dout => mul_out
		);
		
	adders:
	entity work.Adder
		Generic Map (
			input_size => input_size / 2 + 1,
			output_width => 32
		)
		Port Map (
			Din(input_size - 2 downto 0) => mul_out,
			Din(input_size - 1) => Bias, 
			Dout => Dout
		);
end Structural_no_ReLU;


architecture Structural_with_ReLU of linear_unit is
	signal linear_out: std_logic_vector(31 downto 0);
begin
	linear_unit_x:
	entity work.linear_unit(Structural_no_ReLU)
		Generic Map (
			input_size => input_size
		)
		Port Map (
			Din => Din,
			Weights => Weights,
			Bias => Bias,
			Dout => linear_out
		);

	ReLU_x:
	entity work.ReLU(Int)
		Port Map (
			Din => linear_out,
			Dout => Dout
		);
end Structural_with_ReLU;
