#ifndef _value_h
#define _value_h

#include <stdint.h>
#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
   VAL_BOOL,
   VAL_NIL,
   VAL_NUMBER,
   VAL_OBJ
} ValueType;

typedef struct {
   ValueType type;
   union {
      bool boolean;
      int  number;
      Obj* obj;
   } as;
} Value;

#define IS_BOOL(value)		((value).type == VAL_BOOL)
#define IS_NIL(value)		((value).type == VAL_NIL)
#define IS_NUMBER(value)	((value).type == VAL_NUMBER)
#define IS_OBJ(value) 		((value).type == VAL_OBJ)

#define AS_BOOL(value)		((value).as.boolean)
#define AS_NUMBER(value)	((value).as.number)
#define AS_OBJ(value)		((value).as.obj)

Value* objVal(Obj* object);

void setBool(Value* val);
void setNil(Value* val);
void setNumber(Value* val);
void setObject(Value* obj);

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
