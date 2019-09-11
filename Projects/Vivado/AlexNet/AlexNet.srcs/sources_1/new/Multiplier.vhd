library ieee;
use ieee.std_logic_1164.all;
use work.bus_array_pkg.all;


entity Multiplier is
	Generic (
		input_size: positive := 2;
		output_width: positive := 64
	);
	Port (
		Din: in bus_array (input_size - 1 downto 0)(31 downto 0);
		Dout: out bus_array((input_size / 2)  - 1 downto 0)(output_width - 1 downto 0)
	);
end Multiplier;


architecture Int_Behavioral of Multiplier is
	use IEEE.STD_LOGIC_SIGNED.ALL;
begin
	multiplierTree:
	for i in 0 to (input_size / 2) - 1 generate
		Dout(i) <= Din(i) * Din(i + 1);
	end generate;
end Int_Behavioral;
