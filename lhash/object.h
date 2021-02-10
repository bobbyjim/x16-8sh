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

#ifndef _object_h
#define _object_h

#include <stdint.h>

#include "common.h"
#include "value.h"
#include "hash.h"

#define IS_OBJ_TYPE(v,t)	(IS_OBJ(v) && AS_OBJ(v)->type == type)

#define OBJ_TYPE(value)		(AS_OBJ(value)->type)
#define IS_STRING(value) 	isObjType(&value, OBJ_STRING)

#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
   OBJ_STRING
} ObjType;

struct Obj {
   ObjType type;
   struct Obj* next;
};

struct ObjString {
   Obj obj;
   int length;
   char* chars;
   uint32_t hash;
};

ObjString* str_make(const char* chars);
static ObjString* allocateString(char* chars, int length, uint32_t hash);
static uint32_t hashString(const char* key, int length);

ObjString* takeString(char* chars, int length);
//ObjString* copyString(const char* chars, int length);
ObjString* copyString(int start_position, int length);
void printObject(Value* value);
/*inline*/ bool isObjType(Value* value, ObjType type);

void freeObjects();

//
//  STUFF FROM memory.h
//

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2) 
// was: (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)
  
void* reallocate(void* pointer, size_t oldSize, size_t newSize);



#endif
