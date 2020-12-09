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

