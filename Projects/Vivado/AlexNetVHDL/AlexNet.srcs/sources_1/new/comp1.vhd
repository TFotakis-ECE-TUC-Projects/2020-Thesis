library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


ENTITY comp1 is
	Port (
		a: IN std_logic ;
		b: IN std_logic ;
		g: IN std_logic ;
		l: IN std_logic ;
		e: IN std_logic ;
		great: OUT std_logic ;
		less: OUT std_logic ;
		equal: OUT std_logic 
	);
END;


ARCHITECTURE comp1_arch OF comp1 IS
	signal s1,s2,s3: std_logic;
begin
	s1 <= (a and (not b));
	s2  <= (not ((a and (not b)) or (b and (not a))));
	s3 <= (b and (not a));
	
	equal <= (e and s2);
	great <= (g or(e and s1));
	less  <= (l or(e and s3));
end comp1_arch;