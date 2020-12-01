#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "hash.h"
#include "value.h"


////////////////////////////////////////////
//
//  This is in percentages.  E.G. 75 = 75%
//
////////////////////////////////////////////
#define TABLE_MAX_LOAD 75

void initHash(Hash* table) 
{
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

void freeHash(Hash* table)
{
   FREE_ARRAY(Entry, table->entries, table->capacity);
   initHash(table);
}

static Entry* findEntry(Entry* entries, int capacity,
                        ObjString* key) {
  uint32_t index = key->hash % capacity;
  Entry* tombstone = NULL;

  for (;;) {
    Entry* entry = &entries[index];

    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        // Empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // We found a tombstone.
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (entry->key == key) {
      // We found the key.
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

bool hashGet(Hash* table, ObjString* key, Value* value)
{
  Entry* entry;
  if (table->count == 0) return false;

  entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;

}

static void adjustCapacity(Hash* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  int i;
  for (i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value.type = VAL_NIL;
  }

  table->count = 0;
  for (i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    Entry* dest;

    if (entry->key == NULL) continue;

    dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    ++(table->count);
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}

bool hashSet(Hash* table, ObjString* key, Value* value)
{
   Entry* entry;
   bool isNewKey;

   if (100 * (table->count + 1) > table->capacity * TABLE_MAX_LOAD)
   {
      int capacity = GROW_CAPACITY(table->capacity);
      adjustCapacity(table, capacity);
   }

   entry = findEntry(table->entries, table->capacity, key);

   isNewKey = (entry->key == NULL);
   if (isNewKey && IS_NIL(entry->value)) table->count++;

   entry->key = key;
   entry->value = *value;
   return isNewKey;
}

bool hashDelete(Hash* table, ObjString* key)
{
   Entry* entry;
   if (table->count == 0) return false;

   // Find the entry.
   entry = findEntry(table->entries, table->capacity, key);
   if (entry->key == NULL) return false;

   // Place a tombstone in the entry.
   entry->key = NULL;
   entry->value.type = VAL_BOOL;   // BOOL_VAL(true);
   entry->value.as.boolean = true;

   return true;
}

void hashAddAll(Hash* from, Hash* to)
{
  int i;
  for (i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->key != NULL) {
      hashSet(to, entry->key, &(entry->value));
    }
  }
}

ObjString* hashFindString(Hash* table, const char* chars, int length, uint32_t hash)
{
   int index;
   if (table->count == 0) return NULL;
   
   index = hash % table->capacity;
   
   for(;;)
   {
      Entry* entry = &table->entries[index];

      if (entry->key == NULL) {
         // Stop if we find an empty non-tombstone entry.
         if (IS_NIL(entry->value)) return NULL;
      } 
      else if (entry->key->length == length 
           &&  entry->key->hash == hash 
           &&  memcmp(entry->key->chars, chars, length) == 0) 
      {
         // We found it.
         return entry->key;
      }
      index = (index + 1) % table->capacity;
    }
}

