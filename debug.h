#ifndef _debug_h
#define _debug_h

#include <stdint.h>

#include "chunk.h"
#include "scanner.h"

void testVM();

char* debugToken(TokenType type);
char* debugPrecedence(uint8_t p);

void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, int offset);

#endif
