--
-- DO NOT EDIT THIS FILE, your changes will be overwritten
-- This is an automatically generated file.
-- It was generated with uasm, the microassembler, on
-- Fri Jun 14 14:09:24 2019
--
--
-- component LT_microcode_rom
--   port( ADDR_IN  : in  std_logic_vector( 7 downto 0);
--         DATA_OUT : out std_logic_vector(31 downto 0));
-- end component;
--
--
-- This case statement was generated with the following
--   fields:
--
--  signal     DATA_OUT : std_logic_vector(31 downto 0);
--  signal         ASEL : std_logic_vector( 2 downto 0);
--  signal         BSEL : std_logic_vector( 2 downto 0);
--  signal         DSEL : std_logic_vector( 2 downto 0);
--  signal         SSEL : std_logic_vector( 3 downto 0);
--  signal         HSEL : std_logic_vector( 2 downto 0);
--  signal         MUX1 : std_logic;
--  signal         MUX2 : std_logic_vector( 2 downto 0);
--  signal         ADRS : std_logic_vector( 7 downto 0);
--  signal         MISC : std_logic_vector( 3 downto 0);
--
--
--                 ASEL <= DATA_OUT(31 downto 29);   -- bit size: 3
--                 BSEL <= DATA_OUT(28 downto 26);   -- bit size: 3
--                 DSEL <= DATA_OUT(25 downto 23);   -- bit size: 3
--                 SSEL <= DATA_OUT(22 downto 19);   -- bit size: 4
--                 HSEL <= DATA_OUT(18 downto 16);   -- bit size: 3
--                 MUX1 <= DATA_OUT(15);             -- bit size: 1
--                 MUX2 <= DATA_OUT(14 downto 12);   -- bit size: 3
--                 ADRS <= DATA_OUT(11 downto  4);   -- bit size: 8
--                 MISC <= DATA_OUT( 3 downto  0);   -- bit size: 4
--
-- Yeilding a total data width of 32 bits for 9 fields.
-- The maximum address encountered was 0xff, needing 8 bits
--
library ieee;
use ieee.std_logic_1164.ALL;

entity LT_microcode_rom is
port( ADDR_IN  : in  std_logic_vector( 7 downto 0);
      DATA_OUT : out std_logic_vector(31 downto 0));
end LT_microcode_rom;

architecture RTL of LT_microcode_rom is

   signal DATA_OUT_R : std_logic_vector(31 downto 0);
begin
   DATA_OUT <= DATA_OUT_R;

   process(ADDR_IN)
     begin
        case ADDR_IN is
          -- START:
          when "00000000" => DATA_OUT_R <= "00000000100000000000000000000000";
          when "00000001" => DATA_OUT_R <= "00000001000000000000000000000000";
          when "00000010" => DATA_OUT_R <= "00101001110000000000000000000000";
          when "00000011" => DATA_OUT_R <= "00100010000010000000000000000000";
          when "00000100" => DATA_OUT_R <= "00000001100000010000000000000000";
          when "00000101" => DATA_OUT_R <= "01110010100100000000000000000000";
          when "00000110" => DATA_OUT_R <= "10100000000000000001000000000000";
          when "11111111" => DATA_OUT_R <= "00000000000000000000000000000000";
          when others => DATA_OUT_R <= "00000000000000000000000000000000";
       end case;
     end process;
end RTL;
