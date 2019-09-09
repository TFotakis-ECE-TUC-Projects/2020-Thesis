library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.bus_array_pkg.ALL;

entity tb_maxpool is
end tb_maxpool;

architecture tb of tb_maxpool is

constant input_size: positive := 9;

signal Din: bus_array(8 downto 0);
signal Dout: std_logic_vector (31 downto 0);

begin
dut: entity work.max_layer
	Generic Map (input_size => input_size)
	Port Map (Din => Din, Dout => Dout);

stimuli : process
    begin
    	for j in 0 to 10 loop
			for i in 0 to input_size - 1 loop
				Din(i) <= std_logic_vector(to_unsigned(i + j * input_size, 32));
			end loop;
			wait for 100 ns;
		end loop;
        wait;
    end process;
end tb;