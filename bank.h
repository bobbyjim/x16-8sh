#ifndef _bank_h
#define _bank_h

#include <stdint.h>

void putIntoBank(const char* string, uint8_t bank);
char getFromBank(uint8_t bank, int position);

#endif
