#ifndef _list_h
#define _list_h

void  initList();
char* dynamicGetValue(char* key);
int   deleteEntry(char* key);
int   putEntry(char* key, char* val);
void  dumpList();

#endif
