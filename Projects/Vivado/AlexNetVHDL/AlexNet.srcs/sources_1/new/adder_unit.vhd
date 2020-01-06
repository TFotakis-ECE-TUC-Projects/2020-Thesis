library ieee;
use ieee.std_logic_1164.all;
-- use ieee.math_real.all;
-- use ieee.numeric_std.all;
use ieee.std_logic_signed.all;
use work.bus_array_pkg.all;


entity adder_unit is
	Generic (
		use_float: boolean := true;
		output_width: positive := 32
	);
	Port (
		Din: in bus_array (1 downto 0);
		Dout: out std_logic_vector (output_width - 1 downto 0)
	);
end adder_unit;


architecture Structural of adder_unit is
begin
	int:
	if not use_float generate
		Dout <= Din(0) + Din(1);
	end generate;

	float:
	if use_float generate
		ip_float_adder_unit:
		entity work.float_adder_unit
			Port Map(
				s_axis_a_tvalid => '1',
				s_axis_a_tdata => Din(0),
				s_axis_b_tvalid => '1',
				s_axis_b_tdata => Din(1),
				-- m_axis_result_tvalid => tmp_valid,
				m_axis_result_tdata => Dout
			);
	end generate;
end Structural;
