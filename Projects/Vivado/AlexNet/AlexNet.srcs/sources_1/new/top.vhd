library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.bus_array_pkg.all;


entity top is
	Port ( 
		Din: in STD_LOGIC_VECTOR (31 downto 0);
--		Din : in bus_array (8 downto 0);
		Dout: out STD_LOGIC_VECTOR (31 downto 0)
	);
end top;


architecture Structural of top is
	signal m: bus_array(8 downto 0);
begin
	m(0) <= Din;


	DinGen: 
	for i in 1 to 8 generate
		m(i) <= std_logic_vector(to_unsigned(i, 32));
	end generate;

	
	maxpool1:
	entity work.max_layer 
		Generic Map (input_size => 9) 
		Port Map(Din => m, Dout => Dout);
end Structural;
