
#include <stdio.h>

#include "debug.h"
#include "value.h"
#include "scanner.h"
#include "vm.h"
#include "bank.h"


void testVM()
{
//   int twelve;
//   int five;
//   int constant;
   Chunk chunk;

   initVM();

   initChunk(&chunk);
/*
   twelve = addConstant(&chunk, 12);
   writeChunk(&chunk, OP_CONSTANT, 123);
   writeChunk(&chunk, twelve, 123);

   // ---

   constant = addConstant(&chunk, 3);
   writeChunk(&chunk, OP_CONSTANT, 123);
   writeChunk(&chunk, constant, 123);

   writeChunk(&chunk, OP_ADD, 123);

   // ---

   five = addConstant(&chunk, 5);
   writeChunk(&chunk, OP_CONSTANT, 123);
   writeChunk(&chunk, five, 123);

   writeChunk(&chunk, OP_DIVIDE, 123);

   // ---

   constant = addConstant(&chunk, 2);
   writeChunk(&chunk, OP_CONSTANT, 123);
   writeChunk(&chunk, constant, 123 );

   writeChunk(&chunk, OP_MODULO, 123);

   writeChunk(&chunk, OP_NEGATE, 123);
*/
   writeChunk(&chunk, OP_RETURN, 123);

//   disassembleChunk(&chunk, "test chunk");
   interpretChunk(&chunk);
   freeVM();
   freeChunk(&chunk);
}


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
    case OP_EQUAL: 	return simpleInstruction("op_equal", offset);
    case OP_GREATER: 	return simpleInstruction("op_greater", offset);
    case OP_LESS: 	return simpleInstruction("op_less", offset);

    case OP_CONSTANT: 	return constantInstruction("op_constant", chunk, offset);
    case OP_ADD: 	return simpleInstruction("op_add", offset);
    case OP_SUBTRACT: 	return simpleInstruction("op_subtract", offset);
    case OP_MULTIPLY: 	return simpleInstruction("op_multiply", offset);
    case OP_DIVIDE: 	return simpleInstruction("op_divide", offset);
    case OP_MODULO: 	return simpleInstruction("op_modulo", offset);

    case OP_NOT:        return simpleInstruction("op_not", offset);
    case OP_NEGATE: 	return simpleInstruction("op_negate", offset);

    case OP_CAT:	return simpleInstruction("op_catenate", offset);
    case OP_PRINT:      return simpleInstruction("op_print", offset);
    case OP_RETURN: 	return simpleInstruction("op_return", offset);
    default:
      printf("unknown opcode %d\n", instruction);
      return offset + 1;
  }
}



char* debugToken(TokenType type)
{
   char *name;

   switch(type)
   {
      case TOKEN_NIL	       : name = "nil";		break;
      case TOKEN_TRUE	       : name = "true";         break;
      case TOKEN_FALSE	       : name = "false"; 	break;

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
      case TOKEN_PRINT         : name = "echo";  break;
      case TOKEN_FOR           : name = "for";  break;
      case TOKEN_IF            : name = "if";  break;
      case TOKEN_RETURN        : name = "return";  break;
      case TOKEN_SUB           : name = "sub";  break;
      case TOKEN_WHILE         : name = "while";  break;

      case TOKEN_EOF           : name = "eof";  break;
      case TOKEN_ERROR_UNTERMINATED_STRING  : name = "error: unterminated string";  break;
      case TOKEN_ERROR_UNEXPECTED_CHAR      : name = "error: unexpected char";  break;
      case TOKEN_ERROR_EXPECT_END_PAREN     : name = "error: ')' expected"; break;
      case TOKEN_ERROR_EXPECT_END_OF_EXPR   : name = "error: expected end of expression"; break;
      case TOKEN_ERROR_TOO_MANY_CONSTANTS   : name = "error: too many constants"; break;
      case TOKEN_ERROR_EXPRESSION_EXPECTED  : name = "error: expression expected"; break;
      case TOKEN_ERROR_SEMICOLON_EXPECTED   : name = "error: ';' expected"; break;

      case TOKEN_ERROR_HALT_CATCH_FIRE      : name = "error: halt and catch fire"; break;

      default                  : name = "n/a"; break;
   }

   return name;
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

