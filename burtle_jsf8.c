#include <stdint.h>
#include <time.h>
#include <conio.h>

//
// Taken from https://www.pcg-random.org/posts/bob-jenkins-small-prng-passes-practrand.html
//

typedef uint8_t u1;
typedef struct ranctx { 
	u1 a; 	
	u1 b; 
	u1 c; 
	u1 d;
} ranctx;

ranctx rrgenerator;

#define rot8(x,k) (((x)<<(k))|((x)>>(8-(k))))

uint8_t initialized = 0;

u1 ranval( ranctx* x )
{
    u1 e = x->a - rot8(x->b, 1);
    x->a = x->b ^ rot8(x->c, 4);
    x->b = x->c + x->d;
    x->c = x->d + e;
    x->d = e + x->a;
    return x->d;
}

void raninit( ranctx *x, u1 seed ) 
{
    u1 i;
    x->a = 0xed, x->b = x->c = x->d = seed;
    for (i=0; i<20; ++i) 
    {
        (void)ranval(x);
    }
}

void jsf8_srand(uint8_t seed)
{
   raninit( &rrgenerator, seed );
}

uint8_t jsf8_rand()
{
   if (!initialized)
      jsf8_srand(clock());

   return ranval( &rrgenerator );
}

/*
void main()
{
   int i=100;
   while(--i > 0)
   {
      cprintf("%u\r\n", jsf8_rand());
   }
}
*/
