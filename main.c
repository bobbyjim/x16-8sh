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

#define FORTUNE_MAX

void logout()
{
   int r;
   char* fortune[FORTUNE_MAX] = {
        "    i'm sorry dave, i'm afraid i can't do that.",
	"    whom computers would destroy, they must first drive mad.",
	"    your lucky number is 3552664958674928.  watch for it everywhere.",
	"    it may be that your whole purpose in life is simply to serve as a\n    warning to others.",
	"    your fault -- core dumped",
	"    if we do not change our direction we may end up where we're heading.",
	"    q: why did the tachyon cross the road?\n    a: because it was on the other side.",
	"    overload -- core meltdown sequence initiated.",
	"    all programmers are playwrights and all computers are lousy actors.",
	"    /earth is 98% full ... please delete anyone you can.",

	"    every little picofarad has a nanohenry all its own.\n \
                -- don vonada",

	"    klein bottle for rent -- inquire within.",

	"    niklaus wirth has lamented that, whereas europeans pronounce his name\n"
	"    correctly (ni-klows virt), americans invariably mangle it into\n"
	"    (nick-les worth).  which is to say that europeans call him by name, but\n"
	"    americans call him by value.",

	"    q:      why do ducks have flat feet?\n"
	"    a:      to stamp out forest fires.\n"
	"\n"
	"    q:      why do elephants have flat feet?\n"
	"    a:      to stamp out flaming ducks.\n",

	"    there was a young poet named dan,\n"
	"    whose poetry never would scan.\n"
        "    when told this was so,\n"
        "    he said, \"yes, i know.\n"
	"it's because i try to put every possible syllable into that last line that i can\n"
	"    .\"",

	"    a witty saying proves nothing.\n"
        "       -- voltaire",

	"    ray's rule of precision:\n"
        "    measure with a micrometer.  mark with chalk.  cut with an axe."

   };
 
   _randomize();
   r = rand(); // 0 to 32767
   r %= 17;
   printf("\n\n\n%s\n\n\n", fortune[r]);
}

void repl()
{
   int len = 0;
   int sourcebank = 1;
   int tokenbank  = 2;
  
   for(;;)
   {
      printf("%u", _heapmemavail());
      cputs( "% " );

      if (!fgets(lineInputBuffer, sizeof(lineInputBuffer), stdin)) {
         printf("\n");
         break;
      }

      lineInputBuffer[strlen(lineInputBuffer)-1] = '\0'; // chop newline
      printf("[%s]\n", lineInputBuffer);

      if(strlen(lineInputBuffer) > 0)
      {
         if (!strcmp(lineInputBuffer,"logout")) logout();
         if (!strcmp(lineInputBuffer,"globals")) 
         {
            hashDump(&vm.globals);
         }
         else
         {
            /******************************************************

             Script is stored in Bank 1.
             That means the maximum script size is 8K.
             I don't think that's a problem!

            ******************************************************/
      
            setBank(sourcebank);
            bankputs(lineInputBuffer, strlen(lineInputBuffer), 0);
 
            scan(sourcebank, tokenbank);
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

