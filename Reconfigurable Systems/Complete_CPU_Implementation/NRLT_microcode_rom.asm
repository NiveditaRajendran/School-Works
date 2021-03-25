;;; Simple CPU microcode file.
;;; Includes the example microcode for our CPU

;;; Desired number of Address bits
DEPTH:	.equ 8			;

	
;;; Field Definitions

ASEL:	.field	3		; Right input of ALU
BSEL:	.field	3		; Left inpt of ALU
DSEL:	.field	3		; Destination Register
SSEL:	.field  4		; ALU operation
HSEL:	.field	3		; Shifter operation
MUX1:	.field	1		; Next Address Select
MUX2:	.field	3		; Load\Increment select
ADRS:	.field  DEPTH		; Address field
MISC:	.field	4		; Misc Field
	
;;; Constant definitions
;;; ASEL, BSEL, DSEL fields
INP:	.equ	000b		; Select Input port
NONE:	.equ	000b		; No register write
R1:	.equ	001b		; Register 1
R2:	.equ	010b		; Register 2
R3:	.equ	011b		; Register 3
R4:	.equ	100b		; Register 4
R5:	.equ	101b		; Register 5
R6:	.equ	110b		; Register 6
R7:	.equ	111b		; Register 7

;;; SSEL ALU Selection Field 
;;; 			Flags Affected
;;; 			 Z S C V	Function
TSF:	.equ	0000b ;  N N N N	Transfer A 
INC:	.equ	0001b ;  Y Y N N	Increment A by one
ADD:	.equ	0010b ;	 Y Y Y Y	Add A+B
SUB:	.equ	0101b ;	 Y Y Y Y	Subtract A-B
DEC:	.equ	0110b ;	 Y Y N N	Decrement A by one
TRC:	.equ	0111b ;	 Y Y 0 N	Transfer A and Carry=0
AND:	.equ	1000b ;	 Y Y N N	A AND B
OR:	.equ	1010b ;	 Y Y N N	A OR B
XOR:	.equ	1100b ;	 Y Y N N	A Exclusive OR B
COM:	.equ	1110b ;	 Y Y N N	Complement A

	;
;;; HSEL Shifter Selection Field 
;;; 			Function
NSH:	.equ	000b ; 	No Shift
SHL:	.equ	001b ; 	Shift Left
SHR:	.equ	010b ;  Shift Right
ZERO:	.equ	011b ; 	All Zero's in output
RLC:	.equ	100b ; 	Rotate left with carry
ROL:	.equ	101b ; 	Rotate left
ROR:	.equ	110b ; 	Rotate right
RRC:	.equ	111b ; 	Rotate right with carry

;;; MUX1 Field 
;;; 			Function
INT:	.equ    0b   ; Internal Address
EXT:	.equ	1b   ; External Address

;;; MUX2 Field
;;; 			Function
NEXT:	.equ 000b ; 	Go to next address by incrementing CAR
LAD:	.equ 001b ; 	Load address into CAR (Branch)
LC:	.equ 010b ; 	Load on Carry = 1
LNC:	.equ 011b ; 	Load on Carry = 0
LZ:	.equ 100b ; 	Load on Zero = 1
LNZ:	.equ 101b ; 	Load on Zero = 0
LS:	.equ 110b ; 	Load on Sign = 1
LV:	.equ 111b ; 	Load on Overflow = 1	

;;; MISC Field
READ:	.equ 0001b ; Read Memory
WRITE:	.equ 0010b ; Write Memory
LDMAR:	.equ 0100b ; Load MAR
LDIR:	.equ 1000b ; Load IR
FETCH:	.equ 1001b ; Read an instruction into IR
DEREF:	.equ 0101b ; Read mem, and load MAR

	.module "NRLT_microcode_rom"	;

	.org 0			;
PC:	.equ R7					   ;  PC is R7
SP:	.equ R6					   ;  SP is R6
AC:	.equ R1					   ;  AC is R1
X:	.equ R2					   ;  X is R2

	- \ - \ - \ TSF \ NSH \ -  \ NEXT \ - \ - ; No operation
START:	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ; Set MADDR
	- \ - \ - \ - \ - \ - \ - \ - \ READ  ; Wait for RAM
	- \ - \ - \ - \ - \ - \ - \ - \ FETCH ; Load IR
	INP\ - \ - \ - \ - \ EXT \ LAD \ - \ - ; Execute

	.org (1 << 3)	; INA
	AC \ - \ AC \ INC \ NSH \ - \ LAD \ START \ - ;

	.org (2 << 3)      	; CMA
	AC \ - \ AC \ COM \ NSH \ - \ LAD \ START \ - ;

	.org (3 << 3)      	; LDA OPRD
	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
	- \ - \ - \ - \ - \ - \ - \ - \ READ  ;
	INP \ - \ AC \ TSF \ NSH \ - \ LAD \ START \ - ;

	.org (4 << 3)      	; ADI OPRD
	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ  ;
	INP \ AC \ AC \ ADD \ NSH \ - \ LAD \ START \ - ;

	.org (5 << 3)      	; LDA ADRS
	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ ;
	-  \ - \ - \ - \ - \ - \ - \ - \ DEREF ;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ ;
	INP \ - \ AC \ TSF \ NSH \ - \ LAD \ START \ - ;

	.org (6 << 3)      	; STA ADRS
	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ ;
	-  \ - \ - \ - \ - \ - \ - \ - \ DEREF ;
	AC \ - \ - \ TSF \ NSH \ - \ - \ - \ WRITE ;
	AC \ - \ -\ - \ - \ - \ LAD \ START \ - ;
	
	.org(7<<3)          ; NOP
	-  \ - \ - \ - \ - \ - \ - \ - \ -;
	- \ - \ - \ - \ - \ - \ LAD \ START \ - ;
    
    	.org( 8<<3)          ;BEQ
     	AC \ - \ AC \ TSF \ NSH \ INT \ LZ \ BE_Q \ - ;	
	- \ - \ - \ - \ - \ - \ LAD \ START \ - ;
BE_Q:	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ;
	INP \ - \ PC \- \ - \ - \ LAD \ START \ - ;
	 
	.org(9<<3)            ;BMI
	- \ - \ - \ - \ - \ INT \ LS \ BM_I \ - ;
	- \ - \ - \ - \ - \ - \ LAD \ START \ - ;
BM_I:	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ ;
	INP \ - \ PC \- \ - \ - \ LAD \ START \ - ;
	 
	.org(10<<3)         ;JMP
	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ ;
	INP \ - \ PC \- \ - \ - \ LAD \ START \ - ;
	 
	.org(11<<3)         ;SBC Data
	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ  ;
	AC \ INP \ AC \ SUB \ NSH \ - \ LAD \ START \ - ;

	.org (12<<3)           ;JSR addr
	SP \ - \ - \ TSF \ NSH \ - \ - \ - \ LDMAR ;
	PC \ - \ - \ INC \ NSH \ - \ - \ - \ WRITE ;
	PC \ - \ - \ INC \ NSH \ - \ - \ - \ LDMAR ;
        - \ - \ - \ - \ - \ - \ - \ - \ READ ;
        INP \ - \ PC \ TSF \ NSH \ - \ NEXT \ - \ - ;
        SP \ - \ SP \ INC \ NSH \ - \ LAD \ START \ - ;

	.org(13<<3)		;RTS
	SP \ - \ SP \ DEC \ NSH \ - \ NEXT \ - \ - ;
        SP \ - \ - \ - \ - \ - \ - \ - \ LDMAR ;
        - \ - \ - \ - \ - \ - \ - \ - \ READ ;
        INP \ - \ PC \ - \ - \ - \ LAD \ START \ - ;
	
	.org(14<<3)          ; LDX Data
	PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
	-  \ - \ - \ - \ - \ - \ - \ - \ READ ;
	INP \ - \ X \ TSF \ NSH \ - \ LAD \ START \ - ;

	.org (15 << 3)	; INX
	X \ - \ X \ INC \ NSH \ - \ LAD \ START \ - ;
                      
        .org(16<<3)         ;ADC Address X
        PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
        -  \ - \ - \ - \ - \ - \ - \ - \ READ ;
        INP \ -\ R3 \ - \ - \ - \ NEXT \ - \ - ;
        R3 \ X \ R4 \ ADD \ NSH \ - \ NEXT \ - \ - ;
        R4 \ - \ -\ TSF \ NSH \ - \ - \ - \ LDMAR ;
        -  \ - \ - \ - \ - \ - \ - \ - \ READ ;
        INP \ AC \ AC \ ADD \ NSH \ - \ LAD \ START \ - ;        	  
	 
        .org(17<<3)         ;SBC Address X
        PC \ - \ PC \ INC \ NSH \ - \ - \ - \ LDMAR ;
        -  \ - \ - \ - \ - \ - \ - \ - \ READ  ;
        INP \ - \ R3 \ - \ - \ - \ NEXT \ - \ - ;
        R3 \ X \ R4 \ ADD \ NSH \ - \NEXT \ - \ - ;
        R4 \ - \ -\ TSF \ NSH \ - \ - \ - \ LDMAR ;
        -  \ - \ - \ - \ - \ - \ - \ - \ READ  ;
        AC \ INP \ AC \ SUB \ NSH \ - \LAD \ START \ - ;        	  

	.org(18<<3)	;PHA
	SP \ - \ - \ - \ - \ - \ - \ - \ LDMAR ;
	AC \ - \ - \ TSF \ NSH \ - \ - \ - \ WRITE ;
	SP \ - \ SP \ INC \ NSH \ - \ LAD \ START \ - ;

	.org(19<<3) 		;PLA
	SP \ - \ SP \ DEC \ NSH \ - \ NEXT \ - \ - ;
	SP \ - \ - \ - \ - \ - \ - \ - \ LDMAR ;
	- \ - \ - \ - \ - \ - \ - \ - \ READ ;
	INP \ - \ AC \ TSF \ NSH \ - \ LAD \ START \ - ;

	.org ((1<<DEPTH)-1)	; Ensure at least 8 address bits
	-  \ - \ - \ - \ - \ - \ - \ -   \ -     ;
.end ;

