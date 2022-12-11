/*
  ----- kmain.c -----

  Description..: Kernel main. The first function called after
      the bootloader. Initialization of hardware, system
      structures, devices, and initial processes happens here.

      Initial Kernel -- by Forrest Desjardin, 2013,
      Modifications by:    Andrew Duncan 2014,  John Jacko 2017
      				Ben Smith 2018, and Alex Wilson 2019
*/

#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include <core/tables.h>
#include <core/interrupts.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include "modules/r1.h"
#include "modules/r4/com_hand_proc.h"
#include "modules/mpx_supt.h"
#include "../r2/pcb.h"
#include "modules/r5/mcb.h"


#define COM1 0x3f8


void comhand_process();
void idle_process();


void kmain(void)
{
  extern uint32_t magic;
  // Uncomment if you want to access the multiboot header
  // extern void *mbd;
  // char *boot_loader_name = (char*)((long*)mbd)[16];


  // 0) Initialize Serial I/O
  // functions to initialize serial I/O can be found in serial.c
  // there are 3 functions to call
  init_serial(COM1);
  set_serial_out(COM1);
  set_serial_in(COM1);
  klogv("Starting MPX boot sequence...");
  klogv("Initialized serial I/O on COM1 device...");

  // 1) Initialize the support software by identifying the current
  //     MPX Module.  This will change with each module.
  // you will need to call mpx_init from the mpx_supt.c


  // Initialization complete
  // 2) Check that the boot was successful and correct when using grub
  // Comment this when booting the kernel directly using QEMU, etc.
  if ( magic != 0x2BADB002 ){
    //kpanic("Boot was not error free. Halting.");
  }

  // 3) Descriptor Tables -- tables.c
  //  you will need to initialize the global
  // this keeps track of allocated segments and pages
  klogv("Initializing descriptor tables...");
  init_gdt();
  init_idt();
  init_irq();
  init_pic();
  sti();

  //serial_println("  ___  __  ____  ____  ____  ____  __  ____  _  _ ");
  //serial_println(" / __)/  \\(    \\(  __)/ ___)(_  _)/  \\(  _ \\( \\// )");
  //serial_println("( (__(  O )) D ( ) _) \\___ \\  )( (  O ))   // \\// \\");
  //serial_println(" \\___)\\__/(____/(____)(____/ (__) \\__/(__\\_)\\_)(_//");


  // 4)  Interrupt vector table --  tables.c
  // this creates and initializes a default interrupt vector table
  // this function is in tables.c

  klogv("Interrupt vector table initialized!");

  // 5) Virtual Memory -- paging.c  -- init_paging
  //  this function creates the kernel's heap
  //  from which memory will be allocated when the program calls
  // sys_alloc_mem UNTIL the memory management module  is completed
  // this allocates memory using discrete "pages" of physical memory
  // NOTE:  You will only have about 70000 bytes of dynamic memory
  //

  klogv("Initializing virtual memory...");
  init_paging();
  init_queues();
  // 6) Call YOUR command handler -  interface method
  klogv("Transferring control to commhand...");


  mpx_init(MEM_MODULE);
  sys_set_malloc((void*)alloc_mem);
  sys_set_free(free_mem);

  mpx_init(MODULE_R5);
  init_heap((size_t) 50000);


  comhand_process();


  idle_process();
  asm volatile("int $0x60");


  

  //interrupt
  // show_all_allocated();
  // show_all_free();
  // sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);

  // 7) System Shutdown on return from your command handler
  klogv("Starting system shutdown procedure...");

  /* Shutdown Procedure */
  klogv("Shutdown complete. You may now turn off the machine. (QEMU: C-a x)");
  hlt();
}

void comhand_process(){

  struct pcb* comhand_process = setup_pcb("comhand", 1, 0);
  struct context* comhand_reg = (struct context*)(comhand_process->stack_top);
  memset(comhand_reg, 0, sizeof(struct context));
  comhand_reg->fs = 0x10;
  comhand_reg->ds = 0x10;
  comhand_reg->es = 0x10;
  comhand_reg->cs = 0x08;
  comhand_reg->ss = 0x10;
  comhand_reg->ebp = (uint32_t)(comhand_process->stack);
  comhand_reg->esp = (uint32_t)(comhand_process->stack_top);
  comhand_reg->eip = (uint32_t) comhand;
  comhand_reg->eflags = 0x0202;
  insert_pcb(comhand_process);
}



void idle_process(){

  struct pcb* idle_process = setup_pcb("idle", 1, 0);

  struct context* idle_reg = (struct context*)(idle_process->stack_top);
  memset(idle_reg, 0, sizeof(struct context));
  idle_reg->fs = 0x10;
  idle_reg->gs = 0x10;
  idle_reg->ds = 0x10;
  idle_reg->es = 0x10;
  idle_reg->cs = 0x08;
  idle_reg->ss = 0x10;
  idle_reg->ebp = (uint32_t)(idle_process->stack);
  idle_reg->esp = (uint32_t)(idle_process->stack_top);
  idle_reg->eip = (uint32_t) idle;
  idle_reg->eflags = 0x0202;
  insert_pcb(idle_process);
}
