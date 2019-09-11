library ieee;
use ieee.std_logic_1164.all;
use work.bus_array_pkg.all;


entity Adder is
	Generic (
		input_size: positive := 2;
		output_width: positive := 32
	);
	Port (
		Din: in bus_array (input_size - 1 downto 0)(31 downto 0);
		Dout: out std_logic_vector (output_width downto 0)
	);
end Adder;


architecture Int_Behavioral of Adder is
	use IEEE.STD_LOGIC_SIGNED.ALL;

	signal partial: bus_array (input_size - 1 downto 0)(output_width downto 0);
begin
	partial(0) <= Din(0);

	adderTree:
	for i in 1 to input_size - 1 generate
		partial(i) <= Din(i) + partial(i - 1);
	end generate;

	Dout <= partial(input_size - 1);
end Int_Behavioral;
