#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "chunk.h"
#include "bank.h"
#include "value.h"
#include "object.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

#define  PREC_NONE         0
#define  PREC_ASSIGNMENT   1
#define  PREC_OR           2
#define  PREC_AND          3
#define  PREC_EQUALITY     4
#define  PREC_COMPARISON   5
#define  PREC_TERM         6
#define  PREC_FACTOR       7
#define  PREC_UNARY        8
#define  PREC_CALL         9
#define  PREC_PRIMARY      10


uint8_t compiler_source_bank;
uint8_t compiler_token_bank;

Token  tmp1, tmp2;
Token* parser_current;
Token* parser_previous;

int    token_current_read_position;
int    compiler_current_token;

bool parser_hadError;
bool parser_panicMode;

char compilerTempBuffer[256];

//
//  Pratt Parser Data
//
void    (*prefixRule[MAX_TABLE_TOKEN_VALUE]) ();
void    (*infixRule[MAX_TABLE_TOKEN_VALUE]) ();
uint8_t precedenceRule[MAX_TABLE_TOKEN_VALUE];

Chunk* compilingChunk;

//
//   Forward declarations.
//
static void expression();
static uint8_t getRuleNum(TokenType type);
static void parsePrecedence(uint8_t precedence);


void copyToTempBuffer(uint8_t startPosition, uint8_t length)
{
   while(--length)
   {
      compilerTempBuffer[length] = beek(startPosition+length);
   }
}


char* translateErrorCode(uint8_t errorCode)
{
   switch( errorCode )
   {
      case TOKEN_ERROR_UNTERMINATED_STRING: return "unterminated string";
      case TOKEN_ERROR_UNEXPECTED_CHAR    : return "unexpected character";
      case TOKEN_ERROR_EXPECT_END_PAREN   : return "expect ')' after expression";
      case TOKEN_ERROR_TOO_MANY_CONSTANTS : return "too many constants in one chunk.";

      default: return "unknown error";
   }
}

static Chunk* currentChunk() {
  return compilingChunk;
}

static void errorAt(Token* token, uint8_t errorCode) {

  char* message = translateErrorCode( errorCode );
  if (parser_panicMode) return;
  parser_panicMode = true;

  printf("[line %u] error", token->line);
//  fprintf(stderr, "error_at(): [line ??] error");

  if (token->type == TOKEN_EOF) {
    printf(" at end");
  } else if (token->type >= TOKEN_ERROR_START) {
    // Nothing.
  } else {
//    fprintf(stderr, " at '%.*s'", token->length, getInputFrom(token->start_position));

     //copyToTempBuffer(token->start_position, token->length);
     //printf(" at '%s'", compilerTempBuffer);
 
     printf(" at pos(%d) len(%d)\n", token->start_position, token->length);
  }

  printf(": %s\n", message);
  parser_hadError = true;
}

static void error(uint8_t errorCode) {
  errorAt(parser_previous, errorCode);
}

static void errorAtCurrent(uint8_t errorCode) {
  errorAt(parser_current, errorCode);
}

//
//  Reads the token from token_current_read_position
//  into *parser_current;
//
void readToken()
{
   setBank(compiler_token_bank);

   parser_current->type   = bankgetbyte(token_current_read_position);
   ++token_current_read_position;

   parser_current->length = bankgetbyte(token_current_read_position);
   ++token_current_read_position;

   parser_current->start_position = bankgetint(token_current_read_position);
   token_current_read_position += 2;

   parser_current->line = bankgetint(token_current_read_position);
   token_current_read_position += 2;

   ++compiler_current_token;
}

static void advance() 
{
   *parser_previous = *parser_current; // copy contents

   for (;;) {
      //parser_current = scanToken(); 
      readToken();

      if (parser_current->type < TOKEN_ERROR_START) break;

      errorAtCurrent( parser_current->type );
   }
}

static void consume(TokenType type, uint8_t errorCode) {
  if (parser_current->type == type) {
    advance();
    return;
  }

  errorAtCurrent(errorCode);
}

static void emitByte(uint8_t byte) {
  writeChunk(currentChunk(), byte, -1); // parser_previous.line);

  // disassembleChunk...
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}

static void emitReturn() {
  emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value* value) {
  int constant = addConstant(currentChunk(), value);
  if (constant > UINT8_MAX) {
    error(TOKEN_ERROR_TOO_MANY_CONSTANTS); // "too many constants in one chunk.");
    return 0;
  }

  return (uint8_t)constant;
}

static void emitConstant(Value* value) {
  printf("   - emit constant: %d\n", value->as.number);
  emitBytes(OP_CONSTANT, makeConstant(value));
}

static void endCompiler() {
  emitReturn();

#ifdef DEBUG_PRINT_CODE
   if (!parser_hadError) {
      disassembleChunk(currentChunk(), "code");
   }
#endif
}

static void binary() {
  // Remember the operator.
  TokenType operatorType = parser_previous->type;

  // Compile the right operand.
  uint8_t  ruleNum = getRuleNum(operatorType);
  uint8_t  rule_precedence = precedenceRule[ ruleNum ];

  printf("   - binary(): calling parse-precedence(%d)\n", rule_precedence + 1);
  parsePrecedence(rule_precedence + 1);

  // Emit the operator instruction.
  switch (operatorType) {
    case TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
    case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
    case TOKEN_GREATER:       emitByte(OP_GREATER); break;
    case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
    case TOKEN_LESS:          emitByte(OP_LESS); break;
    case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;

    case TOKEN_PLUS:          emitByte(OP_ADD); break;
    case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;

    case TOKEN_DOT:           emitByte(OP_CAT); break; // string catenation
    default:
      return; // Unreachable.
  }
}

static void literal() {
  switch (parser_previous->type) {
    case TOKEN_FALSE: emitByte(OP_FALSE); break;
    case TOKEN_NIL: emitByte(OP_NIL); break;
    case TOKEN_TRUE: emitByte(OP_TRUE); break;
    default:
      return; // Unreachable.
  }
}

static void number() {
  int value;
  Value rval;
  setBank(compiler_source_bank);
  bankgets(compilerTempBuffer, parser_previous->length, parser_previous->start_position);
  value = atoi(compilerTempBuffer);
/*
  printf("\n converted (%s) at (%d) with length (%u) to int (%d)\n", 
     compilerTempBuffer, 
     parser_previous->start_position,
     parser_previous->length,
     value);
*/
  rval.as.number = value;
  rval.type      = VAL_NUMBER;
  emitConstant(&rval); // was: value
}

static void string() {
   Obj* str;
   Value val; // this doesn't feel kosher, but it works
   setBank(compiler_source_bank);

   str = (Obj*) copyString(parser_previous->start_position + 1,
                           parser_previous->length - 2);

   //printf("object type: %d\n", str->type);
   val = *objVal(str);
   //printf("value type: %d\n", val.type);
   //printf("value->obj type: %d\n", val.as.obj->type);
   printObject(&val);
   emitConstant(&val);
}


static void grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, TOKEN_ERROR_EXPECT_END_PAREN);
}

static void unary() {
  TokenType operatorType;
  operatorType = parser_previous->type;

  // Compile the operand.
  parsePrecedence(PREC_UNARY);

  // Emit the operator instruction.
  switch (operatorType) {
    case TOKEN_BANG : emitByte(OP_NOT);    break;
    case TOKEN_MINUS: emitByte(OP_NEGATE); break;
    default:
      return; // Unreachable.
  }
}

//
//  Pratt Parser data.
//
// typedef    uint8_t ParseRuleNum;
// ParseFn    prefix[MAX_TOKEN_VALUE];
// ParseFn    infix[MAX_TOKEN_VALUE];
// Precedence precedence[MAX_TOKEN_VALUE];
//
// prefixRule[ num ] = NULL | grouping | unary | number
// infixRule[  num ] = NULL | binary
// precedence[ num ] = PREC_NONE | PREC_FACTOR
//
#define ParseRule(n,pf,if,prec) prefixRule[n]=pf;infixRule[n]=if;precedenceRule[n]=prec
void initPrattTable()
{
   ParseRule(TOKEN_LEFT_PAREN,grouping,NULL,PREC_NONE);
   ParseRule(TOKEN_RIGHT_PAREN  ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_LEFT_BRACE   ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_RIGHT_BRACE  ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_COMMA        ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_DOT          ,NULL,    binary,PREC_TERM);
   ParseRule(TOKEN_MINUS        ,unary,   binary,PREC_TERM);
   ParseRule(TOKEN_PLUS         ,NULL,    binary,PREC_TERM);
   ParseRule(TOKEN_SEMICOLON    ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_SLASH        ,NULL,    binary,PREC_FACTOR);
   ParseRule(TOKEN_STAR         ,NULL,    binary,PREC_FACTOR);
   ParseRule(TOKEN_BANG         ,unary,   NULL,  PREC_NONE);
   ParseRule(TOKEN_BANG_EQUAL   ,NULL,    binary,  PREC_EQUALITY);
   ParseRule(TOKEN_EQUAL        ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_EQUAL_EQUAL  ,NULL,    binary,  PREC_EQUALITY);
   ParseRule(TOKEN_GREATER      ,NULL,    binary,  PREC_EQUALITY);
   ParseRule(TOKEN_GREATER_EQUAL,NULL,    binary,  PREC_EQUALITY);
   ParseRule(TOKEN_LESS         ,NULL,    binary,  PREC_EQUALITY);
   ParseRule(TOKEN_LESS_EQUAL   ,NULL,    binary,  PREC_EQUALITY);
   ParseRule(TOKEN_IDENTIFIER   ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_STRING       ,string,  NULL,  PREC_NONE);
   ParseRule(TOKEN_NUMBER       ,number,  NULL,  PREC_NONE);
   ParseRule(TOKEN_AND          ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_ELSE         ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_FALSE        ,literal, NULL,  PREC_NONE);
   ParseRule(TOKEN_FOR          ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_SUB          ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_TRUE         ,literal, NULL,  PREC_NONE);
   ParseRule(TOKEN_IF           ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_NIL          ,literal, NULL,  PREC_NONE);
   ParseRule(TOKEN_OR           ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_ECHO         ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_RETURN       ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_WHILE        ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_EOF          ,NULL,    NULL,  PREC_NONE);

/*
   prefixRule[TOKEN_NUMBER] = number; 
       infixRule[TOKEN_NUMBER] = NULL; 
       precedenceRule[TOKEN_NUMBER] = PREC_NONE;
*/
}

void debugPrattTable()
{
   int i = 64;
   while(--i)
   {
      printf("%2d %04x %04x %04x\n", i, prefixRule[i], infixRule[i], precedenceRule[i]);
   }
}

static void parsePrecedence(uint8_t precedence) 
{
   printf("\r\nparse-precedence: %d (%s)\n", precedence, debugPrecedence(precedence));
   advance();

   printf("   - prev (%s) %d\n", debugToken(parser_previous->type), precedenceRule[parser_previous->type]);
   printf("   - curr (%s) %d\n", debugToken(parser_current->type), precedenceRule[parser_current->type]);
   printf("\n");

   //
   //  The prefix rule MUST exist.  Why?
   //
   if ( parser_previous->type >= TOKEN_ERROR_START )  // take care of all the error tokens
   {                                                  // (which DO NOT have Pratt entries!)
      return;					      //
   }					              //
   if ( prefixRule[parser_previous->type] == NULL )
   {
      error(TOKEN_ERROR_EXPRESSION_EXPECTED); // "expression expected.");
      return;
   }

//   printf("   - prefix rule (prev) : %u\n", prefixRule[parser_previous->type]);
   (prefixRule[parser_previous->type])();
//   printf("\n");

   //
   //  If we're at a lower precedence, then advance and call 
   //  the infix rule on the previous token, if present.
   //
/*
   printf("   - curr precedence %d %s\n", 
		precedenceRule[parser_current->type], 
		debugPrecedence(precedenceRule[parser_current->type]));
*/
   while(precedence <= precedenceRule[parser_current->type])
   {
//      printf("   - infix: calling advance()\n");
      advance();

      if ( infixRule[parser_previous->type] )
      {
//         printf("   - infix: calling infix-rule(%d)\n", infixRule[parser_previous->type]);
         (infixRule[parser_previous->type])();
      }
   }
}

static uint8_t getRuleNum(TokenType type) {
   return type; 
}

//
//  this is the thing which connects the parser with the code generator
//
static void expression() {
   parsePrecedence(PREC_ASSIGNMENT);
}

bool compile(uint8_t sourceBank, uint8_t tokenBank, Chunk* chunk)
{
   scanAll(sourceBank, tokenBank);     // in scanner.c

   initPrattTable();

   compilingChunk = chunk;
 
   parser_hadError = false; 
   parser_panicMode = false;

   compiler_source_bank = sourceBank;
   compiler_token_bank  = tokenBank;
   token_current_read_position = 0;
   compiler_current_token = 0;
   parser_current = &tmp1;
   parser_previous = &tmp2;

   //
   //  advance() "primes the pump" on the scanner.
   //
   advance();

   expression();

   consume(TOKEN_EOF, TOKEN_ERROR_EXPECT_END_OF_EXPR);

   endCompiler();
   return !parser_hadError;
}

