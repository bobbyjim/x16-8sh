#ifndef _float5_h
#define _float5_h

typedef struct {
   int  exponent;
   long mantissa;
} Float5;

void add(Float5* a, Float5* b);
void mul(Float5* a, Float5* b);
void invert(Float5* a);

#endif

