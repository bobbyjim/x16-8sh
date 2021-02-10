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

#ifndef _hash_h
#define _hash_h

#include "common.h"
#include "value.h"

typedef struct {
  ObjString* key;
  Value value;
} Entry;

typedef struct {
   uint8_t count;
   uint8_t capacity;
   Entry* entries;
} Hash;


void initHash(Hash* hash);
void adjustCapacity(Hash* hash, int newCapacity);
void freeHash(Hash* hash);
Entry* findEntry(
                Entry* entries,
                int capacity,
                ObjString* key);
bool hashGet(Hash* table, ObjString* key, Value* value);
bool hashSet(Hash* table, ObjString* key, Value* value);
bool hashDelete(Hash* table, ObjString* key);
void hashAddAll(Hash* from, Hash* to);
ObjString* hashFindString(Hash* table, const char* chars, int length, uint32_t hash);
void hashDump(Hash* hash);

#endif
