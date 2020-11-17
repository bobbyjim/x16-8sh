
#include <stdio.h>

#include "debug.h"
#include "value.h"
#include "scanner.h"

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
                               int offset) {
  uint8_t constant = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
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
    case OP_CONSTANT:
      return constantInstruction("op_constant", chunk, offset);

    case OP_DUMP:
      return simpleInstruction("op_dump", offset);

    case OP_ADD:
      return simpleInstruction("op_add", offset);
    case OP_SUBTRACT:
      return simpleInstruction("op_subtract", offset);
    case OP_MULTIPLY:
      return simpleInstruction("op_multiply", offset);
    case OP_DIVIDE:
      return simpleInstruction("op_divide", offset);
    case OP_MODULO:
      return simpleInstruction("op_modulo", offset);

    case OP_NEGATE:
      return simpleInstruction("op_negate", offset);
    case OP_RETURN:
      return simpleInstruction("op_return", offset);
    default:
      printf("Unknown opcode %d\n", instruction);
      return offset + 1;
  }
}



void debugToken(Token* tok, char* msg)
{
   char *name;

   switch(tok->type)
   {
      case TOKEN_MINUS         : name = "minus";      break;
      case TOKEN_PLUS          : name = "plus" ;      break;
//    case TOKEN_POW           : name = "pow"  ;      break;
      case TOKEN_SLASH         : name = "slash";      break;
      case TOKEN_STAR          : name = "star";       break;
//    case TOKEN_XOR           : name = "xor";        break; 
//    case TOKEN_PIPE          : name = "pipe";       break;
//    case TOKEN_AMP           : name = "ampersand";  break;

      case TOKEN_LEFT_PAREN    : name = "left paren"; break;
      case TOKEN_RIGHT_PAREN   : name = "rt paren";   break;
      case TOKEN_COMMA         : name = "comma";      break;
      case TOKEN_DOT           : name = "dot";        break;   
//    case TOKEN_COLON         : name = "colon";      break;  
      case TOKEN_SEMICOLON     : name = "semicolon";  break;  
      case TOKEN_LEFT_BRACE    : name = "left brace";  break;
      case TOKEN_RIGHT_BRACE   : name = "right brace"; break;

      case TOKEN_BANG          : name = "bang";        break;
      case TOKEN_BANG_EQUAL    : name = "bang equal";  break;
      case TOKEN_EQUAL         : name = "assign";       break;
      case TOKEN_EQUAL_EQUAL   : name = "equal equal"; break;
      case TOKEN_GREATER       : name = "greater";     break;
      case TOKEN_GREATER_EQUAL : name = "greater-eq";  break;
      case TOKEN_LESS          : name = "less";        break;
      case TOKEN_LESS_EQUAL    : name = "less-equal";  break;

//    case TOKEN_SEQ           : name = "string eq";  break;
//    case TOKEN_SNE           : name = "string ne";  break;

      case TOKEN_IDENTIFIER    : name = "identifier";  break;
      case TOKEN_STRING        : name = "string";      break;
      case TOKEN_NUMBER        : name = "number";      break;
//    case TOKEN_VARIABLE      : name = "variable";    break;

      case TOKEN_AND           : name = "and";  break;
      case TOKEN_OR            : name = "or";  break;
//    case TOKEN_DOTDOT        : name = "two dots";  break;
      case TOKEN_ECHO          : name = "echo";  break;
      case TOKEN_FOR           : name = "for";  break;
      case TOKEN_IF            : name = "if";  break;
      case TOKEN_NIL           : name = "nil";  break;
      case TOKEN_RETURN        : name = "return";  break;
      case TOKEN_SUB           : name = "sub";  break;
      case TOKEN_WHILE         : name = "while";  break;

      case TOKEN_EOF           : name = "eof";  break;
      case TOKEN_ERROR         : name = "error";  break;
      default                  : name = "n/a"; break;
   }

   printf("token %2u %-11s   :%s  ^%u (%u)\n", tok->type, name, msg, tok->start, tok->len);
/*
   printf("   - type : %u\n", tok->type);
   printf("   - name : %s\n", name);
   printf("   - start: %u\n", tok->start);
   printf("   - len  : %u\n", tok->len);
   printf("\n");
*/
}

void debugPrecedence(uint8_t p, char* msg)
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
      default   : name = sprintf("level %u", p); break;
   }
   printf("%10s :%s\n", name, msg);
}

