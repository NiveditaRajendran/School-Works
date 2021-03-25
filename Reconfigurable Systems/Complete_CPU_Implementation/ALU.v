// File Name: ALU.v
// Author   : Lisler Thomson Pulikkottil, Nivedita Rajendran
// Version  : Rev C
// Description : This file implements an ALU
// Updates: Condition for Overflow Flag updation during Subtract Operation corrected.
// Date : 06/30/2019

module ALU(A, B, SSEL, F, Z, S, C, V);

	input [7:0] A, B;	// INPUTS											
	input [3:0] SSEL;	// Select line								
	output [7:0] F; // OUTPUT
	output Z,S,C,V; // Flags
	
	wire [7:0] A, B; // Inputs
	reg[8:0]	F_TEMP;  // Output
	wire [3:0] SSEL; // Function Select
	reg Z,C,V,S; // Status
	
	assign F = F_TEMP[7:0];
	
	always @(A or B or SSEL or F_TEMP)				
	begin 
	Z <= 1'b0;
	C <= 1'b0;
	V <= 1'b0;
	S <= 1'b0;
	F_TEMP <= 9'b0;
		case (SSEL)							
			4'b0000  :begin
			F_TEMP[7:0] <= A; // Transfer A 
					C <= 1'b0; 
		Z <= 1'b0; 
		S <= 1'b0; 
		V <= 1'b0;
		end
			4'b0001  :begin
			F_TEMP[7:0] <= A + 8'b00000001; // Increment A
					C <= 1'b0; 
		Z <= 1'b0; 
		S <= 1'b0; 
		V <= 1'b0;
		end
			4'b0010  :begin F_TEMP <= A + B; // Sum A, B
		
		if(F_TEMP[8] == 1)  // Carry Flag
			C <= 1'b1;
		else
			C <= 1'b0; 
		if (F_TEMP == 0)  // Zero Flag
			Z<= 1'b1;
		else
			Z<= 1'b0; 
		if (F_TEMP[7] == 1) // Sign Flag
			S <= 1'b1;
		else
			S <= 1'b0; 
		if ((A[7]==0) && (B[7]==0)) 					//Check for overflow flag
		begin
			if( F_TEMP[7] == 1 )
				V <= 1'b1;
			else 
				V <= 1'b0;
		end 
		else
			begin
				if ((A[7]==1) && (B[7]==1)) 
				begin
					if (F_TEMP[7] == 0) 
						V <= 1'b1;
					else 
						V <= 1'b0;
			end 
		end
		end
			4'b0101  :begin F_TEMP <= A - B; // Subtract A, B
		
		if(F_TEMP[8] == 1)  // Carry Flag
			C <= 1'b1;
		else
			C <= 1'b0; 
		if (F_TEMP == 0)  // Zero Flag
			Z<= 1'b1;
		else
			Z<= 1'b0;
		if (F_TEMP[7] == 1) // Sign Flag
			S <= 1'b1;
		else
			S <= 1'b0; 
		if ((A[7]==0) && (B[7]==1)) 					//Check for overflow flag
		begin
			if( F_TEMP[7] == 1 )
				V <= 1'b1;
			else 
				V <= 1'b0;
		end 
		else
			begin
				if ((A[7]==1) && (B[7]==0)) 
				begin
					if (F_TEMP[7] == 0) 
						V <= 1'b1;
					else 
						V <= 1'b0;
			end 
		end
		end
			4'b0110  :begin F_TEMP[7:0] <= A - 8'b00000001; // Decrement A
					C <= 1'b0; 
		if (F_TEMP == 0)  // Zero Flag
			Z<= 1'b1;
		else
			Z<= 1'b0; 
		if (F_TEMP[7] == 1) // Sign Flag
			S <= 1'b1;
		else
			S <= 1'b0; 
		V <= 1'b0;
		end
			4'b0111  : begin  // Transfer A with Carry 0
							F_TEMP[7:0] <= A;
							C <=  1'b0;
									
		if (F_TEMP == 0)  // Zero Flag
			Z<= 1'b1;
		else
			Z<= 1'b0; 
		if (F_TEMP[7] == 1) // Sign Flag
			S <= 1'b1;
		else
			S <= 1'b0; 
		V <= 1'b0;
						  end
			4'b1000  : begin F_TEMP[7:0] <= A & B;  // A AND B
					C <= 1'b0; 
		if (F_TEMP == 0)  // Zero Flag
			Z<= 1'b1;
		else
			Z<= 1'b0; 
		if (F_TEMP[7] == 1) // Sign Flag
			S <= 1'b1;
		else
			S <= 1'b0; 
		V <= 1'b0;
		end
			
			4'b1010  : begin F_TEMP[7:0] <= A | B;  // A OR B
					C <= 1'b0; 
		if (F_TEMP == 0)  // Zero Flag
			Z<= 1'b1;
		else
			Z<= 1'b0; 
		if (F_TEMP[7] == 1) // Sign Flag
			S <= 1'b1;
		else
			S <= 1'b0;
		V <= 1'b0;
		end

			4'b1100  : begin F_TEMP[7:0] <= A ^ B;  // A XOR B
					C <= 1'b0; 
		if (F_TEMP == 0)  // Zero Flag
			Z<= 1'b1;
		else
			Z<= 1'b0;
		if (F_TEMP[7] == 1) // Sign Flag
			S <= 1'b1;
		else
			S <= 1'b0; 
		V <= 1'b0;
		end

			4'b1110  :begin F_TEMP[7:0] <= ~ A;    // Complement A
					C <= 1'b0; 
		if (F_TEMP == 0)  // Zero Flag
			Z<= 1'b1;
		else
			Z<= 1'b0; 
		if (F_TEMP[7] == 1) // Sign Flag
			S <= 1'b1;
		else
			S <= 1'b0; 
		V <= 1'b0;
		end
			default:begin F_TEMP[7:0] <= 8'b0;
					C <= 1'b0; 
		Z <= 1'b0; 
		S <= 1'b0; 
		V <= 1'b0;
		end
		endcase
	end
endmodule

