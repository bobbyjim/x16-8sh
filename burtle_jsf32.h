#ifndef bcpm_rand_h
#define bcpm_rand_h

typedef unsigned long int  u4;
typedef struct ranctx 
{ 
   u4 a; 
   u4 b;  
   u4 c; 
   u4 d; 
   int initialized;
} ranctx;

#define randrot(x,k) (((x)<<(k))|((x)>>(32-(k))))
u4 ranval( ranctx *x );
void raninit( ranctx *x, u4 seed );
void burtle_srand(u4 seed);
int burtle_random();

#endif
