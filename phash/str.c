
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "str.h"

STR* str_new(int len)
{
   STR* str = (STR *) malloc(sizeof(STR));
   bzero((char*)str, sizeof(STR));
   if (len)
      GROWSTR(&(str->str_ptr), &(str->str_len), len + 1);
   return str;
}

void str_grow(STR* str, int len)
{
   if (len && str)
      GROWSTR(&(str->str_ptr), &(str->str_len), len + 1);
}

void str_set(STR* str, char* ptr)
{
   int len;
   if (!ptr) ptr = "";
   len = strlen(ptr);
   GROWSTR(&(str->str_ptr), &(str->str_len), len + 1);
   bcopy(ptr,str->str_ptr,len+1);
   str->str_len = len;
   str->str_nok = 0; // invalidate number
}

STR* str_make(char* s)
{
   STR* str = str_new(0);
   str_set(str, s);
   return str;
}

