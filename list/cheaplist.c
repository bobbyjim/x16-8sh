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

#include <conio.h>
#include <stdlib.h>
#include <string.h>

#include "cheaplist.h"

typedef struct {
   char* key;
   char* val;
} ENTRY;

int freelist = 256;
ENTRY list[256];


int findInList(char* key)
{
   int i = -1;
   while(++i<256)
   {
      if( list[i].key != NULL && ! strcmp(list[i].key, key) )
      {
         return i;
      }
   }
   return -1;
}

char* dynamicGetListValue(char* key)
{
   int i = -1;
   i = findInList(key);

   if (i == -1) 
      i = putIntoList(key, "0");

   if (i > -1)
      return list[i].val;

   return "out of variable space";
}

int deleteFromList(char* key)
{
   int i = findInList(key);
   if (i > -1) // found it
   { 
      list[i].key = NULL;
      ++freelist;
   }
   return i;
}

void initList()
{
   int i = -1;
   while(++i<256)
   {
      list[i].key = NULL;
   }
   freelist = 256;
}

void printEntry(int i)
{
   if (list[i].key != NULL)
   {
      cprintf("entry: %d\n", i);
      cprintf("key:   %s\n", list[i].key);
      cprintf("val:   %s\n", list[i].val);
   }
}

int findFree()
{
   int i = -1;
   while(++i<256)
   {
      if( list[i].key == NULL )
      {
         return i;
      }
   }
   return -1;
}


int putIntoList(char* key, char* val)
{
   int i;

   if (freelist == 0) return 0;

   i = findInList(key);
   if (i == -1) i = findFree();

   if (i > -1) // found a spot
   {
      list[i].key = realloc(list[i].key, strlen(key)+1);
      list[i].val = realloc(list[i].val, strlen(val)+1);
      strcpy(list[i].key, key);
      strcpy(list[i].val, val); 
      --freelist;
      return i;
   }
   
   cprintf("out of variable space.\n");

   return -1; 
}

void dumpList()
{
   int i = -1;
   cprintf("free slots: %d\n", freelist);
   while(++i<256)
   {
      printEntry(i);
   }
}


/*
int main(void)
{
   initList();
   putIntoList("hi", "there");
   cprintf("dynamic test value: %s\n", dynamicGetValue("this is a dynamic test"));
   dumpList();
   return 0;
}
*/
