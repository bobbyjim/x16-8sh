
#include <stdio.h>
#include <stdint.h>
#include <cbm.h>

typedef struct {
   int exponent :8;
   long mantissa;
} Float5;

char* line;


int main()
{
   cbm_k_bsout(0x8E); // revert to primary case

   line = "this is only a test of cc65s pointer handling on the x16";


   return 0;
}
