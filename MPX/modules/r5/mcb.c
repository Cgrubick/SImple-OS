
#include "string.h"
#include "../../include/mem/heap.h"
#include "mcb.h"
#include "../../include/core/serial.h"
#include "../r1/date.h"
#include "../mpx_supt.h"

struct mcb* heap_list = NULL;
#define ALLOCATE 1
#define FREE 0
void init_heap(size_t bytes){
    //allocates all memory available to your memory manager as a single large block
    //using kmalloc() allocate the requested size plus room for the MCB
    //fill in the MCB at the bottom of the heap
    //point the head of the free list to this block
    //init the allocated list to be empty 

    char buffer[100];
	memset(buffer,'\0',100);
    int buffer_size = (int)sizeof(buffer);
    if(bytes <= 0 ){
        sys_req(WRITE,COM1,"Heap not initalized\n", &buffer_size);
        return;
    }
    struct mcb* head = (struct mcb*)kmalloc(bytes);
    head->size = bytes;
    head->start_addr = (unsigned char*)((unsigned char*)head + sizeof(*head));
    head->mem_state = FREE;
    head->next = NULL;
    head->prev = NULL;

    heap_list = head;
}

void* alloc_mem(uint32_t num_bytes){
    //alloccates the memory from the heap
    //returns void* pointer to the allcoated mem or NULL on error
    //first fit method?
    //move the mcb to allocated list
    char buffer[100];
	memset(buffer,'\0',100);
    int buffer_size = (int)sizeof(buffer);
    if(num_bytes <= 0){
        sys_req(WRITE,COM1,"Invalid number of bytes\n", &buffer_size);
        return NULL;
    }

    struct mcb* allocated_mcb = heap_list;
    //iterate through heap until find a suitable mcb size
    while(allocated_mcb != NULL){
        if(allocated_mcb->size > num_bytes && allocated_mcb->mem_state == 0){
            struct mcb* free_mcb = (struct mcb*)(num_bytes + (unsigned char*)allocated_mcb->start_addr);
            size_t temp_size = allocated_mcb->size;
            allocated_mcb->size = num_bytes;
            allocated_mcb->mem_state = ALLOCATE;
            allocated_mcb->next = free_mcb;
            //setting start addrress to heap start address plus new mcb size
            free_mcb->size =  temp_size - num_bytes; 
            free_mcb->start_addr = (unsigned char*)((unsigned char*)free_mcb + sizeof(free_mcb) + num_bytes);
            free_mcb->mem_state = FREE;
            free_mcb->next = NULL;
            free_mcb->prev = allocated_mcb;
            //redefining allocated pointers
            return (void*)(uint32_t)allocated_mcb->start_addr;
        }

        allocated_mcb = allocated_mcb->next;
    }
   
	sys_req(WRITE,COM1,"Requested block is too large! \n",&buffer_size);
    return NULL;
    
    
    
    

}

// Frees Allocated Memory
// Takes in a void ptr to the block that is to be freed
// Returns an int indicating 0 success or 1 failure
int free_mem(void* target_addr){
    // Move the MCB to the free list
    // If the freed block is adjacent to any other freed blocks, they must be merged into a single free block

    // Checks if target_addr is NULL
    if(target_addr == NULL || target_addr <= 0){
      return 1;
    }

    // Checks if heap is empty
    if(heap_list == NULL) {
      return 1;
    }

    struct mcb* curr = heap_list;
    
    while(target_addr != curr->start_addr) {
      if(curr == NULL){
        return 1;
      }
      curr = curr->next;
      
    }

    // Update current MCB to free and adjust the size
    curr->mem_state = 0;
    heap_list->size = heap_list->size + curr->size;

    // Check if the PREV MCB is a free block
    struct mcb* temp_previous = curr->prev;
    if(temp_previous->mem_state == FREE) {
      // Add size of prev to current
      curr->size = curr->size + temp_previous->size;
      // Change currents prev to previous' prev
      curr->prev = temp_previous->prev;
      // Sets previous' prev and next to NULL
      temp_previous->next = NULL;
      temp_previous->prev = NULL;
      // Free the merged MCB
      //sys_free_mem(temp_previous);
    }

    //Check if the NEXT MCB is a free block
    struct mcb* temp_next = curr->next;
    if(temp_next->mem_state == FREE) {
      // Add sizes together
      curr->size = curr->size + temp_next->size;
      // Changing start address of curr to be temp_next start address
      curr->start_addr = temp_next->start_addr;
      // Change currents next to next's next
      curr->next = temp_next->next;
      // Sets temp_next's prev and next to NULL
      temp_next->next = NULL;
      temp_next->prev = NULL;
      // Free the merged MCB
      //sys_free_mem(temp_next);
    }

    return 0;
 }

void show_all_free(){
    char buffer[100];
	memset(buffer,'\0',100);
    int buffer_size = (int)sizeof(buffer);

    struct mcb* curr = heap_list;
   
    while(curr != NULL ){
        if(curr->mem_state == FREE){
            sys_req(WRITE,COM1,"********************\n", &buffer_size);
            sys_req(WRITE,COM1,"Free MCB at address: 0x", &buffer_size);
            char  start_addr[20];
            char  *hex_start_addr = itoa((int)curr->start_addr,start_addr,16);
            sys_req(WRITE,COM1, hex_start_addr, &buffer_size);
            // sys_req(WRITE,COM1,"\n", &buffer_size);
            // char  *int_start_addr = itoa((int)curr->start_addr,start_addr,10);
            // sys_req(WRITE,COM1, int_start_addr, &buffer_size);
            sys_req(WRITE,COM1,", MCB Size: ", &buffer_size);
            char size[20];
            char *real_size_mem = itoa((int)curr->size,size,10);
            sys_req(WRITE,COM1,real_size_mem, &buffer_size);
            sys_req(WRITE,COM1,"\n", &buffer_size);
            sys_req(WRITE,COM1,"********************\n", &buffer_size);
        }
        curr = curr->next;
    }
 

   
}

void show_all_allocated(){
    char buffer[100];
	memset(buffer,'\0',100);
    int buffer_size = (int)sizeof(buffer);
    
    
    struct mcb* curr = heap_list;
    sys_req(WRITE,COM1,"Allocated MCBs:\n", &buffer_size);
    while(curr != NULL){
       if(curr->mem_state == ALLOCATE){sys_req(WRITE,COM1,"********************\n", &buffer_size);
            sys_req(WRITE,COM1,"MCB at address: 0x", &buffer_size);
            char start_addr[20];
            char  *hex_start_addr = itoa((int)curr->start_addr,start_addr,16);
            sys_req(WRITE,COM1, hex_start_addr, &buffer_size);
            // sys_req(WRITE,COM1,"\n", &buffer_size);
            // char  *int_start_addr = itoa((int)curr->start_addr,start_addr,10);
            // sys_req(WRITE,COM1, int_start_addr, &buffer_size);
            sys_req(WRITE,COM1,", MCB Size: ", &buffer_size);
            char size[20];
            char *real_size_mem = itoa((int)curr->size,size,10);
            sys_req(WRITE,COM1,real_size_mem, &buffer_size);
            sys_req(WRITE,COM1,"\n", &buffer_size);
            sys_req(WRITE,COM1,"********************\n", &buffer_size);
        }
        curr = curr->next;
    }
    sys_req(WRITE,COM1,"End of Allocated\n", &buffer_size);
}


uint32_t atoi_hex(const char *s){
    uint32_t charVal = 0;
    while(*s != '\0'){
        uint8_t byte = *s++;
        if(byte >= '0' && byte <= '9') {
            byte = byte - '0';
        }
        else if(byte >= 'a' && byte <= 'f') {
            byte = byte - 'a' + 10;
        }
        else if(byte >= 'A' && byte <= 'F') {
            byte = byte - 'A' + 10;
        }
        charVal = (charVal << 4) | (byte & 0xF);
    }
    return charVal;
}