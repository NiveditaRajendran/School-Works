// File Name: lab6_CPU.v
// Author   : Lisler Thomson Pulikkottil, Nivedita Rajendran
// Version  : Rev A
// Description : This file functions as a topfile for the assignment. The inputs and outputs are declared in the file.
// Date : 06/14/2019

module lab6_CPU(SW,KEY,LED, HEX0, HEX1, HEX2, HEX3, HEX4, HEX5);

input [9:0] SW; // input switches
input KEY; // clock input push button
output [0:6] HEX0, HEX1, HEX2, HEX3, HEX4, HEX5; // sevensegment 
output [7:0] LED; // output leds

wire [7:0] MADDR;
wire [7:0] DATA_O;
wire RD, WR;
wire [7:0] Q;
wire CLK = ~KEY;
wire [7:0] LED;

assign LED = DATA_O;
// THIS IS AN INSTANCE FOR CPU MODULE
CPU cpu1(Q, KEY, DATA_O, HEX0, HEX1, HEX2, HEX3, HEX4, HEX5, SW[9:8], MADDR, RD, WR);

// This is an instance for RAM Module
RAM (MADDR, KEY, DATA_O, RD, WR,	Q);
endmodule
