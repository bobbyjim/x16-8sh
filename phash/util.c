
#include <stdlib.h>
#include <string.h>

#include "util.h"

char* bcopy(char* from, char* to, int len)
{
   char* retval = to;
   while (len--) *to++ = *from++;
   return retval;
}

/*
char* bzero(char* loc, int len)
{
   char *retval = loc;
   while (len--) *loc++ = 0;
   return retval;
}
*/

char* savestr(char* str)
{
   char* newaddr = malloc(strlen(str)+1);
   strcpy(newaddr, str);
   return newaddr;
}

void growstr(char** strptr, int* curlen, int newlen)
{
   if (newlen > *curlen) {
      if (*curlen)
         *strptr = realloc(*strptr, newlen);
      else
         *strptr = malloc(newlen);
   }
}

