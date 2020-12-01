#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include "bank.h"
#include "hash.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

Value nilValue;

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  object->next = vm.objects;
  vm.objects = object;
  return object;
}

static ObjString* allocateString(char* chars, int length, uint32_t hash) {
  ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = length;
  string->chars  = chars;
  string->hash   = hash;

  nilValue.type = VAL_NIL;
  hashSet(&vm.internedStrings, string, &nilValue);
  //printf("allocated string len: %d, chars: %s\n", length, chars);
  return string;
}

//
//  FNV-1a: "the shortest decent hash function I know"
//
static uint32_t hashString(const char* key, int length) {
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

/*
ObjString* copyString(const char* chars, int length) 
{
  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return allocateString(heapChars, length);
}
*/
ObjString* copyString(int start_position, uint8_t length)
{
  char* heapChars = ALLOCATE(char, length + 1);
  uint32_t hash   = hashString(heapChars, length);
  ObjString* interned = hashFindString(&vm.internedStrings, heapChars, length, hash);
  if (interned != NULL) return interned;

  //printf("getting banked string at pos %d length %d\n", start_position + 1, length);
  bankgets(heapChars, length, start_position);
  heapChars[length] = '\0';
  //printf("copy-string: %s\n", heapChars);
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
