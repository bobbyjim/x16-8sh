
#include <stdio.h>
#include <cbm.h>

#include "hash.h"
#include "str.h"

int main()
{
   HASH* table = hnew();
   STR*  value = str_make("this is a test");
   STR*  fetched;

   cbm_k_bsout(0x8E); // revert to primary case

   hstore(table, "foo", value);
   printf("stored: %s\n",  value->str_ptr);

   fetched = hfetch(table, "foo");
   printf("fetched: %s\n", fetched->str_ptr);

   return 0;
}
