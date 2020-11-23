
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
}


int main()
{
   cbm_k_bsout(0x8E); // revert to primary case

   repl();

   return 0;
}
