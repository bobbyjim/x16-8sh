#ifndef _value_h
#define _value_h

#include <stdint.h>
#include "common.h"

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

#define IS_BOOL(value)		((value).type == VAL_BOOL)
#define IS_NIL(value)		((value).type == VAL_NIL)
#define IS_NUMBER(value)	((value).type == VAL_NUMBER)

#define AS_BOOL(value)		((value).as.boolean)
#define AS_NUMBER(value)	((value).as.number)

void setBool(Value* val);
void setNil(Value* val);
void setNumber(Value* val);


//typedef int Value;

typedef struct {
   uint8_t capacity;
   uint8_t count;
   Value* values;
} ValueArray;


bool valuesEqual(Value* a, Value* b);
void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value* value);
void freeValueArray(ValueArray* array);
void printValue(Value* value);


#endif
