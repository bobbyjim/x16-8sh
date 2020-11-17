
int testvars(void);

void init_heap();
void heap_vars_add(char *name, char *value);
void heap_vars_dump();
void set_line_in(char *buf);

unsigned short heap_vars_lookup(unsigned int start, unsigned int stop);
unsigned short heap_var_exists(char* varname);
