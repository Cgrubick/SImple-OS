
#ifndef MCBH
#define MCBH



extern struct mcb {
    unsigned char* start_addr;
    size_t size;
    int mem_state; //1 for allocated, 0 for free
    struct mcb* next;
    struct mcb* prev;
}MCB;


extern struct mcb* heap_list;
//extern struct mcb* allocated_list;
//extern struct mcb* free_list;

void init_heap(size_t bytes);
void* alloc_mem(uint32_t num_bytes);
int free_mem(void* target_addr);
void show_all_free();
void show_all_allocated();
uint32_t atoi_hex(const char *s);

#endif