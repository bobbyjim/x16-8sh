#include <conio.h>
#include <time.h>
#include "random.h"

ranctx rrgenerator;

u4 ranval( ranctx *x ) 
{
    u4 e = x->a - randrot(x->b, 27);
    x->a = x->b ^ randrot(x->c, 17);
    x->b = x->c + x->d;
    x->c = x->d + e;
    x->d = e + x->a;
    return x->d;
}

void raninit( ranctx *x, u4 seed ) 
{
    u4 i;
    x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
    for (i=0; i<20; ++i) {
        (void)ranval(x);
    }
}

void burtle_srand(u4 seed)
{
   raninit(&rrgenerator, seed);
}

int burtle_random()
{
   if ( rrgenerator.initialized == 0 )
      burtle_srand(clock());

   return ranval(&rrgenerator) / 0xffffffff; 
}

void main() // int argc, char* argv[])
{
   int i = 100;
   burtle_srand(100);
   while(--i > 0)
   {
      int r = burtle_random();
      cprintf("%d\n", r);
   }
}

