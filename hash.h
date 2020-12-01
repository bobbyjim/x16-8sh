#ifndef _hash_h
#define _hash_h

#include "common.h"
#include "value.h"

typedef struct {
  ObjString* key;
  Value value;
} Entry;

typedef struct {
   int count;
   int capacity;
   Entry* entries;
} Hash;


void initHash(Hash* hash);
void freeHash(Hash* hash);
bool hashGet(Hash* table, ObjString* key, Value* value);
bool hashSet(Hash* table, ObjString* key, Value* value);
bool hashDelete(Hash* table, ObjString* key);
void hashAddAll(Hash* from, Hash* to);
ObjString* hashFindString(Hash* table, const char* chars, int length, uint32_t hash);

#endif
