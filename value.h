#ifndef _value_h
#define _value_h

#include <stdint.h>
#include "common.h"

/*
typedef enum {
   VAL_BOOL,
   VAL_NIL,
   VAL_NUMBER
} ValueType;

typedef struct {
   ValueType type;
   union {
      bool boolean;
      int  number;
   } as;
} Value;
*/

typedef int Value;

typedef struct {
   uint8_t capacity;
   uint8_t count;
   Value* values;
} ValueArray;


void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);


#endif
