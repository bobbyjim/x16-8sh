#ifndef _str_h
#define _str_h

struct string {
    char*       str_ptr;        /* pointer to malloced string */
    long        str_nval;       /* numeric value, if any */
    int         str_len;        /* allocated size */
    char        str_nok;        /* state of str_nval */
};

typedef struct string STR;

#define CHARS(s)	(s->str_ptr)
#define Nullstr Null(STR*)
#define GROWSTR(pp,lp,len) if (*(lp) < (len)) growstr(pp,lp,len)

STR* str_static();
STR* str_make();
STR* str_nmake();
void growstr(char** strptr, int* curlen, int newlen);

#endif
