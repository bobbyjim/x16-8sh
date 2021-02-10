#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cbm.h>

#include "hash.h"
#include "object.h"

// cl65 -t cx16 -o test-hash test-hash.c hash.c value.c memory.c object.c bank.c vm.c chunk.c debug.c compiler.c scanner.c

int total  = 0;
int passed = 0;
int failed = 0;

void title(char* title)
{
   puts("");
   puts("****************************************");
   puts("*                                      *");
   puts("*              test-hash               *");
   puts("*                                      *");
   puts("****************************************");
   puts("");
}

void verify(char* should, int shouldBe1, int actual)
{
   ++total;
   if (shouldBe1 == 1)
   {
      printf("   ok   ");
      ++passed;
   }
   else
   {
      printf("* fail *");
      ++failed;
   }
   printf(" %03d : %-20s %d\n", total, should, actual);
}

void summary()
{
   puts("");
   puts("****************************************");
   puts("*                                      *");
   puts("*                summary               *");
   puts("*                                      *");
   puts("****************************************");
   puts("");
   
   printf("failed:  %d/%d\n", failed, total);
}


static Hash* testInit()
{
   Hash table;
   initHash(&table);
   verify("count is 0",    table.count    == 0, table.count);
   verify("capacity is 8", table.capacity == 8, table.capacity);
   return &table;
}

void testHashcode(const char* string, int len)
{
   uint32_t hashcode1 = hashString(string, len);
   uint32_t hashcode2 = hashString(string, len);
   verify("hashcode works", hashcode1 == hashcode2, -1);
}

static ObjString* testAllocate(char* string, int len)
{
   uint32_t hashcode = hashString(string, len);
   ObjString* str = allocateString(string, len, hashcode);
   verify("obj-string length", str->length == len, str->length);
   verify("obj-string hash",   str->hash   == hashcode, str->hash);
   verify("obj-string chars",  !strcmp(str->chars, string), -1);
   return str;
}

static Value* testValue(ObjString* obj)
{
   Value* val = objVal((Obj*)obj);
   verify("value type is obj_string", val->type == 3, val->type);
   return val;
}

void testHashGet(Hash* table, ObjString* key, Value* val)
{
}

void testHashSet(Hash* table, ObjString* key, Value* val)
{
   hashSet(table, key, val);
   printf("table count = %d\n", table->count);
   //verify("new item added", table->count == 1);
}

void main()
{
   Hash* table;
   ObjString* k;
   ObjString* v;
   Value* val;
   uint32_t hashcode;

   cbm_k_bsout(0x8E); // revert to primary case

   title("test-hash");
   table = testInit();
   printf(" *table: count %u, cap %u\n", table->count, table->capacity);
   testHashcode("abcd", 4);
   k = testAllocate("abcd", 4);
   v = testAllocate("efgh", 4);

   val = testValue(v);
   
   printf(" *table: count %u, cap %u\n", table->count, table->capacity);
   testHashSet(table, k, val);

   summary();
}
