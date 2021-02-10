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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"
#include "opcodes.h"
#include "bank.h"

#include "debug.h"

uint8_t source_bank;
int     scanner_start_position;    // 
int     scanner_current_position;  // 
int     scanner_line;              // 

uint8_t token_bank;
int     token_rw_head_position;    //  Current token input/output position.
int     token_count;               //  Total # of tokens.

uint8_t token_type;                //  Token workspace.
int     token_start_position;      //  Offset in Bank.
uint8_t token_length;              //
int     token_line;                //  Is >256 realistic?

char scannerTempBuffer[256];       //  Scratch buffer.

static int isAlnumdot(char c) {
   return isalnum(c) || c == '.' || c == 164; // 164 is PETSCII '_'
}

static int isAtEnd() {
   return beek(scanner_current_position) == '\0';
}

static char advance() {
  ++scanner_current_position;
   return beek(scanner_current_position-1);
}

static bool match(char expected) {
  if (isAtEnd()) return false;
  if (beek(scanner_current_position) != expected) return false;

  ++scanner_current_position;
  return true;
}

static char matchNotAlnum() {
  char c;

  if (isAtEnd()) return 0;

  c = beek(scanner_current_position);
  if (isalnum(c)) return 0;
   
  ++scanner_current_position;
  return c;
}

static void skipWhitespace() 
{
  char c;
  for (;;) {
    c = beek(scanner_current_position);
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
      case 160 :  // this is a strange PETSCII space
        advance();
        break;

      case '\n':
         ++scanner_line;
         advance();
         break;

      case '#': // "#" comment
         while(beek(scanner_current_position) != '\n' && !isAtEnd()) advance();
         break;

      case '/': // "//" comment
         if (beeknext(scanner_current_position) == '/') {
            while(beek(scanner_current_position) != '\n' && !isAtEnd()) advance();
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
TokenType makeToken(TokenType type)
{
  token_type = type;
  token_start_position = scanner_start_position;
  token_length = scanner_current_position - scanner_start_position;
  token_line = scanner_line;
  return token_type; 
}

TokenType errorToken(TokenType errorTokenType)
{
  token_type = errorTokenType;
  token_start_position = 0;
  token_length = 0;
  token_line = scanner_line;
  return token_type;
}

static TokenType checkKeyword(
		int start, 
		int length,
    		const char* rest, 
		TokenType type)
{
  if (scanner_current_position - scanner_start_position == start + length 
   && memcmpBank(scanner_start_position+start,rest,length) == 0) {
    return type;
  }
  return TOKEN_IDENTIFIER;
}

static TokenType checkRelatedKeywords(
	int start, 
	int length, 
	const char* rest,
	TokenType type,
	TokenType typeMinusOne)
{
  if (scanner_current_position - scanner_start_position == start + length 
	&& memcmpBank(scanner_start_position+start,rest,length) == 0) {
    return type;
  }
  else 
  if (scanner_current_position - scanner_start_position == start + length - 1
	&& memcmpBank(scanner_start_position+start,rest,length-1) == 0) {
    return typeMinusOne;
  }

  return TOKEN_IDENTIFIER;
}

TokenType string(char terminator) {
  while (beek(scanner_current_position) != terminator && !isAtEnd()) {
    if (beek(scanner_current_position) == '\n') ++scanner_line;
    advance();
  }

  if (isAtEnd()) /*return*/ errorToken(TOKEN_ERROR_UNTERMINATED_STRING);

  // The closing quote.
  advance();
  return makeToken(TOKEN_STRING);
}

static TokenType identifierType()
{ 
   char c;
   switch (beek(scanner_start_position)) 
   {
      case 'b': return checkKeyword(1, 4, "reak",  TOKEN_BREAK);
      case 'd': return checkKeyword(1, 3, "one",   TOKEN_ENDBLOCK); // "done"
      case 'e': 
	if (checkKeyword(1, 3, "lse",   TOKEN_ELSE) == TOKEN_ELSE)
	   return TOKEN_ELSE;
  	return checkKeyword(1, 1, "q", TOKEN_S_EQ);

      case 'f': 
	if (checkKeyword(1, 2, "or",    TOKEN_FOR) == TOKEN_FOR)
	   return TOKEN_FOR;
        return checkKeyword(1, 4, "alse",  TOKEN_FALSE);
	
      case 'g': return checkRelatedKeywords(1, 2, "te", TOKEN_S_GTE, TOKEN_S_GT);
      case 'i': return checkKeyword(1, 1, "f",     TOKEN_IF);
      case 'l': return checkRelatedKeywords(1, 2, "te", TOKEN_S_LTE, TOKEN_S_LT);
      
      case 'n': 
	if (checkKeyword(1, 2, "il",    TOKEN_NIL) == TOKEN_NIL)
	   return TOKEN_NIL;
	return checkKeyword(1, 1, "e", TOKEN_S_NE);

      case 'o': return checkKeyword(1, 1, "f",     TOKEN_OF);

      case 'q': c = matchNotAlnum();
            if (c != 0)
	       return string(c);
	    else
	       return TOKEN_IDENTIFIER;

      case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
      case 's': return checkKeyword(1, 2, "ub",    TOKEN_FUN);
      case 't': 
	if (checkKeyword(1, 3, "rue",   TOKEN_TRUE) == TOKEN_TRUE)
	   return TOKEN_TRUE;
	return checkKeyword(1, 3, "ime", TOKEN_GETTIME);

      case 'u': return checkKeyword(1, 4, "ntil",  TOKEN_UNTIL);
      case 'v': return checkKeyword(1, 2, "ar",    TOKEN_VAR);
      case 'w': return checkKeyword(1, 4, "hile",  TOKEN_WHILE);
  }
  return TOKEN_IDENTIFIER;
}

TokenType identifier() {
  while (isAlnumdot(beek(scanner_current_position))) advance();

  return makeToken(identifierType());
}

TokenType dollarVariable() {
  char c = advance(); // advance to first character, which we've already checked.
  c = advance(); // now advance to next character.
  while (isAlnumdot(beek(scanner_current_position))) advance();

  return makeToken(TOKEN_IDENTIFIER);
}

TokenType arraySize() {
   char c = advance(); // eat #
   c = advance();      // to first character.
   while (isAlnumdot(beek(scanner_current_position))) advance();

   return makeToken(TOKEN_SIZE_OF_VAR);
}


TokenType number() {
  while (isdigit(beek(scanner_current_position))) advance();

  // Look for a fractional part.
  if (beek(scanner_current_position) == '.' && isdigit(beeknext(scanner_current_position))) {
    // Consume the ".".
    advance();

    while (isdigit(beek(scanner_current_position))) advance();
  }

  return makeToken(TOKEN_NUMBER);
}

/************************************************************

 		Scanner API

*************************************************************/
void initScanner(uint8_t sourceBank, uint8_t tokenBank)
{
   source_bank             = sourceBank;
   scanner_start_position   = 0; 
   scanner_current_position = 0;
   scanner_line             = 1;

   token_bank               = tokenBank;
   token_rw_head_position   = 0;
   token_count              = 0;
   token_type               = 0;
   token_start_position     = 0;
   token_length             = 0;
   token_line               = 0;
}

TokenType scanToken()
{
   char c;

   setBank(source_bank); // just in case

   skipWhitespace();
   scanner_start_position = scanner_current_position; 

   if (isAtEnd())
   {
      //printf("found token_eof\n");
      return makeToken(TOKEN_EOF);
   }

   c = advance();

   if (isalpha(c)) return identifier();
   if (isdigit(c)) return number();

   switch(c) 
   {
      // one-char tokens
      case '(': return makeToken(TOKEN_LEFT_PAREN);
      case ')': return makeToken(TOKEN_RIGHT_PAREN);
      case ':': return makeToken(TOKEN_COLON);
      case ';': return makeToken(TOKEN_SEMICOLON);
      case ',': return makeToken(TOKEN_COMMA);
      case '?': return makeToken(TOKEN_PRINT);
      case '^': return makeToken(TOKEN_UP);
      case '%': return makeToken(TOKEN_MOD);
      case PETSCII_LEFT_ARROW: return makeToken(TOKEN_EQUAL);

      // strings
      case '"':  return string('"');
      case '\'': return string('\'');

      // potential two-char tokens
      case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL  	: TOKEN_BANG	);
      case '=': return makeToken(match('=') ? TOKEN_EQUAL_EQUAL 	: TOKEN_EQUAL	);
      case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL  	: TOKEN_LESS	);
      case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL 	: TOKEN_GREATER	);
      case '-': return makeToken(match('=') ? TOKEN_MINUS_EQUAL 	: TOKEN_MINUS 	);
      case '+': return makeToken(match('=') ? TOKEN_PLUS_EQUAL  	: TOKEN_PLUS	);
      case '*': return makeToken(match('=') ? TOKEN_STAR_EQUAL 		: TOKEN_STAR	);
      case '/': return makeToken(match('=') ? TOKEN_SLASH_EQUAL 	: TOKEN_SLASH 	);
      case '&': return makeToken(match('&') ? TOKEN_AND    		: TOKEN_AMP   	);
      case '.': return makeToken(match('.') ? TOKEN_DOTDOT 		: TOKEN_DOT   	);
      case PETSCII_PIPE: return makeToken(match(PETSCII_PIPE)? TOKEN_OR : TOKEN_PIPE  	);

      // registers
      case PETSCII_SPADE:	return makeToken( TOKEN_REGISTER );
      case PETSCII_HEART:	return makeToken( TOKEN_REGISTER );
      case PETSCII_CLUB:	return makeToken( TOKEN_REGISTER );
      case PETSCII_DIAMOND:	return makeToken( TOKEN_REGISTER );

      // variables
      case '$': 
	if (match('#')) {
	   return arraySize();
        }
        else if (isalpha(beeknext(scanner_current_position))) return dollarVariable();
        return makeToken(TOKEN_DOLLAR);
   }

   return errorToken(TOKEN_ERROR_UNEXPECTED_CHAR);
}

void copyToScannerTempBuffer(uint8_t startPosition, uint8_t length)
{
   while(--length)
   {
      scannerTempBuffer[length] = beek(startPosition+length);
   }
}

/****************************************************************************

     Write the token struct to the target bank and position.
     The token is 6 bytes long (char* data is stored in the input source).

*****************************************************************************/
void writeToken()
{
   setBank(token_bank); // just in case

   bankputbyte(token_type,token_rw_head_position);
   ++token_rw_head_position;   

   bankputbyte(token_length,token_rw_head_position);
   ++token_rw_head_position;

   bankputint(token_start_position,token_rw_head_position);
   token_rw_head_position += 2;

   bankputint(token_line,token_rw_head_position);
   token_rw_head_position += 2;

   ++token_count;
}

void scan(uint8_t frombank, uint8_t tobank)
{
   int line = -1;
   initScanner(frombank, tobank);

   for (;;) 
   {
      TokenType type = scanToken();

      if (token_line != line) 
      {
         printf("%4d ", token_line);
         line = token_line;
      }
      else
      {
         printf("   | ");
      }

      bankgets(scannerTempBuffer, token_length, token_start_position);

      printf("%-20s %15s %2d (>%d)\n", 	
	scannerTempBuffer, 
	debugToken(token_type), 
	token_type, 	
	token_rw_head_position);

      writeToken();

      if (token_type == TOKEN_EOF) break;
   }
   printf("\n");
}

