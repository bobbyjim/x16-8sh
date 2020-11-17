//
//  Ideas stolen from 
//  http://c-jump.com/CIS77/CPU/IsaDesign/lecture.html
//
//
//
#ifndef _opcodes_h
#define _opcodes_h

typedef struct {
        int op: 3;
        int reg: 2;
        int mod: 3;
} optype;

#define	OP_SPECIAL		0
#define OP_OR 			1
#define OP_AND			2
#define OP_CMP  		3
#define OP_SUB 			4
#define OP_ADD			5
#define OP_LOD 			6
#define OP_STO			7

#define REG_A 			0	// LOD into these
#define REG_B 			1
#define REG_C 			2 
#define REG_D 			3 

#define MEM_A 			0	// STO into these
#define MEM_B 			1 
#define MEM_C 			2 
#define MEM_D 			3 
#define MEM_BIND		4  
#define MEM_BOFFSIND		5  
#define MEM_OFFSIND		6  
#define MEM_CONSTANT		7  

#define SPECIAL_ZOP		0	// 000 Opcodes
#define SPECIAL_JMP		1
#define SPECIAL_NOT		2 
#define SPECIAL_ILLEGAL		3 

#define ZOP_NOP     		0	// Zero Operand Instructions
#define ZOP_CALLA    		1  	// Syscall to address in A
#define ZOP_ILLEGAL2		2 
#define ZOP_HCF			3 	// Halt, Catch Fire
#define ZOP_ILLEGAL4		4  
#define ZOP_ILLEGAL5		5  
#define ZOP_PUTREG		6       // display register values
#define ZOP_PUTSTR		7       // prints string referenced from A (null terminated)

#define JEQ			0	// Special Jumps
#define JNE			1
#define JLT			2
#define JLE			3
#define JGT			4
#define JGE			5
#define JMP			6
#define JUMP_ILLEGAL		7

#define SUCCESS                 0
#define ERROR_UNKNOWN_OPCODE    1

#endif
