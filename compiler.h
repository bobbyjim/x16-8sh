#ifndef _compiler_h
#define _compiler_h

#include <stdint.h>

#include "vm.h"

bool compile(uint8_t sourceBank, uint8_t tokenBank, Chunk* chunk);

static void expression();

#endif
