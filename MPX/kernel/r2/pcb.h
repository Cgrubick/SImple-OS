#ifndef PCB_H
#define PCB_H

#include "string.h"

struct context{
	uint32_t ds, es, fs, gs, ss;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t eip, cs, eflags;
};

struct pcb {
	char name[10]; //name of pcb
	int class; //user application with be '1' and system will be '0'
	int priority; // 0 (highest) - 9 (lowest)
	int exec_state; //0 for ready, 1 for blocked, and 2 for running
	int dispatch_state; //0 for suspened and 1 for not suspened
	unsigned char *stack;
	unsigned char *stack_top; // this becomes the "stack" + 1024
	struct pcb* next_pcb;
};

extern struct queue {
	struct pcb* head;
	struct pcb* tail;
	int count;

}QUEUE;


typedef struct context context;


extern struct pcb* COP;
extern context* old_context;


void init_queues(void);
struct pcb* find_pcb(char *find_name);
int insert_pcb(struct pcb* pcb_insert);
struct pcb* setup_pcb(char pcb_name[10], int class, int priority);
struct pcb* allocate_pcb();
int free_pcb(struct pcb* pcbtofree);
int remove_pcb(struct pcb* target);
void showpcb(struct pcb* pcbtoshow);
int is_empty(struct queue* queuePtr);
int block_pcb(char *block_name);
int unblock_pcb(char *unblock_name);
void suspend_pcb();
void resume_pcb();
void show_ready_pcb();
void show_blocked_pcb();
void show_all_pcb();
int dequeue(struct pcb* target);
//void print_elements(struct queue* queuePtr);
void setpriority(char *name, char *priority);
//void unblock(char *name);

extern struct queue* ready_q;
extern struct queue* blocked_q;
extern struct queue* sus_ready_q;
extern struct queue* sus_block_q;


#endif
