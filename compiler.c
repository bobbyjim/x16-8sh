#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "chunk.h"

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

Token* parser_current;
Token* parser_previous;
bool parser_hadError;
bool parser_panicMode;

//
//  Pratt Parser Data
//
void    (*prefixRule[MAX_TOKEN_VALUE]) ();
void    (*infixRule[MAX_TOKEN_VALUE]) ();
uint8_t precedenceRule[MAX_TOKEN_VALUE];

Chunk* compilingChunk;

//
//   Forward declarations.
//
static void expression();
static uint8_t getRuleNum(TokenType type);
static void parsePrecedence(uint8_t precedence);


static Chunk* currentChunk() {
  return compilingChunk;
}

static void errorAt(Token* token, const char* message) {

  if (parser_panicMode) return;
  parser_panicMode = true;

//  fprintf(stderr, "[line %u] error", token->line);
  fprintf(stderr, "error_at(): [line ??] error");

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token->len, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser_hadError = true;
}

static void error(const char* message) {
  errorAt(parser_previous, message);
}

static void errorAtCurrent(const char* message) {
  errorAt(parser_current, message);
}

static void advance() 
{
   printf("\ncompiler:advance()\n");
   printf(" : \n");

   parser_previous = parser_current;

   for (;;) {
      parser_current = scanToken();

      debugToken(parser_current,  "advance() current/2");
      debugToken(parser_previous, "advance() previous/2");

      if (parser_current->type != TOKEN_ERROR) break;

      errorAtCurrent(parser_current->start);
   }
}

static void consume(TokenType type, const char* message) {
  if (parser_current->type == type) {
    advance();
    return;
  }

  errorAtCurrent(message);
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

static uint8_t makeConstant(Value value) {
  int constant = addConstant(currentChunk(), value);
  if (constant > UINT8_MAX) {
    error("too many constants in one chunk.");
    return 0;
  }

  return (uint8_t)constant;
}

static void emitConstant(Value value) {
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

  // TODO: fetch precedence using rulenum, etc
  parsePrecedence(rule_precedence + 1);

  // Emit the operator instruction.
  switch (operatorType) {
    case TOKEN_PLUS:          emitByte(OP_ADD); break;
    case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
    case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
    case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
    default:
      return; // Unreachable.
  }
}

static void number() {
  int value = atoi(parser_previous->start);
  printf("   converted to int: %s\n", parser_previous->start);
  emitConstant(value);
}

static void grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void unary() {
  TokenType operatorType;
  operatorType = parser_previous->type;

  // Compile the operand.
  parsePrecedence(PREC_UNARY);

  // Emit the operator instruction.
  switch (operatorType) {
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
   ParseRule(TOKEN_DOT          ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_MINUS        ,unary,   binary,PREC_TERM);
   ParseRule(TOKEN_PLUS         ,NULL,    binary,PREC_TERM);
   ParseRule(TOKEN_SEMICOLON    ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_SLASH        ,NULL,    binary,PREC_FACTOR);
   ParseRule(TOKEN_STAR         ,NULL,    binary,PREC_FACTOR);
   ParseRule(TOKEN_BANG         ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_BANG_EQUAL   ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_EQUAL        ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_EQUAL_EQUAL  ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_GREATER      ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_GREATER_EQUAL,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_LESS         ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_LESS_EQUAL   ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_IDENTIFIER   ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_STRING       ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_NUMBER       ,number,  NULL,  PREC_NONE);
   ParseRule(TOKEN_AND          ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_ELSE         ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_FOR          ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_SUB          ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_IF           ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_NIL          ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_OR           ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_ECHO         ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_RETURN       ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_WHILE        ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_ERROR        ,NULL,    NULL,  PREC_NONE);
   ParseRule(TOKEN_EOF          ,NULL,    NULL,  PREC_NONE);

   prefixRule[TOKEN_NUMBER] = number; 
       infixRule[TOKEN_NUMBER] = NULL; 
       precedenceRule[TOKEN_NUMBER] = PREC_NONE;
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
   debugPrecedence(precedence, "current precedence");
   advance();

   if ( prefixRule[parser_previous->type] == NULL )
   {
      error("expression expected.");
      return;
   }
   printf("calling prefix rule %04x on prev\n", prefixRule[parser_previous->type]);
   (prefixRule[parser_previous->type])();

   printf("parse_precedence: precedence_rule[current]=%u\n", precedenceRule[precedence]);
   while(precedence <= precedenceRule[precedence])
   {
      printf("parse_precedence: calling advance()");
      advance();

      printf("parse_precedence: calling infixRule()");
      (infixRule[parser_previous->type])();
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




bool compile(const char* source, Chunk* chunk)
//bool compile(int sourceBank, Chunk* chunk)
{
   initPrattTable();

   initScanner(source); // in scanner.c
//   initScanner(sourceBank); // in scanner.c
   compilingChunk = chunk;
 
   parser_hadError = false; 
   parser_panicMode = false;

   //
   //  advance() "primes the pump" on the scanner.
   //
   advance();

   expression();

   consume(TOKEN_EOF, "expect end of expression.");

   endCompiler();
   return !parser_hadError;
}

