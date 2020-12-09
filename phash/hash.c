
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hash.h"
#include "str.h"
#include "util.h"


HASH* hnew()
{
   HASH* tb = (HASH*) malloc(sizeof(HASH));
   
   tb->tbl_array = (HENT**) malloc(8 * sizeof(HENT*));
   tb->tbl_fill = 0;
   tb->tbl_max  = 7;
//   hiterint(tb);  // so each() will start off right
   bzero((char*)tb->tbl_array, 8 * sizeof(HENT*));
   return tb;
}

STR* hfetch(HASH* tb, char* key)
{
   char* s;
   int i;
   int hash;
   HENT* entry;
 
   if (!tb) return Nullstr;
   for (s=key,	i=0,	hash=0;
     /* while */ *s && i < COEFFSIZE;
	++s,	++i,	hash *= 5) {
      hash += *s * coeff[i];
   }
   entry = tb->tbl_array[hash & tb->tbl_max];
   for(; entry; entry = entry->hent_next) {
      if (entry->hent_hash != hash) // strings cannot be equal
         continue;
      if (strcmp(entry->hent_key, key)) // is this it?
         continue;
      return entry->hent_val;
   }
   return Nullstr;
}

void hsplit(HASH* tb)
{
    int oldsize = tb->tbl_max + 1;
    int newsize = oldsize * 2;
    int i;
    HENT **a;
    HENT **b;
    HENT *entry;
    HENT **oentry;

    a = (HENT**) realloc((char*)tb->tbl_array, newsize * sizeof(HENT*));
    bzero((char*)&a[oldsize], oldsize * sizeof(HENT*)); /* zero second half */
    tb->tbl_max = --newsize;
    tb->tbl_array = a;

    for (i=0; i<oldsize; i++,a++) {
        if (!*a)                                /* non-existent */
            continue;
        b = a+oldsize;
        for (oentry = a, entry = *a; entry; entry = *oentry) {
            if ((entry->hent_hash & newsize) != i) {
                *oentry = entry->hent_next;
                entry->hent_next = *b;
                if (!*b)
                    tb->tbl_fill++;
                *b = entry;
                continue;
            }
            else
                oentry = &entry->hent_next;
        }
        if (!*a)                                /* everything moved */
            tb->tbl_fill--;
    }
}

bool hstore(HASH* tb, char* key, STR* val)
{
   char* s;
   int i;
   int hash;
   HENT* entry;
   HENT** oentry;

   if (!tb) return false;
   for (s=key,	i=0,	hash=0;
     /* while */ *s && i < COEFFSIZE;
	++s,	++i,	hash *= 5) {
	hash += *s * coeff[i];
   }

   oentry = &(tb->tbl_array[hash & tb->tbl_max]);
   i = 1;
    for (entry = *oentry; entry; i=0, entry = entry->hent_next) {
        if (entry->hent_hash != hash)           /* strings can't be equal */
            continue;
        if (strcmp(entry->hent_key,key)) /* is this it? */
            continue;
        free((char*)entry->hent_val);
        entry->hent_val = val;
        return true;
    }
    entry = (HENT*) malloc(sizeof(HENT));

    entry->hent_key = savestr(key);
    entry->hent_val = val;
    entry->hent_hash = hash;
    entry->hent_next = *oentry;
    *oentry = entry;

    if (i) {                            /* initial entry? */
        tb->tbl_fill++;
        if ((tb->tbl_fill * 100 / (tb->tbl_max + 1)) > FILLPCT)
            hsplit(tb);
    }

    return false;
}

