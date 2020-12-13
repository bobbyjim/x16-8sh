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

#include "debug.h"
#include "value.h"


void disassembleChunk(Chunk* chunk, const char* name) {
  int offset;
  printf("== %s ==\n", name);

  for (offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}

static int simpleInstruction(const char* name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk,
                               int offset) 
{
  Value val;
  uint8_t constant = chunk->code[offset + 1];
  val = chunk->constants.values[constant];
  printf("%-16s %4d '", name, constant);
  printValue(&val);
  printf("'\n");
  return offset + 2;
}

int disassembleInstruction(Chunk* chunk, int offset) {
  uint8_t instruction;
  printf("%04d ", offset);

  if (offset > 0 &&
      chunk->lines[offset] == chunk->lines[offset - 1]) {
    printf("   | ");
  } else {
    printf("%4d ", chunk->lines[offset]);
  }

  instruction = chunk->code[offset];
  switch (instruction) {
    case OP_NIL:  	return simpleInstruction("op_nil", offset);
    case OP_TRUE: 	return simpleInstruction("op_true", offset);
    case OP_FALSE: 	return simpleInstruction("op_false", offset);
    case OP_POP:        return simpleInstruction("op_pop", offset);
    case OP_GET_GLOBAL: return constantInstruction("op_get_global", chunk, offset);
    case OP_DEFINE_GLOBAL: return constantInstruction("op_define_global", chunk, offset);
    case OP_EQUAL: 	return simpleInstruction("op_equal", offset);
    case OP_GREATER: 	return simpleInstruction("op_greater", offset);
    case OP_LESS: 	return simpleInstruction("op_less", offset);

    case OP_CONSTANT: 	return constantInstruction("op_constant", chunk, offset);
    case OP_ADD: 	return simpleInstruction("op_add", offset);
    case OP_SUBTRACT: 	return simpleInstruction("op_subtract", offset);
    case OP_MULTIPLY: 	return simpleInstruction("op_multiply", offset);
    case OP_DIVIDE: 	return simpleInstruction("op_divide", offset);
    case OP_MODULO: 	return simpleInstruction("op_modulo", offset);
    case OP_POW:        return simpleInstruction("op_pow", offset);

    case OP_NOT:        return simpleInstruction("op_not", offset);
    case OP_NEGATE: 	return simpleInstruction("op_negate", offset);

    case OP_GETTIME:    return simpleInstruction("op_time", offset);
    case OP_CAT:	return simpleInstruction("op_catenate", offset);
    case OP_PRINT:      return simpleInstruction("op_print", offset);
    case OP_RETURN: 	return simpleInstruction("op_return", offset);
    default:
      printf("unknown opcode %d\n", instruction);
      return offset + 1;
  }
}

char* debugPrecedence(uint8_t p)
{
   char *name = "n/a";
   switch(p)
   {
      case  0   : name = "none"; break;
      case  1   : name = "assignment"; break;
      case  2   : name = "or"; break;
      case  3   : name = "and"; break;
      case  4   : name = "equality"; break;
      case  5   : name = "comparison"; break;
      case  6   : name = "term"; break;
      case  7   : name = "factor"; break;
      case  8   : name = "unary"; break;
      case  9   : name = "call"; break;
      case  10  : name = "primary"; break;
      default   : name = "something else"; break;
   }
   return name;
}

