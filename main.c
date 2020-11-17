#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <cbm.h>
#include <peekpoke.h>
#include <time.h>

#include "vm.h"
#include "chunk.h"
#include "opcodes.h"
//#include "list.h"
#include "scanner.h"
// #include "bank.h"

uint16_t *r;         // registers R1-R4
uint8_t  *ip;
uint16_t *sp;
uint8_t  *bytecode;
struct    timespec *tp;

char inputLine[255];

/*
void init()
{
   r = (uint16_t*) 0x0002; // on the x16, the 16 bit pseudo regs are $02-$21

   initList();
   putEntry("sys", "x16");
}
*/

void repl()
{
   int bank = 1;
   for(;;)
   {
      putchar( '%' );
      putchar( ' ' );

      if (!fgets(inputLine, sizeof(inputLine), stdin)) {
         printf("\n");
         break;
      }

//      putIntoBank(inputLine, bank);
      interpret(inputLine);
   }
}

void testVM()
{
   int twelve;
   int five;
   int constant;
   Chunk chunk;

   initVM();

   initChunk(&chunk);
   twelve = addConstant(&chunk, 12);
   writeChunk(&chunk, OP_CONSTANT, 123);
   writeChunk(&chunk, twelve, 123);
 
   // ---

   constant = addConstant(&chunk, 3);
   writeChunk(&chunk, OP_CONSTANT, 123);
   writeChunk(&chunk, constant, 123);

   writeChunk(&chunk, OP_ADD, 123);

   // ---

   five = addConstant(&chunk, 5);
   writeChunk(&chunk, OP_CONSTANT, 123);
   writeChunk(&chunk, five, 123);

   writeChunk(&chunk, OP_DIVIDE, 123);

   // ---

   constant = addConstant(&chunk, 2);
   writeChunk(&chunk, OP_CONSTANT, 123);
   writeChunk(&chunk, constant, 123 );

   writeChunk(&chunk, OP_MODULO, 123);

   writeChunk(&chunk, OP_NEGATE, 123);
   writeChunk(&chunk, OP_RETURN, 123);

//   disassembleChunk(&chunk, "test chunk");
   interpretChunk(&chunk);
   freeVM();
   freeChunk(&chunk);
}

void main() 
{
   cbm_k_bsout(0x8E); // revert to primary case

//   testVM();

//   init();  // list stuff

   puts("\n");
   puts("***************************************");
   puts("*                                     *");   
   puts("*                                     *");   
   puts("*                                     *");   
   puts("*   8sh for x16 : v0.1 : 2020-11-16   *");   
   puts("*                                     *");   
   puts("*                                     *");   
   puts("*                                     *");   
   puts("***************************************");
   puts("\n");

   initVM();
   repl();
   freeVM();
}

