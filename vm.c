
#include <stdio.h>
#include <string.h>
#include <cbm.h>
#include <peekpoke.h>

#include "common.h"
#include "compiler.h"
#include "vm.h"
#include "debug.h"

VM vm;

Value NIL_OBJ;
Value TRUE_OBJ;
Value FALSE_OBJ;


static void resetStack() {
  vm.stackTop = vm.stack;
}

static void runtimeError(const char* format, ...)
{
  va_list args;
  size_t instruction;
  int line;

  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  puts("\n");

  instruction = vm.ip - vm.chunk->code - 1;
  line = vm.chunk->lines[instruction];
  printf("[line %d] in script\n", line);

  resetStack();
}


void initVM() {
   resetStack();

   // set some constants
   setNil(&NIL_OBJ);
   TRUE_OBJ.as.boolean = true;
   setBool(&TRUE_OBJ);
   setBool(&FALSE_OBJ);
}

void freeVM() {
}

void push(Value* value) {
  *vm.stackTop = *value;
  vm.stackTop++;
}

Value* pop() {
  vm.stackTop--;
  return vm.stackTop; // was *vm.stackTop
}

Value* top() {
  return vm.stackTop; // without popping
}

Value* peek(int distance)
{
   return &vm.stackTop[-1 - distance];
}

static bool isFalsey(Value* value)
{
   return IS_NIL(*value) || (IS_BOOL(*value) && !AS_BOOL(*value));
}

////////////////////////////////////////////////////////////
//
//
//  "This is the single most important function, by far."
//
//
////////////////////////////////////////////////////////////
static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (&(vm.chunk->constants.values[READ_BYTE()]))

#define LOGICAL_OP(op) \
    do { \
      Value* b = pop(); \
      Value* a = pop(); \
      a->as.boolean = a->as.number op b->as.number; \
      a->type = VAL_BOOL; \
      push(a); \
    } while (false)

#define BINARY_OP(op) \
    do { \
      Value* b = pop(); \
      Value* a = pop(); \
      a->as.number = a->as.number op b->as.number; \
      push(a); \
    } while (false)


  for (;;) {
    uint8_t instruction;
    Value* slot;

#ifdef DEBUG_TRACE_EXECUTION
    printf("                      [");
    for (slot = vm.stack; slot < vm.stackTop; slot++) {
      if (slot > vm.stack) printf(", ");
      //printf("[");
      printValue(slot);
      //printf("]");
    }
    printf("]\n");

    disassembleInstruction(vm.chunk,
                           (int)(vm.ip - vm.chunk->code));
#endif

    switch (instruction = READ_BYTE()) {

      case OP_CONSTANT: 
      {
        Value* constant = READ_CONSTANT();
        push(constant);
        break;
      }

      case OP_NIL:      push(&NIL_OBJ); break;
      case OP_TRUE: 	push(&TRUE_OBJ); break;
      case OP_FALSE:	push(&FALSE_OBJ); break;

      case OP_EQUAL:    
      {
         Value* b = pop();
         Value* a = pop();
	 a->as.boolean = valuesEqual(a,b);
         a->type = VAL_BOOL;
         push(a);
         break;
      }

      case OP_GREATER:  LOGICAL_OP(>); break;
      case OP_LESS:     LOGICAL_OP(<); break;

      case OP_ADD:      BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE:   BINARY_OP(/); break;

      case OP_NOT:
      {
         Value* v = pop();
         printf("value = %d,%d\n", v->type, v->as.boolean);
         v->as.boolean = isFalsey(v);
         v->type = VAL_BOOL;
         printf("value = %d,%d\n", v->type, v->as.boolean);
         push(v);
         break;
      }

      case OP_NEGATE:   
      {
         Value* v = pop();
         v->as.number = -v->as.number;
         push(v);
	 break;
      }

      case OP_MODULO: 
      {
         Value* b = pop();
         Value* a = top();
         int bb = b->as.number;
         int aa = a->as.number;
         while(aa > bb) aa -= bb;
         a->as.number = aa;
         break;
      }

      case OP_RETURN: 
      {
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      }

    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP(op)
#undef LOGICAL_OP(op)
}

InterpretResult interpretChunk(Chunk* chunk) 
{
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

InterpretResult interpret(uint8_t sourcebank, uint8_t tokenbank)
{
   Chunk chunk;
   InterpretResult result;

   initChunk(&chunk);

   if (!compile(sourcebank, tokenbank, &chunk)) // compiler.c
   {
      freeChunk(&chunk);  
      return INTERPRET_COMPILE_ERROR;
   }

   vm.chunk = &chunk;
   vm.ip = vm.chunk->code;

   result = run();
   freeChunk(&chunk);
   return result;
}

