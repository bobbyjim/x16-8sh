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

#ifndef _scanner_h
#define _scanner_h

#include <stdint.h>

#define TokenType uint8_t

typedef struct {
//   const char* start; // not working. why?
   TokenType type;
   int       start_position; // BANK OFFSET (NOT A POINTER ANY MORE)
   uint8_t   length;
   int       line;
} Token;

//void tokenize(char* buffer);
void initScanner(uint8_t sourceBank, uint8_t tokenBank);

//Token* scanToken();
TokenType scanToken();

//void scan(Token* intoThisToken);
char* getInputFrom(uint8_t position);


void scanAll(uint8_t frombank, uint8_t tobank);


#define PETSCII_LEFT_ARROW	95
#define PETSCII_PIPE    	221
#define	PETSCII_SPADE		193
#define	PETSCII_HEART		211
#define PETSCII_DIAMOND		218
#define PETSCII_CLUB		216


#define MAX_TABLE_TOKEN_VALUE   64

#define TOKEN_LEFT_PAREN        1
#define TOKEN_RIGHT_PAREN       2
#define TOKEN_LEFT_BRACE        3
#define TOKEN_RIGHT_BRACE       4
#define TOKEN_COMMA             5
#define TOKEN_DOT               6
#define TOKEN_MINUS             7
#define TOKEN_PLUS              8
#define TOKEN_SEMICOLON         9
#define TOKEN_SLASH             10

#define TOKEN_STAR              11
#define TOKEN_BANG              12
#define TOKEN_BANG_EQUAL        13
#define TOKEN_EQUAL             14
#define TOKEN_EQUAL_EQUAL       15
#define TOKEN_GREATER           16
#define TOKEN_GREATER_EQUAL     17
#define TOKEN_LESS              18
#define TOKEN_LESS_EQUAL        19
#define TOKEN_IDENTIFIER        20

#define TOKEN_STRING            21
#define TOKEN_NUMBER            22
#define TOKEN_AND               23
#define TOKEN_ELSE              24
#define TOKEN_OR                25
#define TOKEN_PRINT             26
#define TOKEN_RETURN            27
#define TOKEN_FOR               28
#define TOKEN_FUN               29
#define TOKEN_IF                30

#define TOKEN_NIL               31
#define TOKEN_VAR               32
#define TOKEN_WHILE             33
#define TOKEN_DOT_DOT           34
#define TOKEN_MINUS_EQUAL       35
#define TOKEN_PLUS_EQUAL        36
#define TOKEN_COLON             37
#define TOKEN_UNUSED11111111111 38
#define TOKEN_PIPE		39
#define TOKEN_STAR_EQUAL	40

#define TOKEN_SLASH_EQUAL	41
#define TOKEN_GETTIME		42
#define TOKEN_OF 		43
#define TOKEN_UP		44
#define TOKEN_AMP               45
#define TOKEN_LEFT_ARROW	46
#define TOKEN_BLOCK             47
#define TOKEN_S_NE		48
#define TOKEN_S_GT		49
#define TOKEN_S_GTE		50

#define TOKEN_S_EQ		51
#define TOKEN_S_LTE		52
#define TOKEN_S_LT		53
#define TOKEN_DOTDOT            54
#define TOKEN_ENDBLOCK          55
#define TOKEN_TRUE		56
#define TOKEN_FALSE		57
#define TOKEN_MOD		58
#define TOKEN_DOLLAR		59
#define TOKEN_SIZE_OF_VAR	60

#define TOKEN_REGISTER		61
#define TOKEN_UNTIL		62
#define TOKEN_UNUSED3333333333	63
#define TOKEN_UNUSED2222222222	64

// --------------------------------

#define TOKEN_X16		66
#define TOKEN_EOF               67

#define TOKEN_ERROR_START			224
#define TOKEN_ERROR_UNTERMINATED_STRING		224
#define TOKEN_ERROR_UNEXPECTED_CHAR		225
#define TOKEN_ERROR_EXPECT_END_PAREN            226
#define TOKEN_ERROR_EXPECT_END_OF_EXPR          227
#define TOKEN_ERROR_TOO_MANY_CONSTANTS          228
#define TOKEN_ERROR_EXPRESSION_EXPECTED         229
#define TOKEN_ERROR_SEMICOLON_EXPECTED		230
#define TOKEN_ERROR_EXPECT_VARIABLE_NAME	231

#define TOKEN_ERROR_HALT_CATCH_FIRE		255

#endif
