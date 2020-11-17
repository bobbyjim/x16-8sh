#ifndef _debug_h
#define _debug_h

#include <stdint.h>

#include "chunk.h"
#include "scanner.h"

void debugToken(Token* tok, char* msg);
void debugPrecedence(uint8_t p, char* msg);

void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, int offset);

#endif
