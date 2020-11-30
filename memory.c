#include <stdlib.h>
#include <stddef.h>

#include "vm.h"
#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  void* result;

  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  result = realloc(pointer, newSize);
  if (result == NULL) exit(1);
  return result;
}

static void freeObject(Obj* object) {
  switch (object->type) {
    case OBJ_STRING: {
      ObjString* string = (ObjString*)object;
      FREE_ARRAY(char, string->chars, string->length + 1);
      FREE(ObjString, object);
      break;
    }
  }
}

void freeObjects() {
  Obj* object = vm.objects;
  while (object != NULL) {
    Obj* next = object->next;
    freeObject(object);
    object = next;
  }
}
