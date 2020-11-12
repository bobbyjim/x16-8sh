#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <cbm.h>
#include <peekpoke.h>

#include "opcodes.h"

typedef struct {
        int op: 3;
        int reg: 2;
        int mod: 3;
} optype;

uint16_t *r;         // registers R1-R4
uint8_t  *ip;
uint16_t *sp;
uint8_t  *bytecode;
uint8_t  cmp_flag;

char buffer[256];
uint8_t program[20];

void init()
{
   r = 0x0002; // on the x16, the 16 bit pseudo regs are $02-$21
}

int interpret() // uint8_t *bytecode)
{
   uint16_t *memptr;
   uint8_t op;
   uint16_t val;
   optype ot;

   ip = bytecode;

   for(;;)
   {
      op = *ip; ++ip;
      ot = (optype) op;
      printf("op[%d] reg[%d] mod[%d]     ", ot.op, ot.reg, ot.mod);

      //
      // Decode RValue
      //
      if (ot.op > 0 && ot.mod < 4) val = r[ot.mod];
      else  // TODO: handle offset indirect modes
      {
         val = *ip; ++ip;
         val += 256 * *ip; ++ip;
      }

      switch(ot.op)
      {
        case OP_OR:   printf("or\n");  r[ot.reg] |= val; break;
        case OP_AND:  printf("and\n"); r[ot.reg] &= val; break;
        case OP_CMP:  printf("cmp\n"); cmp_flag = (r[ot.reg] < val)? -1 : (r[ot.reg] > val); break;
        case OP_SUB:  printf("sub\n"); r[ot.reg] -= val; break;
        case OP_ADD:  printf("add\n"); r[ot.reg] += val; break;
        case OP_LOD:  printf("lod\n"); r[ot.reg] = val; break;
        //case OP_STO:  printf("sto\n"); break;
        case OP_SPECIAL:
                printf("special:\n");
                switch(ot.reg)
                {
                   case SPECIAL_ZOP:    printf("   zop\n");
                        switch(ot.mod)
                        {
                           case ZOP_NOP: printf("      nop\n"); break;
                           case ZOP_HCF: printf("      hcf\n"); exit(0); // break;

     		           case ZOP_CALLA:  
			      printf("   calla to: $%04x\n", r[0]);
			      break;

                           case ZOP_PUTREG:
                              printf("      putreg\n");
                              printf("      a[%04x] b[%04x] c[%04x] d[%04x] f[%02x]\n",
                                 r[0],r[1],r[2],r[3],cmp_flag);
                              break;

                           case ZOP_PUTSTR:
                              memptr = &(r[0]);
                              printf("%X %s\n", memptr, *memptr);
                              break;
                        }
                        break;

                   case SPECIAL_JMP:    printf("   jmp\n");

                        // first, calculate target address
                        val = *ip; ++ip;
                        val += 256 * *ip; ++ip;

                        // now figure out what sort of jump we have
                        switch(ot.mod)
                        {
                           case JEQ: printf("      jeq\n"); if (cmp_flag==0) ip = val; break;
                           case JNE: printf("      jne\n"); if (cmp_flag!=0) ip = val; break;
                           case JLT: printf("      jlt\n"); if (cmp_flag< 0) ip = val; break;
                           case JLE: printf("      jle\n"); if (cmp_flag<=0) ip = val; break;
                           case JGT: printf("      jgt\n"); if (cmp_flag> 0) ip = val; break;
                           case JGE: printf("      jge\n"); if (cmp_flag>=0) ip = val; break;
                           case JMP: printf("      jmp\n"); ip = val; break;
                           default: return ERROR_UNKNOWN_OPCODE;
                        }
                        break;

                   case SPECIAL_NOT:    printf("   not\n"); break;
                   default: return ERROR_UNKNOWN_OPCODE;
                }
                return SUCCESS;
                break;

         default: return ERROR_UNKNOWN_OPCODE;
      }
   }
   return SUCCESS;
}


void tokenize()
{
   // in lieu of a real scanner...

   if ( ! strcmp(buffer, "exit") )
   {
      program[0] = OP_SPECIAL	+ 8*SPECIAL_ZOP		+ 32*ZOP_HCF;
   }
   else if ( ! strcmp(buffer, "reg" ) )
   {
      program[0] = OP_SPECIAL	+ 8*SPECIAL_ZOP		+ 32*ZOP_PUTREG;
   }
   else if ( ! strcmp(buffer, "time" ) )
   {
      program[0] = OP_LOD       + 8*REG_A               + 32*MEM_CONSTANT;
      program[1] = 0x50;
      program[2] = 0xff;
      program[3] = OP_SPECIAL   + 8*SPECIAL_ZOP         + 32*ZOP_CALLA;
   }

/*
   program[0]  = OP_LOD		+ 8*REG_A		+ 32*MEM_CONSTANT;
   program[1]  = 80;
   program[2]  = 0;
   program[3]  = OP_LOD          + 8*REG_B 		+ 32*MEM_CONSTANT;
   program[4]  = 1;
   program[5]  = 2;
   program[6]  = OP_CMP          + 8*REG_A               + 32*REG_B;
   program[9]  = OP_SPECIAL	+ 8*SPECIAL_ZOP 	+ 32*ZOP_PUTREG;
   program[7]  = OP_ADD		+ 8*REG_A		+ 32*REG_B;
   program[8]  = OP_CMP          + 8*REG_A               + 32*REG_B;
   program[9]  = OP_SPECIAL	+ 8*SPECIAL_ZOP 	+ 32*ZOP_PUTREG;
   program[10] = OP_SPECIAL	+ 8*SPECIAL_ZOP		+ 32*ZOP_HCF;
*/

   bytecode = program;
}

void reset() // program buffer
{
   int i;
   for(i=0; i<20; i++)
   {
      program[i] = 0; // NOP
   }
}

void repl()
{
   for(;;)
   {
      reset();
      putchar( '%' );
      putchar( ' ' );
      scanf( "%s", buffer );
      tokenize();
      interpret();
   }
}

void main() 
{
   init();

   cbm_k_bsout(0x8E); // revert to primary case

   printf("\n**************************************\n");
   printf("*                                    *\n");   
   printf("*                                    *\n");   
   printf("*                                    *\n");   
   printf("*   8sh for x16 v0.0 :: 2020-10-11   *\n");   
   printf("*                                    *\n");   
   printf("*   registers a-d in $02-$09         *\n");   
   printf("*                                    *\n");   
   printf("*   supported:                       *\n");   
   printf("*                                    *\n");   
   printf("*     exit                           *\n");   
   printf("*     reg                            *\n");   
   printf("*     time (sort of)                 *\n");   
   printf("*                                    *\n");   
   printf("*                                    *\n");   
   printf("**************************************\n\n");

   repl();
}

