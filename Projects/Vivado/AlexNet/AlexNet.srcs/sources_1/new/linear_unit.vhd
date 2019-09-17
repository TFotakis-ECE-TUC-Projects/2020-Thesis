library ieee;
use ieee.std_logic_1164.all;
use work.bus_array_pkg.all;


entity linear_unit is
	Generic (
		use_float: boolean := true;
		input_size: positive := 1
	);
	Port (
		Din: in bus_array(input_size - 1 downto 0);
		Weights: in bus_array(input_size - 1 downto 0);
		Bias: in std_logic_vector(31 downto 0);
		Dout: out std_logic_vector(31 downto 0)
	);
end linear_unit;


architecture Structural_no_ReLU of linear_unit is
	signal mul_out, unused_mul_out: bus_array(input_size - 1 downto 0);
begin
	multipliers:
	for i in 0 to input_size - 1 generate
		mul_unit:
		entity work.multiplier_unit
			Generic Map (
				use_float => use_float
			)
			Port Map (
				Din(0) => Din(i),
				Din(1) => Weights(i),
				Dout(31 downto 0) => mul_out(i),
				Dout(63 downto 32) => unused_mul_out(i)
			);
	end generate;

	adders:
	entity work.adder_layer
		Generic Map (
			use_float => use_float,
			input_size => input_size + 1,
			output_width => 32
		)
		Port Map (
			Din(input_size - 1 downto 0) => mul_out,
			Din(input_size) => Bias,
			Dout => Dout
		);
end Structural_no_ReLU;


architecture Structural_with_ReLU of linear_unit is
	signal linear_out: std_logic_vector(31 downto 0);
begin
	linear_unit_x:
	entity work.linear_unit(Structural_no_ReLU)
		Generic Map (
			use_float => use_float,
			input_size => input_size
		)
		Port Map (
			Din => Din,
			Weights => Weights,
			Bias => Bias,
			Dout => linear_out
		);

	Int_ReLU:
	if not use_float generate
		ReLU_x:
		entity work.ReLU(Int)
		Port Map (
			Din => linear_out,
			Dout => Dout
		);
	end generate;

	Float_ReLU:
	if use_float generate
		ReLU_x:
		entity work.ReLU(Float)
		Port Map (
			Din => linear_out,
			Dout => Dout
		);
	end generate;
end Structural_with_ReLU;
