#ifndef _bank_h
#define _bank_h

void setBank(int bank);

int bankgets(char* toBuffer, int len, int pos);
int bankgetbyte(int pos);
int bankgetint(int pos);

void bankputs(const char* string, int len, int pos);
void bankputbyte(uint8_t b, int pos);
void bankputint(int b, int pos);

char beek(int position);
char beeknext(int position);
int memcmpBank(int sourcepos, const char* string, int length);

#endif
