// File Name:REGFILE.v
// Author : Lisler Thomson Pulikkottil, Nivedita Rajendran
// Version: Rev C
// Description : This porgram is used to store data into one of the registers from R1 to R7 using DSEL as select lines.
//			The data stored is then output using two buses A and B according to select lines ASEL and BSEL respectively.
// 		The operation is driven using a clock input.
// Updates: CLK Sensitivity for ASEL and BSEL operations are removed.
// Date : 06/30/2019

module REGFILE(DSEL, ASEL, BSEL, RIN, CLK, DIN, A, B);

	input [2:0] DSEL; 					// Used to select registers
	input [2:0] ASEL, BSEL;				// used to select different output buses (A or B)
	input [7:0] RIN, DIN;						// Data stored in the register and the data input
	input CLK;								// Clock signal input
	output [7:0] A, B;					// Output Buses. This will be connected to LEDs to show output

	reg [7:0] A, B;
	reg [7:0] R[7:0];
	
	// Initialising te register values
	initial 
	begin
		R[1] <= 8'b00000000; // AC
		R[2] <= 8'b00000000; // X
		R[3] <= 8'b00000000; // Scratch Register
		R[4] <= 8'b00000000; // Scratch Register
		R[5] <= 8'b00000000; // Scratch Register
		R[6] <= 8'b00000000; // SP
		R[7] <= 8'b00000000; // PC
	end
	always @(negedge CLK)				// negedge is used as the push buttons are Active Low
	begin
	  case (DSEL)							// The DSEL input is complemented as the pushbuttons are Active LOW
		 3'b000  :; 						// Do Nothing
		 3'b001  : R[1] <= RIN;
		 3'b010  : R[2] <= RIN;
		 3'b011  : R[3] <= RIN;
		 3'b100  : R[4] <= RIN;
		 3'b101  : R[5] <= RIN;
		 3'b110  : R[6] <= RIN;
		 3'b111  : R[7] <= RIN; 
		 default : begin 
						// Initial Values for each Registers
						R[1] <= 8'b00000000;
					   R[2] <= 8'b00000000;
						R[3] <= 8'b00000000;
						R[4] <= 8'b00000000;
						R[5] <= 8'b00000000;
						R[6] <= 8'b00000000;
						R[7] <= 8'b00000000;
	  				  end
	  endcase
	end
	always @(*)
	begin
	  case(ASEL)							// This Case statement assigns each register value to output A depending on ASEL value
		 3'b000  : A <= DIN;
		 3'b001  : A <= R[1];
		 3'b010  : A <= R[2];
		 3'b011  : A <= R[3];
		 3'b100  : A <= R[4];
		 3'b101  : A <= R[5];
		 3'b110  : A <= R[6];
		 3'b111  : A <= R[7]; 
	  endcase
	end
	always @(*)
	begin
	  case(BSEL)							// This Case statement assigns each register value to output B depending on BSEL value
		 3'b000  : B <= DIN;
		 3'b001  : B <= R[1];
		 3'b010  : B <= R[2];
		 3'b011  : B <= R[3];
		 3'b100  : B <= R[4];
		 3'b101  : B <= R[5];
		 3'b110  : B <= R[6];
		 3'b111  : B <= R[7];
	  endcase
	end
	
endmodule
