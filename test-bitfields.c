// cl65 -t cx16 test-bitfields.c -o test-bitfields

#include <stdio.h>
#include <cbm.h>

typedef struct 
{
   int type:   8;
   int length: 8;
   int blockOffset: 8;
   int block: 5;
   int flag1: 1;
   int flag2: 1;
   int flag3: 1;
   int line: 8;

} Token;

typedef union 
{
   Token token;
   char  data[5];

} TokenUnion;

void main()
{
   TokenUnion test;

   cbm_k_bsout(0x8E); // revert to primary case

   test.data[0] = 10; 
   test.data[1] = 11; 
   test.data[2] = 12; 
   test.data[3] = 255; 
   test.data[4] = 13; 

   printf("size of struct: %d\n", sizeof(Token));

   printf("type:   %d\n", test.token.type);
   printf("length: %d\n", test.token.length);
   printf("offs:   %d\n", test.token.blockOffset);
   printf("block:  %d\n", test.token.block);
   printf("flag1:  %d\n", test.token.flag1);
   printf("flag2:  %d\n", test.token.flag2);
   printf("flag3:  %d\n", test.token.flag3);
   printf("line:   %d\n", test.token.line);
}

