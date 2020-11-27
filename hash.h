#ifndef _hash_h
#define _hash_h

#include "common.h"
#include "value.h"

typedef struct {
   int count;
   int capacity;
   Entry* entries;
} Hash;

typedef struct {
  ObjString* key;
  Value value;
} Entry;

#endif
