library ieee;
use ieee.std_logic_1164.all;
-- use ieee.math_real.all;
-- use ieee.numeric_std.all;
use ieee.std_logic_signed.all;
use work.bus_array_pkg.all;


entity multiplier_unit is
	Generic (
		use_float: boolean := true
	);
	Port (
		Din: in bus_array (1 downto 0);
		Dout: out std_logic_vector (63 downto 0)
	);
end multiplier_unit;


architecture Structural of multiplier_unit is
begin
	int:
	if not use_float generate
		Dout <= Din(0) * Din(1);
	end generate;

	float:
	if use_float generate
		ip_float_multiplier_unit:
		entity work.float_multiplier_unit
			Port Map(
				s_axis_a_tvalid => '1',
				s_axis_a_tdata => Din(0),
				s_axis_b_tvalid => '1',
				s_axis_b_tdata => Din(1),
				-- m_axis_result_tvalid => tmp_valid,
				m_axis_result_tdata => Dout(31 downto 0)
			);
		Dout(63 downto 32) <= (others => '0');
	end generate;
end Structural;
