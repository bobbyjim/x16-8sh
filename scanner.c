
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"
//#include "list.h"
#include "opcodes.h"

#include "debug.h"

//extern uint8_t program[];
//extern uint8_t *bytecode;

//
// We don't want to be able to change the input source, 
// so we should declare these to be const.
//
const char* scanner_start;
const char* scanner_current;
uint8_t     scanner_line;

static int isDigit(char c) {
  return c >= '0' && c <= '9';
}

static int isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || c == '_';
}

static int isDot(char c) {
   return c == '.';
}

static int isAtEnd() {
  return *scanner_current == '\0';
}

static char advance() {
  ++scanner_current;
  return scanner_current[-1];
}

static char peek() {
  return *scanner_current;
}

static char peekNext() {
  if (isAtEnd()) return '\0';
  return scanner_current[1];
}

static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*scanner_current != expected) return false;

  ++scanner_current;
  return true;
}

static void skipWhitespace() 
{
  char c;
  for (;;) {
    c = peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;

      case '\n':
         ++scanner_line;
         advance();
         break;

      case '/': // comments
         if (peekNext() == '/') {
            while(peek() != '\n' && !isAtEnd()) advance();
         } else {
            return;
         }
         break;

      default:
        return;
    }
  }
}

/************************************************************

 		Token builders

*************************************************************/
static Token* makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner_start;
  token.len = (uint8_t)((uint16_t)scanner_current - (uint16_t)scanner_start);
  
  printf("make_token(): start=%u, len= %u\n", token.start, token.len);
  debugToken(&token, "make_token()");

//  token.line = scanner_line;

  return &token;
}

static Token* errorToken(const char* message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.len = (uint8_t)strlen(message);
//  token.line = line;

  return &token;
}

static TokenType checkKeyword(int start, int length,
    const char* rest, TokenType type) {
  if (scanner_current - scanner_start == start + length &&
      memcmp(scanner_start + start, rest, length) == 0) {

    return type;
  }

  return TOKEN_IDENTIFIER;
}

static TokenType identifierType()
{ 
   switch (scanner_start[0]) {
//    case 'a': 
//    case 'b': 
//    case 'c': 
//    case 'd': 
      case 'e': return checkKeyword(1, 3, "cho", TOKEN_ELSE);
      case 'f': return checkKeyword(1, 2, "or",  TOKEN_FOR);
//    case 'g':
//    case 'h': 
      case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
//    case 'j':
//    case 'k':
//    case 'l':
//    case 'm':
      case 'n': return checkKeyword(1, 3, "ull", TOKEN_NIL);
//    case 'o': 
      case 'p': return checkKeyword(1, 1, "r", TOKEN_ECHO); // pr = print = echo
//    case 'q':
      case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
      case 's': return checkKeyword(1, 2, "ub", TOKEN_SUB);
//    case 't': 
//    case 'u':
//    case 'v': 
      case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
//    case 'x': 
//    case 'y':
//    case 'z':
  }
  return TOKEN_IDENTIFIER;
}

static Token* identifier() {
  while (isAlpha(peek()) || isDigit(peek()) || isDot(peek())) advance();

  return makeToken(identifierType());
}

static Token* variable() {
  while (isAlpha(peek()) || isDigit(peek())) advance();

  // Look for a dot inside.
  if (peek() == '.' && (isAlpha(peekNext()) || isDigit(peekNext()))) {
     // Consume the .
     advance();

     while (isAlpha(peek()) || isDigit(peek())) advance();
  }
  
  return makeToken(TOKEN_IDENTIFIER); // VARIABLE); 
}


static Token* number() {
  while (isDigit(peek())) advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the ".".
    advance();

    while (isDigit(peek())) advance();
  }

  return makeToken(TOKEN_NUMBER);
}

static Token* string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') ++scanner_line;
    advance();
  }

  if (isAtEnd()) return errorToken("unterminated string.");

  // The closing quote.
  advance();
  return makeToken(TOKEN_STRING);
}



/************************************************************

 		Scanner API

*************************************************************/
//void initScanner(int sourceBank)
void initScanner(const char* source)
{
   scanner_start = source;
   scanner_current = source;
   scanner_line = 1;
}

Token* scanToken()
{
   char c;

   skipWhitespace();
   scanner_start = scanner_current; // after we've eaten the whitespace

   if (isAtEnd()) return makeToken(TOKEN_EOF);
   c = advance();

   if (isAlpha(c)) return identifier();
   if (isDigit(c)) return number();

   switch(c) 
   {
      // one-char tokens
      case '(': return makeToken(TOKEN_LEFT_PAREN);
      case ')': return makeToken(TOKEN_RIGHT_PAREN);
//      case ':': return makeToken(TOKEN_COLON);
      case ';': return makeToken(TOKEN_SEMICOLON);
      case ',': return makeToken(TOKEN_COMMA);

      case '-': return makeToken(TOKEN_MINUS);
      case '+': return makeToken(TOKEN_PLUS);
      case '*': return makeToken(TOKEN_STAR);
      case '/': return makeToken(TOKEN_SLASH);

      case '.': return makeToken(TOKEN_DOT);


      // potential two-char tokens
      case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL  : TOKEN_BANG);
      case '=': return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
      case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL  : TOKEN_LESS);
      case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
//    case '-': return makeToken(match('=') ? TOKEN_MINUS_EQUAL : TOKEN_MINUS );
//    case '+': return makeToken(match('=') ? TOKEN_PLUS_EQUAL  : TOKEN_PLUS);
//    case '*': return makeToken(match('=') ? TOKEN_STAR_EQUAL  : TOKEN_STAR );
//    case '/': return makeToken(match('=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH );

      case '|': return makeToken(match('|') ? TOKEN_OR     : TOKEN_OR ); // PIPE );
      case '&': return makeToken(match('&') ? TOKEN_AND    : TOKEN_AND ); // AMP );
//      case '.': return makeToken(match('.') ? TOKEN_DOTDOT : TOKEN_DOT );

      // strings
      case '"': return string();
      case '\'': return string();

      // variables
      case '$': if (isAlpha(peek())) return variable();
   }

   return errorToken("unexpected character.");
}

