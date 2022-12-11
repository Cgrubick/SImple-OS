#include "../../kernel/r2/pcb.h"
#include "../../modules/r3/procsr3.h"
#include "../mpx_supt.h"
#include "string.h"
#include "../include/system.h"
#include "../r1.h"



void infiniteBody();

void infinite(){
  struct pcb* infinite = setup_pcb("infinite",1,0);

  //set register 1 to stack top
  context *infinite_reg = (context *)(infinite -> stack_top);
  memset(infinite_reg, 0, sizeof(struct context));

  //assign registers to register1.
  infinite_reg->fs = 0x10;
  infinite_reg->gs = 0x10;
  infinite_reg->ds = 0x10;
  infinite_reg->es = 0x10;
  infinite_reg->cs = 0x08;
  infinite_reg->ss = 0x10;
  infinite_reg->ebp = (uint32_t)(infinite->stack);
  infinite_reg->esp = (uint32_t)(infinite->stack_top);
  infinite_reg->eip = (uint32_t) infiniteBody;
  infinite_reg->eflags = 0x202;

  insert_pcb(infinite);
}


void infiniteBody(){

  int write = 100;
  while(1) {
    sys_req(WRITE, DEFAULT_DEVICE, "Infinite process is currently running\n", &write);
    sys_req(IDLE, 0, NULL, NULL);
  }

}
