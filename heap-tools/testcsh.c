#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_heap(void);
void process(void);
void substitute(void);
unsigned short heap_vars_lookup(unsigned short A, unsigned short B);

char line_in[256];
char line_sub[256];
char heap_vars[4096];
unsigned char i;
unsigned char j;
unsigned char tick;

/*typedef struct heap_segment{
    struct heap_segment * next;
    struct heap_segment * prev;
    unsigned char is_allocated; // to be removed
    signed short segment_size;  // Includes this header, negative is allocated
} heap_segment_t;
*/

void heap_vars_dump(void) {
  unsigned short curr = 2;
dump1:
  printf("%d:\n", curr);
  printf("next = %d\n", *(unsigned short *)&heap_vars[curr]);
  printf("prev = %d\n", *(unsigned short *)&heap_vars[curr+2]);
  printf("size = %04X (%d bytes %s)\n", *(unsigned short *)&heap_vars[curr+4], *(unsigned short *)&heap_vars[curr+4] & 0x7fff, (*(unsigned short *)&heap_vars[curr+4]&0x8000)>0?"used":"free");
  if(*(unsigned short *)&heap_vars[curr+4]>0) {
    printf("name = %s (len %ld)\n", &heap_vars[curr+6], strlen(&heap_vars[curr+6])+1);
    printf("value = %s (len %ld)\n", &heap_vars[curr+6+strlen(&heap_vars[curr+6])+1], strlen(&heap_vars[curr+6+strlen(&heap_vars[curr+6])+1])+1);
  }
  curr = *(unsigned short *)&heap_vars[curr];
  if( curr > 0 )
    goto dump1;
} /* heap_vars_dump() */

void heap_vars_add(char *name, char *value) {
    unsigned short curr, best = 0, bytes;
    unsigned char is_allocated;
    signed short diff, best_diff = 0x7fff; // Max signed short
    unsigned char l, m;

    // Add the header to the number of bytes we need
    bytes = strlen(name) + 1 + strlen(value) + 1 + 6;

    // Find the allocation that is closest in size to this request
    for (curr = 2; curr != 0; curr = *(unsigned short *)&heap_vars[curr]) {
        is_allocated = ((*(unsigned short *)&heap_vars[curr+4] & 0x8000) != 0);
        diff = (*(unsigned short *)&heap_vars[curr+4] & 0x7fff) - bytes;
        if ((is_allocated == 0) && (diff < best_diff) && (diff >= 0)) {
            best = curr;
            best_diff = diff;
        }
    }

    // There must be no free memory right now :(
    if (best == 0) {
        printf("error: out of variable space\n");
        exit(4);
    }

    // If the best difference we could come up with was large, split up this segment into two.
    // Since our segment headers are rather large, the criterion for splitting the segment is that
    // when split, the segment not being requested should be twice a header size
    if (best_diff > 15) {
        //bzero(((void*)(best)) + bytes, sizeof(heap_segment_t));
        curr = *(unsigned short *)&heap_vars[best];
        *(unsigned short *)&heap_vars[best] = best + bytes;
        *(unsigned short *)&heap_vars[best+bytes] = *(unsigned short *)&heap_vars[curr];
        *(unsigned short *)&heap_vars[best+bytes+2] = best;
        *(unsigned short *)&heap_vars[best+bytes+4] = *(unsigned short *)&heap_vars[best+4] - bytes;
        *(unsigned short *)&heap_vars[best+4] = bytes | 0x8000; /* is_allocated */
    }

    l = 0;
add1:
    heap_vars[best+6+l] = name[l];
    if( name[l] != '\000' ) {
      l++;
      goto add1;
    }
    l++;
    m = 0;
add2:
    heap_vars[best+6+l] = value[m];
    if( value[m] != '\000' ) {
      l++;
      m++;
      goto add2;
    }
}

/*void heap_vars_delete(unsigned short ptr) {
    heap_segment_t * seg = ptr - sizeof(heap_segment_t);
    seg->is_allocated = 0;

    // try to coalesce segements to the left
    while(seg->prev != NULL && !seg->prev->is_allocated) {
        seg->prev->next = seg->next;
        seg->prev->segment_size += seg->segment_size;
        seg = seg->prev;
    }
    // try to coalesce segments to the right
    while(seg->next != NULL && !seg->next->is_allocated) {
        seg->segment_size += seg->next->segment_size;
        seg->next = seg->next->next;
    }
}
*/


// heap_var structure =
// BB *next
// BB *prev
// BB size (negative/B1b7 is allocated)
// B+ name
// B 0 (delimiter)
// B* value
// B 0 (delimiter

int main(void)
{
  init_heap();
  strcpy(line_in, "line with no substitutions.");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  strcpy(line_in, "line with terminal $");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  strcpy(line_in, "line with bare $ embedded.");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  strcpy(line_in, "line with both $varA and $(varB) substituted.");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  strcpy(line_in, "line with $?(varA) and $?varC substituted.");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  strcpy(line_in, "line with $#varA and $%(varB) substituted.");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  strcpy(line_in, "line with $(varA) substituted.");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  strcpy(line_in, "line with $(varA)sub nospace.");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  strcpy(line_in, "line with $varBsub nospace.");
  printf("replacing: %s\n", line_in);
  process();
  printf(" replaced: %s\n", line_sub);
  return 0;
} /* main() */

void init_heap(void)
{
  heap_vars[0] = 0;
  heap_vars[1] = 0;
  heap_vars[2] = 0;
  heap_vars[3] = 0;
  heap_vars[4] = 0;
  heap_vars[5] = 0;
  *(unsigned short *)&heap_vars[6] = sizeof(heap_vars);
  heap_vars[8] = 0;
  heap_vars[9] = 0;
  //heap_vars_add("varA", "value A");
  //heap_vars_add("varB", "value B");
  heap_vars_dump();
  printf("\n");
} /* init_heap() */

void process(void)
{
  i = 0;
  j = 0;
  tick = 0;
  
process_loop:
  if( line_in[i] == '$' ) {
    if( (line_in[i+1] == ' ') ||
        (line_in[i+1] == '\t') ||
        (line_in[i+1] == '\000') ||
        (tick == 1) ) {
      goto copy;
    }
    substitute();
    goto process_loop;
  }
  if( line_in[i] == '\'' ) {
    tick = !tick;
  }
  if( line_in[i] == '\000' ) {
    line_sub[j] = '\000';
    return;
  }
copy:
  line_sub[j] = line_in[i];
  i++;
  j++;
  goto process_loop;
} /* process() */

void substitute(void)
{
  unsigned char paren = 0;
  unsigned char mode = 0; /* 0 = value, 1 = defined, 2 = words, 3 = characters */
  unsigned char sub_start = 0;
  unsigned char sub_end = 0;
  unsigned char var_start = 0;
  unsigned char var_end = 0;
  unsigned short var_ptr;
  unsigned char var_count;
  
  i++; /* last character was $ so advance read pointer */
  if( line_in[i] == '?' ) {
    mode = 1;
    i++;
  } else if( line_in[i] == '#' ) {
    mode = 2;
    i++;
  } else if( line_in[i] == '%' ) {
    mode = 3;
    i++;
  } /* if: first character after $ is ?, #, % */
  if( line_in[i] == '(' ) {
    paren = 1;
    i++;
  } /* if: first character after $, $?, $#, or $% is ( */
  var_start = i;
  
substitute1:
  if( (line_in[i] == ')') && (paren == 1) ) {
    var_end = i-1;
    i++;
    paren = 0;
    goto substitute2;
  } /* if: encounter matching ) */
  if( ( (line_in[i] >= 'a') && (line_in[i] <= 'z') ) ||
      ( (line_in[i] >= 'A') && (line_in[i] <= 'Z') ) ||
      ( (line_in[i] >= '0') && (line_in[i] <= '9') ) ||
      (line_in[i] == '_') ) {
    i++;
    goto substitute1;
  } /* if: a word character */
  var_end = i-1; /* any other character ends variable */

substitute2:
  if( paren == 1 ) {
    printf("error: variable name missing matching parenthesis\n");
    exit(1);
  }
  if( var_end == var_start ) {
    printf("error: variable name missing after $\n");
    exit(2);
  }
  if( line_in[i] == '[' ) {
    /* VAJ: TBD */
  } /* if: subscripts present */

  var_ptr = heap_vars_lookup(var_start, var_end);

  if( mode == 1 ) {
    if( var_ptr == 0 ) {
      line_sub[j] = '0';
    } else {
      line_sub[j] = '1';
    }
    j++;
    return;
  } /* if: mode 1 (defined) */
  if( var_ptr == 0 ) {
    printf("error: variable not defined\n");
    exit(3);
  }

  if( mode == 2 ) {
    var_count = 0;
    mode = 0; /* reuse mode 0 = non-word, 1 = word */
substitute3:
    if( heap_vars[var_ptr] == '\000' ) {
      /* VAJ: print number(var_count) to string */
      j += sprintf( &line_sub[j], "%d", var_count );
      return;
    }
    if( mode == 0 ) {
      if( ( (heap_vars[var_ptr] >= 'a') && (heap_vars[var_ptr] <= 'z') ) ||
          ( (heap_vars[var_ptr] >= 'A') && (heap_vars[var_ptr] <= 'Z') ) ||
          ( (heap_vars[var_ptr] >= '0') && (heap_vars[var_ptr] <= '9') ) ||
          (heap_vars[var_ptr] == '_') ) {
        mode = 1;
        var_count++;
      }
    } else {
      if( ( (heap_vars[var_ptr] >= 'a') && (heap_vars[var_ptr] <= 'z') ) ||
          ( (heap_vars[var_ptr] >= 'A') && (heap_vars[var_ptr] <= 'Z') ) ||
          ( (heap_vars[var_ptr] >= '0') && (heap_vars[var_ptr] <= '9') ) ||
          (heap_vars[var_ptr] == '_') ) {
        mode = mode;
      } else {
        mode = 0;
      }
    }
    var_ptr++;
    goto substitute3;
  } /* if: mode 2 (words) */

  if( mode == 3 ) {
    var_count = 0;
substitute4:
    if( heap_vars[var_ptr] != '\000' ) {
      var_ptr++;
      var_count++;
      goto substitute4;
    }
    /* VAJ: print number(var_count) to string */
    j += sprintf( &line_sub[j], "%d", var_count );
    return;
  } /* if: mode 3 (characters) */
    
substitute5:
  if( heap_vars[var_ptr] == '\000' ) {
    return;
  } /* if: end of substitution */
  line_sub[j] = heap_vars[var_ptr];
  j++;
  var_ptr++;
  goto substitute5;
} /* substitute() */

unsigned short heap_vars_lookup(unsigned short var_start, unsigned short var_end)
{
  unsigned short var_ptr = 2;
  unsigned char k;

lookup1:
  if( heap_vars[var_ptr+5] < 0 ) {
    k = 0;
lookup2:
    if( (heap_vars[var_ptr+6+k] == '\000') && (var_start+k > var_end) ) {
      return var_ptr+6+k+1;
    } /* if: end condition */
    if( heap_vars[var_ptr+6+k] == line_in[var_start+k] ) {
      k++;
      goto lookup2;
    } /* if: characters match */
  } /* if: allocated variable */
  if (*(unsigned short *)&heap_vars[var_ptr] != 0) {
    var_ptr = *(unsigned short *)&heap_vars[var_ptr];
    goto lookup1;
  } /* if: check next variable */
  return 0;
} /* lookup() */

unsigned short heap_var_exists(char *varname)
{
   unsigned short var_ptr = 2;
   unsigned char k;
   unsigned short var_start = 0;
   unsigned short var_end = strlen(varname);

lookup1:
  if( heap_vars[var_ptr+5] < 0 ) {
    k = 0;
lookup2:
    if( (heap_vars[var_ptr+6+k] == '\000') && (var_start+k > var_end) ) {
      return var_ptr+6+k+1;
    } /* if: end condition */
    if( heap_vars[var_ptr+6+k] == varname[var_start+k] ) {
      k++;
      goto lookup2;
    } /* if: characters match */
  } /* if: allocated variable */
  if (*(unsigned short *)&heap_vars[var_ptr] != 0) {
    var_ptr = *(unsigned short *)&heap_vars[var_ptr];
    goto lookup1;
  } /* if: check next variable */
  return 0;
}

