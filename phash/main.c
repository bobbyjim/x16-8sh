
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cbm.h>
#include <conio.h>

#include "hash.h"
#include "str.h"

void addOne(HASH* table, char* key, char* val)
{
   STR* value = str_make(val);
   hstore(table, key, value);
}

void buildKey(char* k, int num)
{
   sprintf(k, "key-%d", num);
}

#define 	LOOPMAX		200

int main()
{
   HASH* table = hnew();
   int i;

   char* keys[] = {
	"starport",
   	"size",
	"atmosphere",
	"hydrographics",
	"population",
	"government",
	"law level",
	"tech level",
	"bases",
	"trade codes",
	"pbg",
	"zone",
	"stellar"
   };

   char* vals[] = {
   	"a",
	"7",
	"8",
	"8",
	"8",
	"9",
	"9",
	"c",
	"ns",
	"ag ri sa",
	"123",
	"g",
	"g4 v m0 d"
   };

   cbm_k_bsout(0x8E); // revert to primary case

   printf("size of a STR: %d\n", sizeof(STR));

   for(i=0; i<13; ++i)
   {
      printf("adding %s -> %s\n", keys[i], vals[i]);
      addOne(table, keys[i], vals[i]);
   }
   printf("\n");
   for(i=0; i<13; ++i)
   {
      STR* fetched = hfetch(table, keys[i]);
      printf("%s -> %s\r\n", keys[i], fetched->str_ptr);
   }

   for(i=1; i<LOOPMAX; ++i)
   {
      char key[32];
      char val[64];

      buildKey(key, i);
      sprintf(val, "value=%d 012345678901234567890123456789", i);
      addOne(table, key, val);
      if (i % 16 == 0) printf("%d ", i);
   }
   printf("\n");

   for(i=1; i<LOOPMAX; ++i)
   {
      char key[32];

      STR* fetched;
      buildKey(key, i);
      fetched = hfetch(table, key);
      printf("%s -> %s\r\n", key, fetched->str_ptr);
   }

   return 0;
}
