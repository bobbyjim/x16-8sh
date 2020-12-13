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

#ifndef _value_h
#define _value_h

#include <stdint.h>
#include "common.h"


/*
 *  Let's keep our options open, shall we?
 */
#define   NUMBER_TYPE		long
#define	  NUMBER_PARSE    	atol
#define   NUMBER_PRINT(val)     cprintf("%ld", val)


typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
   VAL_BOOL,
   VAL_NIL,
   VAL_NUMBER,
   VAL_OBJ
} ValueType;

typedef struct {
   ValueType type;
   union {
      NUMBER_TYPE number;
      Obj* obj;
      bool boolean;
   } as;
} Value;

#define IS_BOOL(value)		((value).type == VAL_BOOL)
#define IS_NIL(value)		((value).type == VAL_NIL)
#define IS_NUMBER(value)	((value).type == VAL_NUMBER)
#define IS_OBJ(value) 		((value).type == VAL_OBJ)

#define AS_BOOL(value)		((value).as.boolean)
#define AS_NUMBER(value)	((value).as.number)
#define AS_OBJ(value)		((value).as.obj)

Value* objVal(Obj* object);

void setBool(Value* val);
void setNil(Value* val);
void setNumber(Value* val);
void setObject(Value* obj);

//typedef int Value;

typedef struct {
   uint8_t capacity;
   uint8_t count;
   Value* values;
} ValueArray;


bool valuesEqual(Value* a, Value* b);
void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value* value);
void freeValueArray(ValueArray* array);
void printValue(Value* value);


#endif
