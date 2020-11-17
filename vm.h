#ifndef _vm_h
#define _vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256


typedef struct {
   Chunk* chunk;
   uint8_t* ip;
   Value stack[STACK_MAX];
   Value* stackTop;
} VM;

#define InterpretResult uint8_t
#define INTERPRET_OK 			0
#define INTERPRET_COMPILE_ERROR		1
#define INTERPRET_RUNTIME_ERROR		2

void initVM();
void freeVM();
InterpretResult interpretChunk(Chunk* chunk);
InterpretResult interpret(int bank); // const char* source);
void push(Value value);
Value pop();

#endif
