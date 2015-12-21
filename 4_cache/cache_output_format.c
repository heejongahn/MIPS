#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cache.h"

#define BYTES_PER_WORD 4

int capacity;
int way;
int blocksize;
block** cache;
uint32_t PC;
int set;
int words;
int total_reads = 0;
int total_writes = 0;
int write_backs = 0;
int reads_hits = 0;
int write_hits = 0;
int reads_misses = 0;
int write_misses = 0;

/***************************************************************/
/*                                                             */
/* Procedure : cdump                                           */
/*                                                             */
/* Purpose   : Dump cache configuration                        */
/*                                                             */
/***************************************************************/
void cdump(int capacity, int assoc, int blocksize){

    printf("Cache Configuration:\n");
    printf("-------------------------------------\n");
    printf("Capacity: %dB\n", capacity);
    printf("Associativity: %dway\n", assoc);
    printf("Block Size: %dB\n", blocksize);
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : sdump                                           */
/*                                                             */
/* Purpose   : Dump cache stat                       */
/*                                                             */
/***************************************************************/
void sdump(int total_reads, int total_writes, int write_backs,
    int reads_hits, int write_hits, int reads_misses, int write_misses) {
    printf("Cache Stat:\n");
    printf("-------------------------------------\n");
    printf("Total reads: %d\n", total_reads);
    printf("Total writes: %d\n", total_writes);
    printf("Write-backs: %d\n", write_backs);
    printf("Read hits: %d\n", reads_hits);
    printf("Write hits: %d\n", write_hits);
    printf("Read misses: %d\n", reads_misses);
    printf("Write misses: %d\n", write_misses);
    printf("\n");
}


/***************************************************************/
/*                                                             */
/* Procedure : xdump                                           */
/*                                                             */
/* Purpose   : Dump current cache state                        */
/*                                   */
/* Cache Design                               */
/*                                   */
/*        cache[set][assoc][word per block]               */
/*                                   */
/*                                   */
/*       ----------------------------------------           */
/*       I        I  way0  I  way1  I  way2  I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set0  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set1  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*                                   */
/*                                                             */
/***************************************************************/
void xdump(int set, int way, block** cache)
{
    int i,j,k = 0;

    printf("Cache Content:\n");
    printf("-------------------------------------\n");
    for(i = 0; i < way;i++)
    {
        if(i == 0)
        {
            printf("    ");
        }
        printf("      WAY[%d]",i);
    }
    printf("\n");

    for(i = 0 ; i < set;i++)
    {
        printf("SET[%d]:   ",i);
        for(j = 0; j < way;j++)
        {
            if(k != 0 && j == 0)
            {
                printf("          ");
            }
            printf("0x%08x  ", cache[i][j].address);
        }
        printf("\n");
    }
    printf("\n");
}




int main(int argc, char *argv[]) {
    int i, j, k;

    bool print_cache = false;

    FILE *inputFile;
    char type;
    uint32_t addr;

    for(i = 1; i < argc; i++) {
        for(j = 0; j < strlen(argv[i]); j++) {
              if(!strcmp(argv[i], "-x")) {
                  print_cache = true;
              }
              else if(!strcmp(argv[i], "-c")) {
                  i++;
                  sscanf(argv[i], "%d:%d:%d", &capacity, &way, &blocksize);
              }
              else {
                  inputFile = fopen(argv[i], "r");
              }
        }
    }
    set = capacity/way/blocksize;
    words = blocksize / BYTES_PER_WORD;

    // Cache Initialization
    cache = (block**) malloc (sizeof(block*) * set);

    for(i = 0; i < set; i++) {
        cache[i] = (block*) malloc(sizeof(block) * way);
    }

    for(i = 0; i < set; i++) {
        for(j = 0; j < way; j ++)
            cache[i][j].valid = 0;
            cache[i][j].dirty = 0;
            cache[i][j].tag = 0;
            cache[i][j].address = 0;
            cache[i][j].used_at = 0;
    }

    while(fscanf(inputFile, "%c %x\n", &type, &addr) != EOF) {
        access(type, addr);
    }

    // test example
    cdump(capacity, way, blocksize);
    sdump(total_reads, total_writes, write_backs, read_hits, write_hits, read_misses, write_misses);
    if (print_cache) {
        xdump(set, way, cache);
    }

    return 0;
}
