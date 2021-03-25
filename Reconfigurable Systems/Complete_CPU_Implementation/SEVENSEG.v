// Filename: SEVSEG.v
// Author: Lisler Thomson Pulikkottil, Nivedita Rajendran
// Version: Rev B
// Description: This file implements the seven segment display. 
// Date : 06/14/2019

module SEVENSEG(SW, HEX);

input [3:0] SW; // Switches are set as inputs
output [0:6] HEX; // Seven Segment Outputs
reg [0:6] HEX;

always @(SW)   // This makes sure that the output only changes when there is a change in SW
begin

case(SW)
 4'b0000: HEX = 7'b0000001; // "0"
 4'b0001: HEX = 7'b1001111; // "1"
 4'b0010: HEX = 7'b0010010; // "2"
 4'b0011: HEX = 7'b0000110; // "3"
 4'b0100: HEX = 7'b1001100; // "4"
 4'b0101: HEX = 7'b0100100; // "5"
 4'b0110: HEX = 7'b0100000; // "6"
 4'b0111: HEX = 7'b0001111; // "7"
 4'b1000: HEX = 7'b0000000; // "8"
 4'b1001: HEX = 7'b0001100; // "9"
 4'b1010: HEX = 7'b0001000; // "A"
 4'b1011: HEX = 7'b1100000; // "B"
 4'b1100: HEX = 7'b0110001; // "C"
 4'b1101: HEX = 7'b1000010; // "D"
 4'b1110: HEX = 7'b0110000; // "E"
 4'b1111: HEX = 7'b0111000; // "F"
 default: HEX = 7'bXXXXXXX;
endcase;

end

endmodule
