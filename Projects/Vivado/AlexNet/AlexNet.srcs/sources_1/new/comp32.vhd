library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


ENTITY comp32 is
	GENERIC (BW : INTEGER := 32);
	PORT (
		a_32: IN STD_LOGIC_VECTOR (BW -1 DOWNTO 0);
		b_32: IN STD_LOGIC_VECTOR (BW -1 DOWNTO 0);
		g_32: OUT STD_LOGIC;
		l_32: OUT STD_LOGIC;
		e_32: OUT STD_LOGIC 
	);
END comp32;


architecture comp32_arch of comp32 is
	signal gre: std_logic_vector(BW downto 0);
	signal les: std_logic_vector(BW downto 0);
	signal equ: std_logic_vector(BW downto 0);
begin
	gre(BW) <= '0';
	les(BW) <= '0';
	equ(BW) <= '1';
	
	
	gen: 
	for i in 0 to BW-1 generate
		biti:
		entity work.comp1 
		Port Map ( 
			a => a_32(i),
			b => b_32(i),
			g => gre(i+1),
			l => les(i+1), 
			e => equ(i+1), 
			great => gre(i), 
			less => les(i), 
			equal => equ(i)
		);
	end generate;
	
	
	g_32 <= gre(0);
	l_32 <= les(0);          
	e_32 <= equ(0);  
end architecture comp32_arch;