#ifndef _compiler_h
#define _compiler_h

#include "vm.h"

bool compile(const char* source, Chunk* chunk);
//bool compile(int sourceBank, Chunk* chunk);

static void expression();

#endif
