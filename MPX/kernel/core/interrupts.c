/*
  ----- interrupt.c -----

  Description..: Interrupt handling routines for
    traps, gates, exceptions.
*/

#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include <core/tables.h>
#include <core/interrupts.h>
#include "../r2/pcb.h"
#include "modules/mpx_supt.h"
#include "string.h"

// Programmable Interrupt Controllers
#define PIC1 0x20
#define PIC2 0xA0

// Initialization Code Words
#define ICW1 0x11
#define ICW4 0x01

/*
  Procedure..: io_wait
  Description..: The i386 can do an io wait by accessing another port.
      Mainly used in initializing the PIC.
*/
#define io_wait() asm volatile ("outb $0x80")

struct pcb* COP = NULL;



context* old_context = NULL;


extern void divide_error();
extern void debug();
extern void nmi();
extern void breakpoint();
extern void overflow();
extern void bounds();
extern void invalid_op();
extern void device_not_available();
extern void double_fault();
extern void coprocessor_segment();
extern void invalid_tss();
extern void segment_not_present();
extern void stack_segment();
extern void general_protection();
extern void page_fault();
extern void reserved();
extern void coprocessor();
extern void rtc_isr();
extern void sys_call_isr();
extern uint32_t* sys_call(context* registers);

extern idt_entry idt_entries[256];

//Current serial handler
extern void isr0();
void do_isr()
{
  char in = inb(COM2);
  serial_print(&in);
  serial_println("here");
  outb(0x20,0x20); //EOI
}

/*
  Procedure..: irq_init
  Description..: Installs the initial interrupt handlers for
      the first 32 irq lines. Most do a panic for now.
*/
void init_irq(void)
{
  int i;

  // Necessary interrupt handlers for protected mode
  uint32_t isrs[17] = {
    (uint32_t)divide_error,
    (uint32_t)debug,
    (uint32_t)nmi,
    (uint32_t)breakpoint,
    (uint32_t)overflow,
    (uint32_t)bounds,
    (uint32_t)invalid_op,
    (uint32_t)device_not_available,
    (uint32_t)double_fault,
    (uint32_t)coprocessor_segment,
    (uint32_t)invalid_tss,
    (uint32_t)segment_not_present,
    (uint32_t)stack_segment,
    (uint32_t)general_protection,
    (uint32_t)page_fault,
    (uint32_t)reserved,
    (uint32_t)coprocessor
  };

  // Install handlers; 0x08=sel, 0x8e=flags
  for(i=0; i<32; i++){
    if (i<17) idt_set_gate(i, isrs[i], 0x08, 0x8e);
    else idt_set_gate(i, (uint32_t)reserved, 0x08, 0x8e);
  }
  // Ignore interrupts from the real time clock
  idt_set_gate(0x60, (uint32_t)sys_call_isr, 0x08, 0x8e);
  idt_set_gate(0x08, (uint32_t)rtc_isr, 0x08, 0x8e);
}

/*
  Procedure..: pic_init
  Description..: Initializes the programmable interrupt controllers
      and performs the necessary remapping of IRQs. Leaves interrupts
      turned off.
*/
void init_pic(void)
{
  outb(PIC1,ICW1);   //send initialization code words 1 to PIC1
  io_wait();
  outb(PIC2,ICW1);   //send icw1 to PIC2
  io_wait();
  outb(PIC1+1,0x20); //icw2: remap irq0 to 32
  io_wait();
  outb(PIC2+1,0x28); //icw2: remap irq8 to 40
  io_wait();
  outb(PIC1+1,4);    //icw3
  io_wait();
  outb(PIC2+1,2);    //icw3
  io_wait();
  outb(PIC1+1,ICW4); //icw4: 80x86, automatic handling
  io_wait();
  outb(PIC2+1,ICW4); //icw4: 80x86, automatic handling
  io_wait();
  outb(PIC1+1,0xFF); //disable irqs for PIC1
  io_wait();
  outb(PIC2+1,0xFF); //disable irqs for PIC2
}



uint32_t* sys_call(context* registers){
      //checking if sys call has been called before
 // COP->next_pcb = NULL;

  if(params.op_code != EXIT && params.op_code != IDLE){
    //serial_println("invalid params");
    return (uint32_t*) registers;
  }
  struct pcb* ready_head = ready_q->head;
  remove_pcb(ready_head);


  if(old_context == NULL){

    old_context = registers;

  }

  else{

    //if == idle, save the context (reassign cops stack top )
    //if == exit, free cop aka remove or make it null??
    if(params.op_code == IDLE){


      //move current process to end of queue
      //switch cop to next in ready queue
      //set cop's state to ready
      //memcpy(COP->stack_top,registers,sizeof(*registers));
      //
      // if(COP != NULL && ready_q->head == NULL){
      //   return (uint32_t *) COP->stack_top;
      // }
      COP->stack_top = (unsigned char*)registers;
      COP->exec_state = 0;
      insert_pcb(COP);

    }

    else if(params.op_code == EXIT){
      // COP = ready_head;
      //free COP
      //switch to next in ready queue
      free_pcb(COP);
    }
  }

  if(ready_head == NULL){
    return (uint32_t*) old_context;
  }
  else {
    COP = ready_head;
    COP->exec_state = 2;
    return (uint32_t *) COP->stack_top;
  }
}






void do_divide_error()
{
  kpanic("Division-by-zero");
}
void do_debug()
{
  kpanic("Debug");
}
void do_nmi()
{
  kpanic("NMI");
}
void do_breakpoint()
{
  kpanic("Breakpoint");
}
void do_overflow()
{
  kpanic("Overflow error");
}
void do_bounds()
{
  kpanic("Bounds error");
}
void do_invalid_op()
{
  kpanic("Invalid operation");
}
void do_device_not_available()
{
  kpanic("Device not available");
}
void do_double_fault()
{
  kpanic("Double fault");
}
void do_coprocessor_segment()
{
  kpanic("Coprocessor segment error");
}
void do_invalid_tss()
{
  kpanic("Invalid TSS");
}
void do_segment_not_present()
{
  kpanic("Segment not present");
}
void do_stack_segment()
{
  kpanic("Stack segment error");
}
void do_general_protection()
{
  kpanic("General protection fault");
}
void do_page_fault()
{
  kpanic("Page Fault");
}
void do_reserved()
{
  serial_println("die: reserved");
}
void do_coprocessor()
{
  kpanic("Coprocessor error");
}
