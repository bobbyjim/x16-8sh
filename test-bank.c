// cl65 -t cx16 test-bank.c -o test-bank

#include <stdio.h>
#include <cbm.h>
#include <peekpoke.h>

#define LOGICAL_FILE_NUM	1
#define SD_CARD			8
#define HOST_FILE_SYSTEM	1
#define SA_IGNORE_HEADER	0
#define BANK			1
#define LOAD_FLAG		0

void main()
{
   cbm_k_bsout(0x8E); // revert to primary case

   cbm_k_setnam("fortune.txt");
   cbm_k_setlfs(LOGICAL_FILE_NUM,HOST_FILE_SYSTEM,SA_IGNORE_HEADER);
   cbm_k_open();
 
   POKE(0x9f61, BANK); // let's put it in bank 1
   cbm_k_load(LOAD_FLAG, 0xa000);

   cbm_k_close(LOGICAL_FILE_NUM);
}

