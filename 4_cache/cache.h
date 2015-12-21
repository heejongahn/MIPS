typedef struct cache_block {
    char valid;
    char dirty;
    uint32_t tag;
    uint32_t address;
    uint32_t used_at;
} block;

extern block** cache;
extern uint32_t PC;
extern int capacity;
extern int way;
extern int blocksize;
extern int set;
extern int words;

extern int total_reads;
extern int total_writes;
extern int write_backs;
extern int read_hits;
extern int write_hits;
extern int read_misses;
extern int write_misses;

void access(char x, uint32_t addr);
