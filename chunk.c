#include <stdlib.h>
#include <stddef.h>

#include "memory.h"
#include "chunk.h"

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(uint8_t, chunk->lines, chunk->capacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, uint8_t line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code,
        oldCapacity, chunk->capacity);

    //
    //  change to something like runlength encoding
    //
    chunk->lines = GROW_ARRAY(uint8_t, chunk->lines,
        oldCapacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

