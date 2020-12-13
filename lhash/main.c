
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
   sprintf(k, "key-%d", num);
}

char* getOne(HASH* table, char* key)
{
   // return hfetch(table, key);

   STR* keyo = str_make(key);
   Value* value;

   hashGet(table, keyo, value);

   return AS_CSTRING(*value);
}

#define 	LOOPMAX		150

int main()
{
   HASH* table; // = hnew();
   int i;

   initHash(table);

   cbm_k_bsout(0x8E); // revert to primary case

   //printf("size of an entry: %d\n", sizeof(HENT));
   for(i=1; i<LOOPMAX; ++i)
   {
      char key[32];
      char val[64];

      buildKey(key, i);
      sprintf(val, "value-%d", i);
      addOne(table, key, val);
      if (i % 16 == 0)
      { 
         hashDump(table);
      }
   }
   printf("\n");

   for(i=1; i<LOOPMAX; ++i)
   {
      char key[32];
      char* fetched;

      buildKey(key, i);
      fetched = getOne(table, key);
      printf("%s: %s  ", key, fetched);
      if (i % 4 == 0 )
      {
         printf("\n");
      }
   }

   return 0;
}
