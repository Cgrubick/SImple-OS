#include "../r1.h"
#include "../mpx_supt.h"
//regular include mpx on LOUD Path
//#include "/home/loud/Documents/MPX/include/string.h"
//mpx include on mac
#include "string.h"
#include "help.h"
#include "date.h"
#include "../../kernel/r2/pcb.h"
#include "../../modules/r3/procsr3.h"
#include "../../modules/r3/usercom.h"
#include <core/serial.h>
#include "../../modules/r4/com_hand_proc.h"
#include "../../modules/r5/mcb.h"
#include "../../modules/r4/infinite.h"

#define COM1 0x3f8
void comhand(){

	// MPX logo

			int write = 100;
			sys_req(WRITE,DEFAULT_DEVICE,"\033[0;36m", &write);
			sys_req(WRITE,DEFAULT_DEVICE,"\n___  ___________   __\n", &write);
			sys_req(WRITE,DEFAULT_DEVICE,"|  \\/  || ___ \\ \\ / /\n", &write);
			sys_req(WRITE,DEFAULT_DEVICE,"| .  . || |_/ /\\ V / \n", &write);
			sys_req(WRITE,DEFAULT_DEVICE,"| |\\/| ||  __/ /   \\ \n", &write);
			sys_req(WRITE,DEFAULT_DEVICE,"| |  | || |   / /^\\ \\\n", &write);
			sys_req(WRITE,DEFAULT_DEVICE,"\\_|  |_/\\_|   \\/   \\/  R6\n", &write);
			sys_req(WRITE,DEFAULT_DEVICE,"\033[0;37m", &write);

	int quit = 0;
	while(!quit)
	{
      char buffer[100];
	  memset(buffer,'\0',100);
      int buffer_size = (int)sizeof(buffer);

	  sys_req(READ, COM1,buffer,&buffer_size);
	  //sys_req(WRITE,COM1,buffer,&buffer_size);

	  if(0==strcmp(buffer,"quit\r")){
		sys_req(WRITE,COM1,"Are you sure you would like to quit? Y/N: \n", &buffer_size);
      	char answer[3];
      	memset(answer,'\0',3);
      	int answer_size = (int)sizeof(answer);
      	sys_req(READ, COM1,answer,&answer_size);

      	if(0==strcmp(answer,"Y\r") || 0==strcmp(answer,"y\r")){
			//int exit = 0;
			// while(ready_q->head != NULL || exit == 1){
			// 	if(strcmp(ready_q->head->name, "idle") || strcmp(ready_q->head->name, "com_hand")){
			// 		serial_println("Cannot Remove comhand or idle");
			// 		exit = 1;
			// 	}
			// 	remove_pcb(ready_q->head);
			// }
      		sys_req(EXIT, DEFAULT_DEVICE, NULL, NULL);
      	}
      }

      else if(0==strcmp(buffer,"help\r")){
      	 //sys_req(WRITE,COM1,"help\n",&buffer_size);
      	 help();
	  }

	  else if(0==strcmp(buffer,"version\r")){
      	  sys_req(WRITE,COM1,"The current version is: 2.1 on ",&buffer_size);
      	  print_date(); //__date__
      	  print_time(); //__time__

      	  sys_req(WRITE,COM1,"\n", &buffer_size);
      }

      else if(0==strcmp(buffer,"date\r")){
      	  //sys_req(WRITE,COM1,"date\n",&buffer_size);
      	  print_date();
      	  sys_req(WRITE,COM1,"\n", &buffer_size);
      }

      else if(0==strcmp(buffer,"time\r")){
      	  //sys_req(WRITE,COM1,"date\n",&buffer_size);
      	  print_time();
      	  sys_req(WRITE,COM1,"\n", &buffer_size);
      }

      else if(0==strcmp(buffer,"setdate\r")){
      	  sys_req(WRITE,COM1,"Input date as such '$MM/DD/YY'\n", &buffer_size);

      	  char date[30];
      	  memset(date,'\0',30);
      	  int date_size = (int)sizeof(date);

      	  sys_req(READ,COM1,date,&date_size);
      	  int result = set_date(date);

      	  if(result == 0){
      	  	sys_req(WRITE,COM1,"Invalid Input, Date Not Set!\n", &buffer_size);
      	  }
      	  else {
      	  	sys_req(WRITE,COM1,"Date Succesfully Set!\n", &buffer_size);
      	  }

      }

      else if(0==strcmp(buffer,"settime\r")){
     	  sys_req(WRITE,COM1,"Input time as such '$hours/minutes'\n", &buffer_size);

      	  char time[30];
      	  memset(time,'\0',30);
      	  int time_size = (int)sizeof(time);

      	  sys_req(READ,COM1,time,&time_size);
      	  int result = set_time(time);

      	  if(result == 0){
      	  	sys_req(WRITE,COM1,"Invalid Input, Time Not Set!\n", &buffer_size);
      	  }
      	  else{
      	  	sys_req(WRITE,COM1,"Time Succesfully Set!\n", &buffer_size);
		  }
      }

	  else if(0==strcmp(buffer,"suspendpcb\r")){
		//Tyler
	  	// Puts pcb ins suspended state and moves to suspended queue
	    //Param: user enters process name
      	//Error Checks: name must be valid

		suspend_pcb();
      }

      else if(0==strcmp(buffer,"resumepcb\r")){
		//Tyler
	    //Param: user enters process name
      	//Error Checks: name must be valid
		int buf = 10;
		sys_req(WRITE,COM1,"Please enter name of PCB to resume on next line:\n",&buf);

		char name[10];
		memset(name,'\0',10);
		int name_size = (int)sizeof(name);

		sys_req(READ,COM1,name,&name_size);

		//if result is 1 then valid input 0 is invalid
		if(find_pcb(name) == NULL){
			sys_req(WRITE,COM1,"Error: Invalid PCB name. Please type the command again and use a valid name.\n",&buf);
		}
		resume_pcb(name);
		sys_req(WRITE,COM1,"PCB successfully resumed!\n",&buf);
		//klogv("works");
      }

      else if(0==strcmp(buffer,"setpcbpriority\r")){
	      //Param: user enters process name, and new priority
      	  //Error Checks: name must be valid and priority must be 0-9
      		sys_req(WRITE, COM1, "Please enter the name of the PCB you would like to set the priority of on the next line: \n", &buffer_size);

      		char name[10];
      		memset(name, '\0', 10);
      		int name_size = (int)sizeof(name);


      		sys_req(READ, COM1, name, &name_size);


      		sys_req(WRITE, COM1, "Enter the priority 0-9 for the PCB on the next line:\n", &buffer_size);

      		char priority[2];
      		memset(priority,'\0', 2);
      		int pri_size = (int)sizeof(priority);
      		sys_req(READ, COM1, priority, &pri_size);
      		setpriority(name, priority);

      	  //sys_req(WRITE,COM1,"setpcbpriority\n", &buffer_size);
      }

      else if(0==strcmp(buffer,"showpcb\r")){
      	  //shows name,class,state,suspended status, and priority
	      //Param: user enters process name
      	  //Error Checks: name must be valid
      		sys_req(WRITE, COM1, "Please enter the name of the PCB you would like to show on the next line: \n", &buffer_size);

      		char name[10];
      		memset(name, '\0', 10);
      		int name_size = (int)sizeof(name);

      		sys_req(READ, COM1, name, &name_size);
      		if(find_pcb(name) == NULL) {
				sys_req(WRITE,COM1,"Error: Invalid PCB name. Please type the command again and use a valid name.\n",&buffer_size);
			}
			else{
				struct pcb* ptr_pcb = find_pcb(name);
				showpcb(ptr_pcb);
			}
      }

      else if(0==strcmp(buffer,"showready\r")){
      	//Andrew
      	//shows all processes in ready state, name, class, state, suspeneded statues, priority
	    //Param: none
      	//Error Checks: none
      	show_ready_pcb();
      }

	  else if(0==strcmp(buffer,"showblocked\r")){
	  	//Andrew
		//shows all processes in blocked state, name, class, state, suspeneded statues, priority
	    //Param: none
      	//Error Checks: none
		show_blocked_pcb();
      }

      else if(0==strcmp(buffer,"showall\r")){
		//Tyler
	  	//shows all processes, name, class, state, suspeneded statues, priority
	    //Param: none
      	//Error Checks: none
		show_all_pcb();
      }

	  else if(0==strcmp(buffer,"loadR3\r") || 0==strcmp(buffer,"loadr3\r")){
		//loads the set of processes given for dispatching

			struct pcb *apcb5 = load_proc1();
			if (apcb5 != NULL){
				//serial_print("loading  ");
				//serial_println( apcb5->name);
				insert_pcb(apcb5);
				sys_req(WRITE,COM1, "loaded P1...\n",&buffer_size);
			}

			struct pcb *apcb4 = load_proc2();
			if (apcb4 != NULL){
				//serial_print("loading  ");
				//serial_println( apcb4->name);
				insert_pcb(apcb4);
				sys_req(WRITE,COM1, "loaded P2...\n",&buffer_size);
			}

			struct pcb *apcb3 = load_proc3();
			if (apcb3 != NULL){
				//serial_print("loading  ");
				//serial_println( apcb3->name);
				insert_pcb(apcb3);
				sys_req(WRITE,COM1, "loaded P3...\n",&buffer_size);
			}

			struct pcb *apcb2  = load_proc4();
			if (apcb2 != NULL){
				// serial_print("loading  ");
				// serial_println( apcb2->name);
				insert_pcb(apcb2);
				sys_req(WRITE,COM1, "loaded P4...\n",&buffer_size);
			}
			struct pcb *apcb1 = load_proc5();
			if (apcb1 != NULL){
				// serial_print("loading  ");
				// serial_println( apcb1->name);

				insert_pcb(apcb1);
				sys_req(WRITE,COM1, "loaded P5...\n",&buffer_size);
			}

      	}

	  else if(0==strcmp(buffer,"show free\r")){
		show_all_free();

      }

		else if(0==strcmp(buffer,"infinite\r")){
			infinite();
		}

	  else if(0==strcmp(buffer,"show allocated\r")){
		show_all_allocated();

	  }

	  else if(0==strcmp(buffer,"free memory\r")){
		//clay
		sys_req(WRITE,COM1,"Input MCB location to deallocate:\n", &buffer_size);
      	char hex_loc[12];
      	memset(hex_loc,'\0',12);
      	int loc_size = (int)sizeof(hex_loc);
		sys_req(READ,COM1,hex_loc,&loc_size);
		for(int i = 0; i < 12; i++){
			if(hex_loc[i] == '\r'){
				hex_loc[i] = '\0';
			}
		}

		int* hex_addr = (void*)atoi_hex(hex_loc);
		int result = free_mem(hex_addr);
      	if(result == 1){
      	  	sys_req(WRITE,COM1,"Invalid MCB Address!\n", &buffer_size);
      	}
      	else {
      	  	sys_req(WRITE,COM1,"MCB Successfully Freed!\n", &buffer_size);
      	}

    }

	  else if(0==strcmp(buffer,"allocate memory\r")){
		sys_req(WRITE,COM1,"Input bytes to allocated:\n", &buffer_size);

      	char bytes[10];
      	memset(bytes,'\0',10);
      	int size = (int)sizeof(bytes);
		sys_req(READ,COM1,bytes,&size);

		//filter out carriage returns
		for(int i = 0; i < 6; i++){
			if(bytes[i] == '\r'){
				bytes[i] = '\0';
			}
		}


		int real_bytes = atoi(bytes);


      	alloc_mem((uint32_t)real_bytes);

    	}

    	else{
			sys_req(WRITE,COM1,"Command not found\n",&buffer_size);
  			//interrupt

		} 
		sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);

	}


}
