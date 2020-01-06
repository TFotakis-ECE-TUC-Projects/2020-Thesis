library ieee;
use ieee.std_logic_1164.all;
--use ieee.std_logic_signed.all;
use ieee.numeric_std.all;
use ieee.math_real.all;
use work.fp_utils_pkg.all;
use work.bus_array_pkg.ALL;


entity tb_multiplier_unit is
end tb_multiplier_unit;

architecture tb of tb_multiplier_unit is
	constant use_float: boolean := true;
	constant input_size: positive := 2;
	constant output_width: positive := 32;

	signal Din: bus_array(input_size - 1 downto 0);
	signal Dout: std_logic_vector (output_width - 1 downto 0);
begin
	mul_unit:
	entity work.multiplier_unit
		Generic Map (
			use_float => use_float,
			output_width => output_width
		)
		Port Map (
			Din => Din,
			Dout => Dout
		);


	stimuli:
	process
	begin
    	for j in 0 to 10 loop
			for i in 0 to (input_size - 1) / 2 loop
				if not use_float then
					Din(i) <= std_logic_vector(to_signed(i + j * input_size, 32));
				else
					Din(i) <= real_to_flt(0.1 * real(i + j * input_size + 3), normal, 32, 24);
				end if;
			end loop;
			for i in (input_size - 1) / 2 + 1 to input_size - 1 loop
				if not use_float then
					Din(i) <= std_logic_vector(to_signed(-1 * (i + j * input_size), 32));
				else
					Din(i) <= real_to_flt(-0.1 * real(i + j * input_size + 3), normal, 32, 24);
				end if;
			end loop;
			wait for 100 ns;
		end loop;
        wait;
    end process;
end tb;
