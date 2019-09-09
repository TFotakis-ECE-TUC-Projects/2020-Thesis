library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

package bus_array_pkg is
	type bus_array is array (natural range <>) of std_logic_vector (31 downto 0);
--	type bus_2d_array is array (natural range <>) of bus_array;
end package;
