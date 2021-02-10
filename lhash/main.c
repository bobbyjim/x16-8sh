
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>
#include <conio.h>

#include "hash.h"
#include "object.h"

#define HASH	Hash
#define STR	ObjString

void addOne(HASH* table, char* key, char* val)
{
   STR* keyo = str_make(key);
   Obj* valo = (Obj*) str_make(val);
   Value* value = objVal(valo);
   
   //hstore(table, key, value);
   hashSet(table, keyo, value);
}

void buildKey(char* k, int num)
{
   sprintf(k, "k-%d", num);
}

char* getOne(HASH* table, char* key)
{
   // return hfetch(table, key);

   STR* keyo = str_make(key);
   Value* value;

   hashGet(table, keyo, value);

   return AS_CSTRING(*value);
}

#define 	LOOPMAX		20

int main()
{
   HASH* table; // = hnew();
   int i;
   int j;

   initHash(table);
   adjustCapacity(table, 256);

   cbm_k_bsout(0x8E); // revert to primary case

   for(i=1; i<LOOPMAX; ++i)
   {
      char key[32];
      char val[64];

      buildKey(key, i);
      sprintf(val, "v-%d", i);
      addOne(table, key, val);

      printf("table count = %d, capacity = %d, heap = %d\n", table->count, table->capacity, _heapmaxavail());
      for(j=1; j<table->count; j++)
      {
         char* fetched = getOne(table, key);
         if ( fetched == NULL )
         {
            printf("  - key %d missing\n", j );
         }
      }
   }
   printf("\n");

   for(i=1; i<LOOPMAX; ++i)
   {
      char key[32];
      char* fetched;

      buildKey(key, i);
      fetched = getOne(table, key);
      printf("(%s,%s) ", key, fetched);
   }
   return 0;
}
