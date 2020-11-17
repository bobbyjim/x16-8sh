
#include <string.h>
#include <stdint.h>
#include <peekpoke.h>

uint8_t currentBank = 0;

void putIntoBank(const char* string, uint8_t bank)
{
   int i;
   
   if (currentBank != bank)
   {
      POKE(0x9f61, bank);
      currentBank = bank;
   }
   
   for( i=0; i<strlen(string); i++ )
   {
      POKE(0xa000 + i, string[i]);
   }
}

char getFromBank(uint8_t bank, int position)
{
   if (currentBank != bank)
   {
      POKE(0x9f61, bank);
      currentBank = bank;
   }

   return (char) PEEK(0xa000 + position);
}
