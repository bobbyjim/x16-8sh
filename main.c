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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <cbm.h>
#include <peekpoke.h>
#include <time.h>
#include <conio.h>

#include "vm.h"
#include "chunk.h"
#include "opcodes.h"
#include "scanner.h"
#include "bank.h"
#include "debug.h"
#include "hash.h"

char lineInputBuffer[256];

void repl()
{
   int len = 0;
   int sourcebank = 1;
   int tokenbank  = 2;
  
   for(;;)
   {
      cputs( "% " );

      if (!fgets(lineInputBuffer, sizeof(lineInputBuffer), stdin)) {
         printf("\n");
         break;
      }

      lineInputBuffer[strlen(lineInputBuffer)-1] = '\0'; // chop newline
      printf("[%s]\n", lineInputBuffer);

      if(strlen(lineInputBuffer) > 0)
      {
         if (!strcmp(lineInputBuffer,"exit")) exit(0);
         if (!strcmp(lineInputBuffer,"globals")) 
         {
            hashDump(&vm.globals);
         }
         else
         {
            /******************************************************

             Script is stored in Bank 1.
             That means the maximum script size is 8K.
             I don't think that's a problem.  

            ******************************************************/
      
            setBank(sourcebank);
            bankputs(lineInputBuffer, strlen(lineInputBuffer), 0);

            interpret(sourcebank, tokenbank); // in vm.c


         }
      }
   }
}

void main() 
{
   cbm_k_bsout(0x8E); // revert to primary case

   puts("\n");
   puts("8sh for x16 (2020.12.01)"); // copyright (c) 2020 robert eaglestone\n");
   //puts("  this program comes with absolutely no warranty.");
   //puts("  this is free software, and you are welcome to redistribute it");
   //puts("  under certain conditions; consult 'copying' for details.");
   puts("\n");

   initVM();
   //testVM();
   repl();
   freeVM();
}

