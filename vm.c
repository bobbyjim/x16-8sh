
#include <stdio.h>
#include <cbm.h>
#include <peekpoke.h>

#include "common.h"
#include "compiler.h"
#include "vm.h"
#include "debug.h"

VM vm;

static void resetStack() {
  vm.stackTop = vm.stack;
}

void initVM() {
   resetStack();
}

void freeVM() {
}

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
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
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

#define BINARY_OP(op) \
    do { \
      int b = pop(); \
      int a = pop(); \
      push(a op b); \
    } while (false)


  for (;;) {
    uint8_t instruction;
    Value* slot;

#ifdef DEBUG_TRACE_EXECUTION
    printf("                      ");
    for (slot = vm.stack; slot < vm.stackTop; slot++) {
      if (slot > vm.stack) printf(", ");
      //printf("[");
      printValue(*slot);
      //printf("]");
    }
    printf("\n");

    disassembleInstruction(vm.chunk,
                           (int)(vm.ip - vm.chunk->code));
#endif

    switch (instruction = READ_BYTE()) {

      case OP_CONSTANT: 
      {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }

      // 
      // A cheapo way to dump memory at $a000
      // 
      case OP_DUMP: 
      {
         int i = -1;
         int block = pop();
         while(++i < 256)
         {
            unsigned char c = PEEK(0xa000 + block*0x100 + i);
            printf("%02x ", c);
            if (i%16==15) puts(" ");
         }
         puts(" ");
         break;
      }

      case OP_ADD:      BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE:   BINARY_OP(/); break;

      // TODO probably faster to directly negate the stack top!
      case OP_NEGATE:   push(-pop()); break;

      case OP_MODULO: 
      {
         int b = pop();
         int a = pop();
         while(a>b) a -= b;
         push(a);
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
}

InterpretResult interpretChunk(Chunk* chunk) 
{
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

InterpretResult interpret(int sourceBank) // const char* source)
{
   Chunk chunk;
   InterpretResult result;

   initChunk(&chunk);

//   if (!compile(source, &chunk))
   if (!compile(sourceBank, &chunk))
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

