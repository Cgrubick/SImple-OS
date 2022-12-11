#include "../../kernel/r2/pcb.h"
#include "../../modules/r3/procsr3.h"
#include "../mpx_supt.h"
#include "string.h"
#include "../include/system.h"
#include "../r1.h"





struct pcb* load_proc1(){
    
    struct pcb* new_pcb = setup_pcb("p1", 1, 9);
    //klogv("hello");
   
    struct context* cp = (struct context*)(new_pcb->stack_top);
    memset(cp, 0, sizeof(struct context));
    cp->fs = 0x10;
    cp->gs = 0x10;
    cp->ds = 0x10;
    cp->es = 0x10;
    cp->cs = 0x08;
    cp->ss = 0x10;
    cp->ebp = (uint32_t)(new_pcb->stack);
    cp->esp = (uint32_t)(new_pcb->stack_top);
    cp->eip = (uint32_t) proc1;
    cp->eflags = 0x0202;
    return new_pcb;
}

struct pcb* load_proc2(){
    struct pcb* new_pcb = setup_pcb("p2", 1, 1);
    struct context* cp = (struct context*)(new_pcb->stack_top);
    memset(cp, 0, sizeof(struct context));
    cp->fs = 0x10;
    cp->gs = 0x10;
    cp->ds = 0x10;
    cp->es = 0x10;
    cp->cs = 0x08;
    cp->ss = 0x10;
    cp->ebp = (uint32_t)(new_pcb->stack);
    cp->esp = (uint32_t)(new_pcb->stack_top);
    cp->eip = (uint32_t) proc2;
    cp->eflags = 0x202;
    return new_pcb;
}

struct pcb* load_proc3(){
    struct pcb* new_pcb = setup_pcb("p3", 1, 1);
    struct context* cp = (struct context*)(new_pcb->stack_top);
    memset(cp, 0, sizeof(struct context));
    cp->fs = 0x10;
    cp->gs = 0x10;
    cp->ds = 0x10;
    cp->es = 0x10;
    cp->cs = 0x08;
    cp->ss = 0x10;
    cp->ebp = (uint32_t)(new_pcb->stack);
    cp->esp = (uint32_t)(new_pcb->stack_top);
    cp->eip = (uint32_t) proc3;
    cp->eflags = 0x202;
    return new_pcb;
}

struct pcb* load_proc4(){
    struct pcb* new_pcb = setup_pcb("p4", 0, 0);
    struct context* cp = (struct context*)(new_pcb->stack_top);
    memset(cp, 0, sizeof(struct context));
    cp->fs = 0x10;
    cp->gs = 0x10;
    cp->ds = 0x10;
    cp->es = 0x10;
    cp->cs = 0x08;
    cp->ss = 0x10;
    cp->ebp = (uint32_t)(new_pcb->stack);
    cp->esp = (uint32_t)(new_pcb->stack_top);
    cp->eip = (uint32_t) proc4;
    cp->eflags = 0x202;
    return new_pcb;
    
}

struct pcb* load_proc5(){
    struct pcb* new_pcb = setup_pcb("p5", 0, 9);
    struct context* cp = (struct context*)(new_pcb->stack_top);
    memset(cp, 0, sizeof(struct context));
    cp->fs = 0x10;
    cp->gs = 0x10;
    cp->ds = 0x10;
    cp->es = 0x10;
    cp->cs = 0x08;
    cp->ss = 0x10;
    cp->ebp = (uint32_t)(new_pcb->stack);
    cp->esp = (uint32_t)(new_pcb->stack_top);
    cp->eip = (uint32_t) proc5;
    cp->eflags = 0x202;
    return new_pcb;
    
}


struct pcb* load_com_hand(){
    
    struct pcb* new_pcb = setup_pcb("com_hand", 0 , 0);
    //klogv("hello");
   
    struct context* cp = (struct context*)(new_pcb->stack_top);
    memset(cp, 0, sizeof(struct context));
    cp->fs = 0x10;
    cp->gs = 0x10;
    cp->ds = 0x10;
    cp->es = 0x10;
    cp->cs = 0x08;
    cp->ss = 0x10;
    cp->ebp = (uint32_t)(new_pcb->stack);
    cp->esp = (uint32_t)(new_pcb->stack_top);
    cp->eip = (uint32_t)comhand;
    cp->eflags = 0x0202;
    return new_pcb;
}


// struct pcb* load_alarm(){
    
//     struct pcb* new_pcb = setup_pcb("alarm", SYSTEM, 1);
//     //klogv("hello");
   
//     struct context* cp = (struct context*)(new_pcb->stack_top);
//     memset(cp, 0, sizeof(struct context));
//     cp->fs = 0x10;
//     cp->gs = 0x10;
//     cp->ds = 0x10;
//     cp->es = 0x10;
//     cp->cs = 0x08;
//     cp->ss = 0x10;
//     cp->ebp = (uint32_t)(new_pcb->stack);
//     cp->esp = (uint32_t)(new_pcb->stack_top);
//     cp->eip = (uint32_t) comhand;
//     cp->eflags = 0x0202;
//     return new_pcb;
// }

// struct pcb* load_infinite(){
    
//     struct pcb* new_pcb = setup_pcb("infinite", SYSTEM, 1);
//     //klogv("hello");
   
//     struct context* cp = (struct context*)(new_pcb->stack_top);
//     memset(cp, 0, sizeof(struct context));
//     cp->fs = 0x10;
//     cp->gs = 0x10;
//     cp->ds = 0x10;
//     cp->es = 0x10;
//     cp->cs = 0x08;
//     cp->ss = 0x10;
//     cp->ebp = (uint32_t)(new_pcb->stack);
//     cp->esp = (uint32_t)(new_pcb->stack_top);
//     cp->eip = (uint32_t) alarm;
//     cp->eflags = 0x0202;
//     return new_pcb;
// }

// struct pcb* load_idle(){
    
//     struct pcb* new_pcb = setup_pcb("idle", 0, 9);
    
//     struct context* cp = (struct context*)(new_pcb->stack_top);
//     memset(cp, 0, sizeof(struct context));
//     cp->fs = 0x10;
//     cp->gs = 0x10;
//     cp->ds = 0x10;
//     cp->es = 0x10;
//     cp->cs = 0x08;
//     cp->ss = 0x10;
//     cp->ebp = (uint32_t)(new_pcb->stack);
//     cp->esp = (uint32_t)(new_pcb->stack_top);
//     cp->eip = (uint32_t)idle;
//     cp->eflags = 0x0202;
//     return new_pcb;
// }


