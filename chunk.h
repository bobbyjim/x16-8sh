#ifndef clox_chunk_h
#define clox_chunk_h

#include <stdint.h>

#include "value.h"

typedef enum {
  OP_NULL,
  OP_DUMP,

  OP_RETURN,
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_MODULO,
  OP_NEGATE,


  OP_GET,
  OP_PUT,
  OP_PUSH,
  OP_POP,
  OP_RM,
  OP_ECHO,
  OP_JUMP,
  OP_JNE,
  OP_LOOP,
  OP_CALL,
  OP_SUB,
  OP_APPEND,
  OP_WRITE,
  OP_DO,
  OP_WHILE,

  OP_REL,
  OP_EQ,
  OP_OROR,
  OP_ANDAND,
  OP_POW,
  OP_INC,
  OP_DEC
} OpCode;

typedef struct {
   uint8_t count;
   uint8_t capacity;
   uint8_t* code;
   uint8_t* lines;
   ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, uint8_t line);
int addConstant(Chunk* chunk, Value value);


//
//  This is a mishmash of Perl3's perly.h and Lox's chunk.h
//

/*
#define OP_CONSTANT 	0x00
#define OP_GET      	0x00
#define OP_NULL     	0x00
#define OP_POP      	0x00
#define OP_PUSH     	0x00
#define OP_PUT      	0x00

#define OP_RM		0x00

#define OP_ECHO     	0x00
#define OP_JUMP     	0x00
#define OP_JNE      	0x00

#define OP_LOOP     	0x00
#define OP_CALL     	0x00
#define OP_RETURN   	0x00
#define OP_FUN      	0x00

#define OP_APPEND   	0x00
#define OP_WRITE	0x00
//
//  no OP_OPEN or OP_CLOSE for now
//

#define OP_DO		0x00
#define OP_WHILE	0x00


// "RELOP" is relational, e.g. < <= => > gt lt ge le 
// "EQOP"  is equality, e.g. == != eq ne
// "ADDOP" includes subtraction
// "MULOP" includes division
#define OP_REL		0x00 
#define OP_EQ		0x00
#define OP_ADD		0x00
#define OP_MUL		0x00
#define OP_OROR		0x00
#define OP_ANDAND	0x00

#define OP_POW		0x00
#define OP_INC		0x00
#define OP_DEC		0x00

// "left shift" <
#define OP_LS		0x00
// "right shift" >
#define OP_RS		0x00
*/

/*
typedef union {
    int ival;
    char *cval;
    double dval;
} YYSTYPE;
*/

#endif
