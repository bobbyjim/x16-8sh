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

static void adjustCapacity(Hash* table, int capacity);

void initHash(Hash* table) 
{
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
  adjustCapacity(table, 8);
}

void freeHash(Hash* table)
{
   FREE_ARRAY(Entry, table->entries, table->capacity);
   initHash(table);
}

static Entry* findEntry(
		Entry* entries, 
		int capacity,
		ObjString* key) {
  int index = key->hash % capacity;
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

void hashDump(Hash* table)
{
   int i;
   Entry* entries = table->entries;
   printf("\n*** hash-dump() count %d, capacity %d\n", table->count, table->capacity);
   for(i=0; i<table->count; i++)
   {
      Entry* entry = &entries[i];
      ObjString* val = (ObjString*) entry->value.as.obj;
      printf("   %s -> %s\n", entry->key->chars, val->chars);
   }
   printf("***\n");
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
  int count = 0;

  // Step 1: initialize new table.
  for (i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value.type = VAL_NIL;
  }

  // Step 2: copy over old entries.
  for (i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    Entry* dest;

    if (entry->key == NULL) continue;

    dest = findEntry(entries, capacity, entry->key);
    dest->key   = entry->key;
    dest->value = entry->value;
    ++count;
  }

  // Stop 3: finalize new table and delete old table.
  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->count = count;
  table->entries = entries;
  table->capacity = capacity;
}

bool hashSet(Hash* table, ObjString* key, Value* value)
{
   Entry* entry;
   bool isNewKey;
   ObjString* v = (ObjString*) value->as.obj;

   printf("hash-set()\n");
   printf("  - table (count %u, cap %u)\n", table->count, table->capacity);
   printf("  - params (%s->%s)\n", key->chars, v->chars);

   if (100 * (table->count + 1) > table->capacity * TABLE_MAX_LOAD)
   {
      int capacity = GROW_CAPACITY(table->capacity);
      adjustCapacity(table, capacity);
   }

   //printf("finding entry");
   entry = findEntry(table->entries, table->capacity, key);

   isNewKey = (entry->key == NULL);
   if (isNewKey && IS_NIL(entry->value)) 
   {
//      printf("new entry");
      table->count++;
   }
   else
   { 
//      printf("old entry");
   }

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

