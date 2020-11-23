#ifndef _list_h
#define _list_h

void  initList();
char* dynamicGetListValue(char* key);
int   deleteFromList(char* key);
int   putIntoList(char* key, char* val);
void  dumpList();

#endif
