#ifndef _util_h
#define _util_h

#include <stdio.h>

#define Null(type) ((type)NULL)
#define Nullch	   Null(char*)

char* bcopy(char* from, char* to, int len);
//char* bzero(char* loc, int len);
char* savestr(char *str);
void growstr(char** strptr, int* curlen, int newlen);

#endif
