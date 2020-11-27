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

char mainInputBuffer[256];

void repl()
{
   int len = 0;
   int sourcebank = 1;
   int tokenbank  = 2;
  
   for(;;)
   {
      cputs( "% " );

      if (!fgets(mainInputBuffer, sizeof(mainInputBuffer), stdin)) {
         printf("\n");
         break;
      }

      mainInputBuffer[strlen(mainInputBuffer)-1] = 0; // chop newline

      if (!strcmp(mainInputBuffer,"exit")) exit(0);

      /******************************************************

       I've had a lot of input buffer pointers blow up on me;
       it SEEMS as though memory is real easy to corrupt.
       THEREFORE, it SEEMS that one way to avoid this problem
       is to store the "script" in Bank 1.

       Yeah that means the maximum script size is 8K.
       I don't think that's a problem.  

      ******************************************************/
      
      setBank(sourcebank);
      bankputs(mainInputBuffer, strlen(mainInputBuffer), 0);

      //scanAll(sourcebank, tokenbank);
      interpret(sourcebank, tokenbank); // in vm.c
   }
}

void main() 
{
   cbm_k_bsout(0x8E); // revert to primary case

   puts("\n");
   puts("***************************************");
   puts("*                                     *");   
   puts("*   8sh for x16 : v0.1 : 2020-11-27   *");   
   puts("*                                     *");   
   puts("***************************************");   
   puts("*                                     *");   
   puts("*   values supported:                 *");   
   puts("*                                     *");   
   puts("*   int constants, true, false, nil   *");   
   puts("*                                     *");   
   puts("*                                     *");   
   puts("*   ops supported:                    *");   
   puts("*                                     *");   
   puts("*   + - * / ()                        *");   
   puts("*   > >= == <= < !                    *");   
   puts("*                                     *");   
   puts("*                                     *");   
   puts("*   input is stored in bank 1         *");   
   puts("*   tokens are in bank 2              *");   
   puts("*                                     *");   
   puts("*   type 'exit' to exit               *");   
   puts("*                                     *");   
   puts("***************************************");
   puts("\n");

   initVM();
   //testVM();
   repl();
   freeVM();
}

