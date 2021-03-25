// File Name: CPU.v
// Author   : Lisler Thomson Pulikkottil, Nivedita Rajendran
// Version  : Rev C
// Description : THIS FILE IMPLEMENTS A CPU THAT LINKS VARIOUS MODULES
// 	The current reveision is not a complete working code. 
//		Reading values from registers in REGFILE seems to be not working.
// Updates: Added code for MUX 1 implementation. Debug outputs were updated. 
// Date : 06/30/2019

module CPU(DATA_I, CLK, DATA_O, HEX0, HEX1, HEX2, HEX3, HEX4, HEX5, DEBUGSEL, MADDR, RD, WR);

input [7:0] DATA_I; // INPUT DATA
input CLK; // CLOCK INPUT	
input [1:0] DEBUGSEL; // Select line for DEBUG

output [7:0] DATA_O; // DATA OUTPUT
output [0:6] HEX0, HEX1, HEX2, HEX3, HEX4, HEX5;  // DEBUG OUTPUTS
output [7:0] MADDR; // Memory Address Port
output RD; // Memory Read Signal
output WR; // Memory write Signal

reg [7:0] CAR = 8'b0; // CONTROL ADDRESS REGISTER
reg [23:0] DEBUG = 24'b0; // Debug Outputs
reg [7:0] RIN = 8'b0; // Register IN 
reg [7:0] DIN = 8'b0; // Data In to Registers
reg [7:0] DATA_O = 8'b0; // Data Out
wire [7:0] EXT_addr; // External Address
reg [7:0] buf_addr = 8'b0; // Address Buffer
reg [7:0] MAR = 8'b0; // Memory Address Register
reg [7:0] IR; // Instruction Register
wire [7:0] MADDR; // Register for Memory Address Port
wire WR,RD; // Register for Read and Write Signals

wire [0:6] HEX0, HEX1, HEX2, HEX3, HEX4, HEX5; // for Seven Segment Displays
wire Z,S,C,V; // Status Flags

wire [31:0] CW; // CONTROL WORD
wire [ 2:0]  ASEL; // REGISTER SELECT
wire [ 2:0]  BSEL; // REGISTOR SELECT
wire [ 2:0]  DSEL; // DESTINATION REGISTER SELECT
wire [ 3:0]  SSEL; // ALU FUNCTION SELECT
wire [ 2:0]  HSEL; // SHIFTER FUNCTION SELECT
wire         MUX1; // EXT / INT ADDRESS SOURCE SELECT
wire [ 2:0]  MUX2; // CAR UPDATION SELECT MUX
wire [ 7:0]  ADRS; // ADDRESS TO BE LOADED (BRANCH ADDRESS)
wire [ 3:0]  MISC; // MISCELLANEOUS FUNCTIONS

wire [7:0] A, B, F, SH; // Outputs of each module
wire CO; // Carry out from Shifter

// THIS MODULE GETS THE CORRESPONDING CONTROL WORD FOR CAR VALUES
NRLT_microcode_rom rom1(CAR, CW); // INSTANCE FOR MICROCODE

assign      ASEL = CW[31:29];    // bit size:3
assign      BSEL = CW[28:26];    // bit size:3
assign      DSEL = CW[25:23];    // bit size:3
assign      SSEL = CW[22:19];    // bit size:4
assign      HSEL = CW[18:16];    // bit size:3
assign      MUX1 = CW[15];       // bit size:1
assign      MUX2 = CW[14:12];    // bit size:3
assign      ADRS = CW[11: 4];    // bit size:8
assign      MISC = CW[ 3: 0];    // bit size:4

assign RD = MISC[0]; // Connecting Signal to Read 
assign WR = MISC[1]; // Connecting Signal to Write 
assign MADDR = MAR;  // MAR connected to MADDR output of CPU

//always @(*)
//begin
assign EXT_addr[7:3] = IR[4:0];
assign EXT_addr[2:0] = 3'b0;
	
//	if ( MISC[0] == 1'b1)
//		WR = 1'b0;
//	if (MISC[1] == 1'b1)
//		WR = 1'b1;
//end

// REGISTER MODULE
REGFILE reg1(DSEL, ASEL, BSEL, RIN, CLK, DIN, A, B);

// ALU MODULE
ALU alu1(A, B, SSEL, F, Z, S, C, V);

// SHIFTER MODULE
SHIFTER shf1(F, C, HSEL, SH, CO);

always @(*)
begin
     DIN <= DATA_I; // Data Input is assigned to DIN
     DATA_O <= SH; // Shifter Output is given to Data Output
     RIN <= SH;  // Shifter Output is given to RIN of REGFILE
	  
	  // These case statement is used to assign various debug outputs
	  case (DEBUGSEL)
	  2'b00: begin
	  // Select lines and CAR in DEBUG output
				DEBUG[3:0] <= ASEL;
				DEBUG[7:4] <= BSEL;
				DEBUG[11:8] <= DSEL;
				DEBUG[15:12] <= SSEL;
				DEBUG[19:16] <= HSEL;
				DEBUG[23:20] <= CAR[3:0];
			end
	  2'b01: begin
	  // Lower 4 bits or outputs of each module and CAR in DEBUG output
				DEBUG[3:0] <= A[3:0];
				DEBUG[7:4] <= B[3:0];
				DEBUG[11:8] <= DSEL;
				DEBUG[15:12] <= F[3:0];
				DEBUG[19:16] <= SH[3:0];
				DEBUG[23:20] <= CAR[3:0];
			end
	  2'b10: begin
	  // Flags and CAR in DEBUG Output
				DEBUG[3:0] <= IR[7:4];
				DEBUG[7:4] <= IR[3:0];
				DEBUG[11:8] <= MAR[7:4];
				DEBUG[15:12] <= MAR[3:0];
				DEBUG[19:16] <= CAR[7:4];
				DEBUG[23:20] <= CAR[3:0];
			end
	  2'b11: begin
	  // Data Inputs, Data outputs, RIN and Shifter output and CAR in DEBUG output
				DEBUG[3:0] <= MADDR[7:4];
				DEBUG[7:4] <= MADDR[3:0];
				DEBUG[11:8] <= DATA_I[7:4];
				DEBUG[15:12] <= DATA_I[3:0];
				DEBUG[19:16] <= EXT_addr[7:4];
				DEBUG[23:20] <= EXT_addr[3:0];
			end
	  endcase
end

// THIS IMPLEMENTS MUX1
always @(*)
begin
	case(MUX1)
		1'b0: buf_addr <= ADRS; // Loads Internal Address
		1'b1: buf_addr <= EXT_addr; // Loads External Address
		//default: buf_addr <= 8'b0;
	endcase
end

// Loading IR
always @(negedge CLK)
begin 
	if (MISC[3] == 1'b1)
		IR <= DATA_I;
end
// Loading MAR
always @(negedge CLK)
begin
	if (MISC[2] == 1'b1)
		MAR <= A;
end

// THIS IMPLEMENTS MUX2
always @(negedge CLK)
begin

	  case(MUX2)
		 3'b000  : begin  // GO to Next address by incrementing CAR
		 CAR <= CAR + 8'b00000001;
		 end
		 3'b001  : begin  // Load Address into CAR (Branch)
		 CAR <= buf_addr;
		 end
		 3'b010  : begin	// Load on Carry = 1
		 if (C == 1) 
		 CAR <= buf_addr;
		 else
		 CAR <= CAR + 8'b00000001;
		 end
		 3'b011  : begin	// Load on Carry = 0
		 if (C == 0)
		 CAR <= buf_addr;
		 else 
		 CAR <= CAR + 8'b00000001;
		 end
		 3'b100  : begin  // Load on Zero = 1
		 if (Z == 1)
		 CAR <= buf_addr;
		 else
		 CAR <= CAR + 8'b00000001;
		 end
		 3'b101  : begin	// Load on Zero = 0
		 if ( Z == 0) 
		 CAR <= buf_addr;
		 else
		 CAR <= CAR + 8'b00000001;
		 end
		 3'b110  : begin  // Load on Sign = 1
		 if (S == 1)
		 CAR <= buf_addr;
		 else
		 CAR <= CAR + 8'b00000001;
		 end
		 3'b111  : begin	// Load on Overflow = 1
		 if (V == 1)
		 CAR <= buf_addr;
		 else 
		 CAR <= CAR + 8'b00000001;
		 end
		 default: CAR <= 8'b0;
  endcase
end

// FOLLOWING ASSIGNES THE DEBUG OUTPUT TO VARIOUS SEVEN SEGMENT DISTPLAYS

SEVENSEG SEG0(DEBUG[23:20], HEX0);
SEVENSEG SEG1(DEBUG[19:16], HEX1);
SEVENSEG SEG2(DEBUG[15:12], HEX2);
SEVENSEG SEG3(DEBUG[11:8], HEX3);
SEVENSEG SEG4(DEBUG[7:4], HEX4);
SEVENSEG SEG5(DEBUG[3:0], HEX5);

endmodule
