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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cbm.h>
#include <conio.h>

#include "bank.h"

typedef struct {
   int exponent :8;
   long mantissa;
} Float5;

char inputBuffer[256];
char fetchBuffer[256];
char* current;

void repl()
{
   int bank = 1;

   printf("%d\n", bank);
/*
   for(;;)
   {
      cputs("% ");

      if (!fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
         cputs("\n");
         break;
      }

      setBank(bank);
      bankputs(inputBuffer, strlen(inputBuffer), 0);
      bankgets(fetchBuffer, sizeof(fetchBuffer), 0);
      puts(fetchBuffer);
   }
*/
}


int main()
{
   cbm_k_bsout(0x8E); // revert to primary case

   repl();

   return 0;
}
