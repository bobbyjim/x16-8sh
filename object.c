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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include "bank.h"
#include "hash.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  object->next = vm.objects;
  vm.objects = object;
  return object;
}

/*static*/ ObjString* allocateString(char* chars, int length, uint32_t hash) {
  Value* theNilThing = getNil(); // value.h
  ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = length;
  string->chars  = chars;
  string->hash   = hash;

  hashSet(&vm.internedStrings, string, theNilThing);
  //printf("allocated string len: %d, chars: %s\n", length, chars);
  return string;
}

//
//  FNV-1a: "the shortest decent hash function I know"
//
/*static*/ uint32_t hashString(const char* key, int length) {
  int i;
  uint32_t hash = 2166136261u;

  for (i = 0; i < length; i++) {
    hash ^= key[i];
    hash *= 16777619;
  }

  return hash;
}

ObjString* takeString(char* chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString* interned = hashFindString(&vm.internedStrings, chars, length, hash);

  if (interned != NULL)
  {
     FREE_ARRAY(char, chars, length+1);
     return interned;
  }

  return allocateString(chars, length, hash);
}

//ObjString* copyString(const char* chars, int length) 
//{
  //char* heapChars = ALLOCATE(char, length + 1);
  //memcpy(heapChars, chars, length);
  //heapChars[length] = '\0';
  //return allocateString(heapChars, length);
//}

ObjString* copyString(int start_position, int length)
{
  int tmp;
  char* heapChars = ALLOCATE(char, length + 1);
  uint32_t hash   = hashString(heapChars, length);
  ObjString* interned = hashFindString(&vm.internedStrings, heapChars, length, hash);
  if (interned != NULL) return interned;

  tmp = bankgets(heapChars, length, start_position);
  if (tmp == 0) 
     printf("*** ALERT *** copy-string() bytes copied = 0. did you set the bank?\n");

  heapChars[length] = '\0';
  return allocateString(heapChars, length, hash);
}

void printObject(Value* value) {
  //printf("value type: %d, obj type: %d\n", value->type, value->as.obj->type);
  switch (OBJ_TYPE(*value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(*value));
      break;
  }
}

/*inline*/ bool isObjType(Value* value, ObjType type)
{
   return IS_OBJ(*value) && AS_OBJ(*value)->type == type;
}

