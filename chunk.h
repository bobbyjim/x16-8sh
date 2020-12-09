/*
  
    8SH: a command interpreter for 8 bit 'retro' systems.
    Copyright (C) 2020 Robert Eaglestone

    This file is part of 8SH.

    8SH is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    8SH is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 8SH.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef clox_chunk_h
#define clox_chunk_h

#include <stdint.h>

#include "value.h"

typedef enum {
  OP_NIL,
  OP_FALSE,
  OP_TRUE,

  OP_RETURN,
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_MODULO,
  OP_NEGATE,
  OP_NOT,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,

  OP_CAT,

  OP_GET,
  OP_PUT,
  OP_PUSH,
  OP_POP,
  OP_GET_GLOBAL,
  OP_DEFINE_GLOBAL,

  OP_GETTIME,

  OP_RM,
  OP_PRINT,
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
uint8_t addConstant(Chunk* chunk, Value* value);


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

#define OP_PRINT     	0x00
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
