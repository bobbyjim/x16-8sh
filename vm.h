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

#ifndef _vm_h
#define _vm_h

#include "chunk.h"
#include "value.h"
#include "hash.h"
#include "object.h"

#define STACK_MAX 256

//
//  Next to go.  Nuff said.
//
typedef struct {
   Chunk* chunk;
   uint8_t* ip;
   Value stack[STACK_MAX];
   Value* stackTop;
   Obj* objects;
   Hash globals;
   Hash internedStrings; // interned string hashtable
} VM;


#define InterpretResult uint8_t
#define INTERPRET_OK 			0
#define INTERPRET_COMPILE_ERROR		1
#define INTERPRET_RUNTIME_ERROR		2

extern VM vm;

void initVM();
void freeVM();

void initInterpreter();
void freeInterpreter();

InterpretResult interpretChunk(Chunk* chunk);
InterpretResult interpret(uint8_t sourcebank, uint8_t tokenbank);

void push(Value* value);
Value* pop();

#endif
