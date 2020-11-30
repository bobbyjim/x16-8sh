#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include "bank.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  object->next = vm.objects;
  vm.objects = object;
  return object;
}

static ObjString* allocateString(char* chars, int length) {
  ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  //printf("allocated string len: %d, chars: %s\n", length, chars);
  return string;
}

ObjString* takeString(char* chars, int length) {
  return allocateString(chars, length);
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
  //printf("getting banked string at pos %d length %d\n", start_position + 1, length);
  bankgets(heapChars, length, start_position);
  heapChars[length] = '\0';
  //printf("copy-string: %s\n", heapChars);
  return allocateString(heapChars, length);
}

void printObject(Value* value) {
  //printf("value type: %d, obj type: %d\n", value->type, value->as.obj->type);
  switch (OBJ_TYPE(*value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(*value));
      break;
  }
}
