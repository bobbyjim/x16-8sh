#include <stdint.h>
#include <peekpoke.h>

int currentBank = -1;

void setBank(int bank)
{
   if (bank < 1) bank = 1;

   if (currentBank != bank)
   {
      POKE(0x9f61, bank);
      currentBank = bank;
   }
}

////////////////////////////////////////////////////////////////////
//
//  This is designed to work sort of like fgets.
//
////////////////////////////////////////////////////////////////////
int bankgets(char* toBuffer, int maxlen, int startpos)
{
   int i;
   startpos += 0xa000;
   for(i=0; i<maxlen; ++i)
   {
      int c = PEEK(startpos + i);
      toBuffer[i] = c;
      if (c=='\0') break;
   }
   toBuffer[maxlen] = '\0'; // EOL
   return 1;
}

void bankputs(const char* string, int len, int startpos)
{
   int i;
   startpos += 0xa000;
   for(i=0; i<len; ++i)
   {
      POKE(startpos + i, string[i]);
   }
   POKE(startpos + len, 0);  // EOF just to make sure.
}

char beek(int position)
{
   return (char) PEEK(0xa000 + position);
}

char beeknext(int position)
{
   return beek(position + 1); // '\0' on EOF is automatic
}

int memcmpBank(int sourcepos, const char* string, int length)
{
   int i;
   for(i=0; i<length; i++)
      if (string[i] != beek(sourcepos + i))
         return string[i] == beek(sourcepos + i);
   return 0;
}

void bankputbyte(uint8_t b, int pos)
{
   POKE(0xa000 + pos, b);
}

void bankputint(int b, int pos)
{
   POKE(0xa000 + pos, b & 0xff);
   POKE(0xa001 + pos, b >> 8);
}

int bankgetbyte(int pos)
{
   return PEEK(0xa000 + pos);
}

int bankgetint(int pos)
{
   return PEEK(0xa000 + pos)
        + (PEEK(0xa001 + pos) << 8);
}

