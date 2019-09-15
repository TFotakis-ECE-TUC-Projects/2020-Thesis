library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;


entity tb_comp_less_than is
end tb_comp_less_than;


architecture tb of tb_comp_less_than is
	-- Enumerated type to indicate special floating point values.
	-- Note: denormalized numbers are treated as zero, and signalling NaNs are treated as quiet NaNs.
	-- Out Of Range just means the value is larger than VHDL can support in a Real type
	type floating_point_special_t is (normal, zero_pos, zero_neg, inf_pos, inf_neg, nan, out_of_range);

	-- Function to limit an integer bit width to a value for which functions can use VHDL's integer type for calculations
	function width_limit(w : integer) return integer is
		constant MAX_WIDTH : integer := 30;  -- maximum width: allows 2**width to be stored in an integer variable
	begin
		if w < MAX_WIDTH then
			return w;
		else
			return MAX_WIDTH;
		end if;
	end function width_limit;

	-- Function to convert a real number to a std_logic_vector floating point representation
	function real_to_flt(x : real;                       -- real number to convert
						s : floating_point_special_t;   -- indicates special values to convert (overrides x)
						w, fw : integer)                -- total and fractional bit width of result
	return std_logic_vector is
	constant EW       : integer := w - fw;
	constant FW_LIM   : integer := width_limit(fw);    -- limited internal value of fractional bit width
	constant E_BIAS   : integer := 2 ** (EW - 1) - 1;
	constant MANT_MAX : real    := 2.0 - 1.0 / real(2 ** (FW_LIM - 1));
	variable result   : std_logic_vector(w-1 downto 0) := (others => '0');
	variable sign     : std_logic := '0';
	variable exp      : integer   := 0;
	variable mant     : real;
	variable mant_int : integer;
	begin

	-- Handle special cases
	case s is
		when zero_pos =>  -- plus zero
		result(w-1) := '0';                          -- sign bit clear
		result(w-2 downto 0) := (others => '0');     -- exponent and mantissa clear

		when zero_neg =>  -- minus zero
		result(w-1) := '1';                          -- sign bit set
		result(w-2 downto 0) := (others => '0');     -- exponent and mantissa clear

		when inf_pos =>   -- plus infinity
		result(w-1) := '0';                          -- sign bit clear
		result(w-2 downto fw-1) := (others => '1');  -- exponent
		result(fw-2 downto 0)   := (others => '0');  -- mantissa

		when inf_neg =>   -- minus infinity
		result(w-1) := '1';                          -- sign bit set
		result(w-2 downto fw-1) := (others => '1');  -- exponent
		result(fw-2 downto 0)   := (others => '0');  -- mantissa

		when nan =>       -- Not a Number
		result(w-1) := '0';                          -- sign bit
		result(w-2 downto fw-1) := (others => '1');  -- exponent
		result(fw-2)            := '1';              -- MSB of mantissa set
		result(fw-3 downto 0)   := (others => '0');  -- rest of mantissa clear

		-- NOTE: out_of_range might be possible under some circumstances, but it can be represented
		when normal | out_of_range =>
		-- Handle normal numbers

		-- Zero must be requested using s = zero_pos or s = zero_neg, not s = normal and x = 0.0
		assert x /= 0.0
			report "ERROR: real_to_flt: zero must be requested using s = zero_pos or s = zero_neg, not s = normal and x = 0.0"
			severity failure;

		-- Get sign bit
		if x < 0.0 then
			sign := '1';
			mant := -x;
		else
			sign := '0';
			mant := x;
		end if;

		-- Normalize input to calculate exponent
		while mant < 1.0 loop
			exp  := exp - 1;
			mant := mant * 2.0;
		end loop;
		while mant > MANT_MAX loop
			exp  := exp + 1;
			mant := mant / 2.0;
		end loop;

		-- Remove hidden bit and convert to std_logic_vector
		-- To avoid VHDL's integer type overflowing, use at most 30 bits of the mantissa
		mant := mant - 1.0;
		mant_int := integer(mant * real(2 ** (FW_LIM - 1)));  -- implicit round-to-nearest
		result(fw - 2 downto fw - FW_LIM) := std_logic_vector(to_unsigned(mant_int, FW_LIM - 1));

		-- Add exponent bias and convert to std_logic_vector
		exp := exp + E_BIAS;
		result(w - 2 downto fw - 1) := std_logic_vector(to_unsigned(exp, EW));

		-- Add sign bit
		result(w - 1) := sign;

	end case;

	return result;
	end function real_to_flt;

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
