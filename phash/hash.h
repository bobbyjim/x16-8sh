#ifndef _hash_h
#define _hash_h

#include <stdbool.h>

#include "str.h"

#define FILLPCT 70

typedef struct hentry HENT;
typedef struct htbl HASH;

struct hentry {
   HENT* hent_next;
   char* hent_key;
   STR*  hent_val;
   int   hent_hash;
};

struct htbl {
   HENT** tbl_array;
   int    tbl_max;
   int    tbl_fill;
   int    tbl_riter;  // current root of iterator
   HENT*  tbl_either; // current entry of iterator
};

STR* hfetch();
bool hstore();
STR* hdelete();
HASH* hnew();
void hclear();
void hfree();
void hentfree();
int hiterinit();
HENT* hiternext();
char* hiterkey();
STR* hiterval();

#define COEFFSIZE (16 * 8)      /* size of array below */
static char coeff[] = {
                61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
                61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
                61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
                61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
                61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
                61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
                61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
                61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1};

#endif

