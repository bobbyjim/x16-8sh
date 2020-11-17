#include <stdlib.h>
#include <stddef.h>

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
