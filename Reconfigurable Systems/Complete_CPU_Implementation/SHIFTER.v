// File Name:SHIFTER.v
// Author : Lisler Thomson Pulikkottil, Nivedita Rajendran
// Version: Rev B
// Description : This program performs various shifting operations depending on a 3 bit select line.
// Date : 06/14/2019

module SHIFTER(F, CI, HSEL, S, CO);

	input [7:0] F; // INPUT
	input CI; //CARRY IN
	input [2:0] HSEL; // SHIFTER FUNCTION SELECT
	
	output [7:0] S; // OUTPUT
	output CO; //CARRY OUT

	reg [7:0] S;
   wire [2:0] HSEL;
	reg CO;
	reg [8:0] result; // TEMPORARY VARIABLE

	always @(F or CI or HSEL or result)
	begin
		CO <= 1'b0; // INITIALIZING CARRY OUT
		result <= 9'b0;
		case (HSEL)
			3'b000: //No Shift
				begin
					S <= F;
				end									
			3'b001: 	//Shift LEft
				begin
					S <= F << 1;
				end 								
			3'b010: //Shift Right
				begin
					S <= F >> 1;
				end 									
			3'b011: //All Zeros
				begin
					S <= 8'b0;
				end 																			
			3'b100: //Rotate Left with Carry
				begin								 	
					{CO, S} <= F << 1;
					S[0] = CI;
				end
			3'b101://Rotate Left
				begin                          
					result <= F << 1;
					result[0] <= F[7];
					S <= result[7:0];
				end
			3'b110: //Rotate Right
				begin                      
					result[0] <= F[0];
					S <= F >> 1;
					S[7] <= result[0];
				end
			3'b111: // Rotate RIght with Carry
				begin
					CO <= F[0];
					S <= F >> 1;
					S[7] <= CI;
				end
		endcase
	end

endmodule
