library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.fp_utils_pkg.all;

entity tb_comp_less_than is
end tb_comp_less_than;


architecture tb of tb_comp_less_than is
    signal In0  : std_logic_vector (31 downto 0);
    signal In1  : std_logic_vector (31 downto 0);
    signal Dout : std_logic;
begin
	dut:
	-- entity work.comp_less_than(Int_Behavioral_Signed)
	-- entity work.comp_less_than(Int_Behavioral_Unsigned)
	-- entity work.comp_less_than(Int_Structural_Signed)
	-- entity work.comp_less_than(Int_Structural_Unsigned)
	entity work.comp_less_than(Float_Structural)
    	Port Map (
			In0  => In0,
            In1  => In1,
			Dout => Dout
		);

    stimuli : process
    begin
        In0 <= (others => '0');
        In1 <= (others => '0');
		wait for 100ns;

        -- In0 <= std_logic_vector(to_unsigned(1, 32));
        -- In1 <= std_logic_vector(to_unsigned(2, 32));
        In0 <= real_to_flt(1.0, normal, 32, 24);
        In1 <= real_to_flt(2.0, normal, 32, 24);
		wait for 100ns;

        -- In0 <= std_logic_vector(to_unsigned(2, 32));
        -- In1 <= std_logic_vector(to_unsigned(1, 32));
		In0 <= real_to_flt(2.0, normal, 32, 24);
        In1 <= real_to_flt(1.0, normal, 32, 24);
		wait for 100ns;

        -- In0 <= std_logic_vector(to_unsigned(-1, 32));
        -- In1 <= std_logic_vector(to_unsigned(-2, 32));
		-- wait for 100ns;

        -- In0 <= std_logic_vector(to_unsigned(-2, 32));
        -- In1 <= std_logic_vector(to_unsigned(-1, 32));
		-- wait for 100ns;

        -- In0 <= std_logic_vector(to_unsigned(1, 32));
        -- In1 <= std_logic_vector(to_unsigned(-2, 32));
		-- wait for 100ns;

        -- In0 <= std_logic_vector(to_unsigned(2, 32));
        -- In1 <= std_logic_vector(to_unsigned(-1, 32));
		-- wait for 100ns;

        -- In0 <= std_logic_vector(to_unsigned(-1, 32));
        -- In1 <= std_logic_vector(to_unsigned(2, 32));
		-- wait for 100ns;

        -- In0 <= std_logic_vector(to_unsigned(-2, 32));
        -- In1 <= std_logic_vector(to_unsigned(1, 32));
		-- wait for 100ns;

        wait;
    end process;
end tb;
