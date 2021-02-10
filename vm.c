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

#include <stdio.h>
#include <string.h>
#include <cbm.h>
#include <peekpoke.h>
#include <time.h>

#include "common.h"
#include "compiler.h"
#include "vm.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "hash.h"

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
   vm.objects = NULL;
   initHash(&vm.globals);
   adjustCapacity(&vm.globals, 16);
   initHash(&vm.internedStrings);
   adjustCapacity(&vm.internedStrings, 16);

   // set some constants
   setNil(&NIL_OBJ);
   TRUE_OBJ.as.boolean = true;
   setBool(&TRUE_OBJ);
   setBool(&FALSE_OBJ);
}

void freeVM() {
   puts("free-vm()\n");
   freeHash(&vm.globals);
   freeHash(&vm.internedStrings);
   freeObjects();
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
  return vm.stackTop-1; // without popping
}

Value* peek(int distance)
{
   return &vm.stackTop[-1 - distance];
}

static bool isFalsey(Value* value)
{
   return IS_NIL(*value) || (IS_BOOL(*value) && !AS_BOOL(*value));
}

static void concatenate() {
  ObjString* b = AS_STRING(*pop());
  ObjString* a = AS_STRING(*pop());
  ObjString* result;
  Value obj;

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  result = takeString(chars, length);

  //push(OBJ_VAL(result));
  obj.type = VAL_OBJ;
  obj.as.obj = (Obj*) result;
  push(&obj);
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
#define READ_STRING()   AS_STRING(*READ_CONSTANT())

#define LOGICAL_OP(op) \
    do { \
      Value* b = pop(); \
      Value* a = top(); \
      a->as.boolean = a->as.number op b->as.number; \
      a->type = VAL_BOOL; \
    } while (false)

#define BINARY_OP(op) \
    do { \
      Value* b = pop(); \
      Value* a = top(); \
      a->as.number = a->as.number op b->as.number; \
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

      case OP_POP:      pop(); break;

      case OP_GETTIME:
      {
        Value timeVal;
        time_t t;
        time(&t);
        timeVal.type = VAL_NUMBER;
        timeVal.as.number = t;
        push(&timeVal);
        break;
      }

      case OP_GET_GLOBAL:
      {
         ObjString* name = READ_STRING();
         Value* value;

         //
         //  An improvised "hashGet()"
         //
	 Entry* entries = vm.globals.entries;
	 int capacity   = vm.globals.capacity;
         Entry* found = findEntry(entries, capacity, name);
         if ( found->key != NULL )
         {
            printf("vm - entry value is %s\n", AS_CSTRING(found->value));
            value = &found->value;
         }

/*
         if (!hashGet(&vm.globals, name, value)) 
         {
            runtimeError("undefined variable '%s'.", name->chars);
            return INTERPRET_RUNTIME_ERROR;
         }
         else
         {
            printf("vm - value for %s is %s\n", name->chars, AS_CSTRING(*value));
         }
*/
         push(value);
         break;
      }

      case OP_DEFINE_GLOBAL: 
      {
	 ObjString* name = READ_STRING();
	 hashSet(&vm.globals, name, peek(0));
         pop();
         hashDump(&vm.globals);
         break;
      }

      case OP_EQUAL:    
      {
         Value* b = pop();
         Value* a = top();
	 a->as.boolean = valuesEqual(a,b);
         a->type = VAL_BOOL;
         break;
      }

      case OP_GREATER:  LOGICAL_OP(>); break;
      case OP_LESS:     LOGICAL_OP(<); break;

      case OP_ADD:      BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE:   BINARY_OP(/); break;
      case OP_MODULO:   BINARY_OP(%); break;
      case OP_POW:      
      {
         Value* b = pop();
         Value* a = top();
         long   n = b->as.number;
         long   m = a->as.number;
         while(--n>0) a->as.number *= m;
         break;
      }

      case OP_NOT:
      {
         Value* v = top();
         printf("value = %d,%d\n", v->type, v->as.boolean);
         v->as.boolean = isFalsey(v);
         v->type = VAL_BOOL;
         printf("value = %d,%d\n", v->type, v->as.boolean);
         break;
      }

      case OP_NEGATE:   
      {
         Value* v = top();
         v->as.number = -v->as.number;
	 break;
      }

/*
      case OP_MODULO: 
      {
         Value* b = pop();
         Value* a = top();
         a->as.number = a->as.number % b->as.number;
         break;
      }
*/

      case OP_CAT:
      {
        if (IS_STRING(*peek(0)) && IS_STRING(*peek(1))) {
          concatenate();
        } else {
          runtimeError(
              "Operands must be two strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }

      case OP_PRINT:
      {
         printValue(pop());
         printf("\n");
         break;
      }

      case OP_RETURN: 
      {
         // Exit interpeter.
         return INTERPRET_OK;
      }

    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP(op)
#undef LOGICAL_OP(op)
}

InterpretResult interpretChunk(Chunk* chunk) 
{
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

void initInterpreter()
{
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

void freeInterpreter()
{
}


