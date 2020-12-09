/*
  
    8SH: a command interpreter for 8 bit 'retro' systems.
    Copyright (C) 2020 Robert Eaglestone

    This file is part of 8SH.

    8SH is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    8SH is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 8SH.  If not, see <https://www.gnu.org/licenses/>.

*/

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
