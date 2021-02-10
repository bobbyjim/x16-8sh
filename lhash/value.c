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

#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include "object.h"
#include "value.h"

void initValueArray(ValueArray* array) {
   array->values = NULL;
   array->capacity = 0;
   array->count = 0;
}

void writeValueArray(ValueArray* array, Value* value) {
  if (array->capacity < array->count + 1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(Value, array->values,
                               oldCapacity, array->capacity);
  }

  array->values[array->count] = *value;
  array->count++;
}

void freeValueArray(ValueArray* array) {
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}

void printValue(Value* value) {
   switch(value->type) {
	case VAL_BOOL:   cprintf(value->as.boolean ? "true" : "false"); break;
	case VAL_NIL:    cprintf("nil"); break;
	case VAL_NUMBER: NUMBER_PRINT(AS_NUMBER(*value)); break;
        case VAL_OBJ:    printObject(value); break;
   }
}

bool valuesEqual(Value* a, Value* b) 
{
  if (a->type != b->type) return false;

  switch (a->type) {
    case VAL_BOOL:   return AS_BOOL(*a) == AS_BOOL(*b);
    case VAL_NIL:    return true;
    case VAL_NUMBER: return AS_NUMBER(*a) == AS_NUMBER(*b);
    case VAL_OBJ:    return AS_OBJ(*a) == AS_OBJ(*b);
/*
    {
      ObjString* aString = AS_STRING(*a);
      ObjString* bString = AS_STRING(*b);
      return aString->length == bString->length &&
          memcmp(aString->chars, bString->chars,
                 aString->length) == 0;
    }
*/
    default:
      return false; // Unreachable.
  }
}

void setBool(Value* val)
{
   val->type = VAL_BOOL;
}

void setNil(Value* val)
{
   val->type = VAL_NIL;
}

void setNumber(Value* val)
{
   val->type = VAL_NUMBER;
}

void setObject(Value *obj)
{
   obj->type = VAL_OBJ;
}

Value* objVal(Obj* object)
{
   Value* val = malloc(sizeof(Value));
   val->type = VAL_OBJ;
   val->as.obj = object; 
   return val;
}

