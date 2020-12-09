#ifndef _list_h
#define _list_h

typedef struct {
   char* sval;
   int   len;
   long  nval;
   char  nok;	// state of nval
} VALUE;

typedef struct {
   char*  key;
   VALUE* val;
} ENTRY;

void   initList();
VALUE* dynamicGetValue(char* key);
int    deleteEntry(char* key);
int    putEntry(char* key, VALUE* val);
void   dumpList();

#endif
