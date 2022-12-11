#include "pcb.h"
#include "../../modules/mpx_supt.h"
#include "string.h"
#include "../../include/core/serial.h"
#include "../../modules/r1/date.h"
#include "string.h"

#define COM1 0x3f8



struct queue* ready_q = NULL;
struct queue* blocked_q = NULL;
struct queue* sus_ready_q = NULL;
struct queue* sus_block_q = NULL;
struct queue QUEUE;

param params;
struct pcb* allocate_pcb(){
	//jacob
	struct pcb *allocatedpcb = sys_alloc_mem(sizeof(struct pcb));
	allocatedpcb->stack = sys_alloc_mem(1024);
	allocatedpcb->stack_top = allocatedpcb->stack + 1024 - sizeof(struct context);
	return allocatedpcb; //return pointer to the new allocated pcb
}

int free_pcb(struct pcb* pcbtofree){
	// if(strcmp(pcbtofree->name, "idle") || strcmp(pcbtofree->name, "com_hand")){
	// 	serial_println("Cannot Remove comhand or idle");
	// 	return 1;
	// }
	int indicator;
	indicator = sys_free_mem(pcbtofree);
	return indicator; //the return value should be an int, 0 for success, else error
}
struct pcb* setup_pcb(char pcb_name[10], int class, int priority){
	//clay 9/16/22 3pm

	//error codes - error allocating,initlizing, or invalid parameter
	struct pcb* ptr_pcb = allocate_pcb();
	//checks if allocation was succesful if not returns null
	if(ptr_pcb == NULL){
		return NULL;
	}

	//exec state at 0 is ready
	//dispatch state at 1 is not suspended


	if(priority < 0 || priority > 9){
		//klogv("Priority error");
		return NULL;
	}
	if( (class != 0) && (class != 1) ){
		// klogv("class error");
		return NULL;
	}
	if(pcb_name[0] == '\0'){
		return NULL;
	}
	ptr_pcb->class = class;
		// klogv("Class done");

	strcpy(ptr_pcb->name,pcb_name);
		// klogv("Name done");

	ptr_pcb->dispatch_state = 1;
		// klogv("Dispatch done");

	ptr_pcb->exec_state = 0;
		// klogv("Exec done");

	ptr_pcb->priority = priority;
		// klogv("Priority done");
	int buf = 10;


	if(find_pcb(ptr_pcb->name) != NULL ){
		sys_req(WRITE, COM1, "PCB name already in use! \n", &buf);
		return NULL;
	}

	return ptr_pcb;

}
struct pcb* find_pcb(char *find_name){
	//Andrew
	//Set a pointer to the head of the ready queue
	int i = 0;
	char name_of_pcb[10];
	memset(name_of_pcb,'\0',10);
    int pcb_name_size = (int)sizeof(name_of_pcb);

	//fills pcb string with pcb name until there is a space or until the index is greater
	while(find_name[i] != '\r' && i < pcb_name_size){
		name_of_pcb[i] = find_name[i];
		i++;
	}

	if(name_of_pcb[0] == '\0' || name_of_pcb[0] == ' '){
		return NULL;
	}


	struct pcb* ptr_tmp_ready = ready_q->head;
	struct pcb* ptr_tmp_sus_ready = sus_ready_q->head;
	struct pcb* ptr_tmp_blocked = blocked_q->head;
	struct pcb* ptr_tmp_sus_block = sus_block_q->head;


	while(ptr_tmp_ready != NULL) {
		//Checks if the parameter matches the name of a process in the ready queue
		if (0 == strcmp(name_of_pcb,ptr_tmp_ready->name)){

			//If so, return the pointer
			return ptr_tmp_ready;
		} else {
			//If not, go to the next process in the ready queue and try again
			ptr_tmp_ready = ptr_tmp_ready->next_pcb;
		}
	}

	//Set a pointer to the head of the blocked queue
	while(ptr_tmp_blocked != NULL) {
		//Checks if the parameter matches the name of a process in the blocked queue
		if (0 == strcmp(name_of_pcb,ptr_tmp_blocked->name)){
			//If so, return the pointer
			return ptr_tmp_blocked;
		} else {
			//If not, go to the next process in the blocked queue and try again
			ptr_tmp_blocked = ptr_tmp_blocked->next_pcb;
		}
	}

	//Set a pointer to the head of the suspended ready queue
	while(ptr_tmp_sus_ready != NULL) {
		//Checks if the parameter matches the name of a process in the suspended ready queue
		if (0 == strcmp(name_of_pcb,ptr_tmp_sus_ready->name)){
			//If so, return the pointer
			return ptr_tmp_sus_ready;
		} else {
			//If not, go to the next process in the suspended ready queue and try again
			ptr_tmp_sus_ready = ptr_tmp_sus_ready->next_pcb;
		}
	}

	//Set a pointer to the head of the suspended blocked queue
	while(ptr_tmp_sus_block != NULL) {
		//Checks if the parameter matches the name of a process in the suspended blocked queue
		if (0 == strcmp(name_of_pcb,ptr_tmp_sus_block->name)){
			//If so, return the pointer
			return ptr_tmp_sus_block;
		} else {
			//If not, go to the next process in the suspended blocked queue and try again
			ptr_tmp_sus_block = ptr_tmp_sus_block->next_pcb;
		}
	}
	//If no process contains the name entered in, return NULL
	return NULL;
}

void init_queues(){
	ready_q = (struct queue *) sys_alloc_mem(sizeof(struct queue));
	ready_q -> head = NULL;
	ready_q -> tail = NULL;
	ready_q -> count = 0;

	blocked_q = (struct queue *) sys_alloc_mem(sizeof(struct queue));
	blocked_q -> head = NULL;
	blocked_q -> tail = NULL;
	blocked_q -> count = 0;

	sus_ready_q = (struct queue *) sys_alloc_mem(sizeof(struct queue));
	sus_ready_q -> head = NULL;
	sus_ready_q -> tail = NULL;
	sus_ready_q -> count = 0;

	sus_block_q = (struct queue *) sys_alloc_mem(sizeof(struct queue));
	sus_block_q -> head = NULL;
	sus_block_q -> tail = NULL;
	sus_block_q -> count = 0;

}

int insert_pcb(struct pcb* pcb_insert){
	//clay
	//returns 1 for success and 0 for failed insertion
	if(pcb_insert == NULL){
		serial_println("invalid PCB");
		return 0;
	}

	int pcb_exec_state= pcb_insert->exec_state; //2 - running, 1 - blocked, 0 - ready
	int pcb_dis_state = pcb_insert->dispatch_state; //0 is suspended queue and 1 is non suspended queue

	struct queue* queuePtr = NULL;

	if((pcb_exec_state == 1) && (pcb_dis_state == 1)){
		//set queue pointer to blocked queue
		//blocked_q = queuePtr;
		queuePtr = blocked_q;
	}
	else if((pcb_exec_state) == 0 && (pcb_dis_state == 1)){
		//set queue pointer to ready queue
		//ready_q = queuePtr;
		queuePtr = ready_q;
	}
	if((pcb_exec_state) == 0 && (pcb_dis_state == 0)){
		//set queue pointer to suspended queue
		//sus_ready_q = queuePtr;
		queuePtr = sus_ready_q;
	}
	else if((pcb_exec_state == 1) && (pcb_dis_state == 0)){
		//set queue pointer to suspended block queue
		//sus_block_q = queuePtr;
		queuePtr = sus_block_q;
	}
	if(queuePtr == ready_q || queuePtr == sus_ready_q){
		pcb_insert->next_pcb = NULL;
    	if(is_empty(queuePtr)){
       		queuePtr->head = queuePtr->tail = pcb_insert;
			//pcb_insert = queuePtr->tail;
			queuePtr->tail->next_pcb = NULL;
			queuePtr->head->next_pcb = NULL;
			// serial_println(queuePtr->tail->name);
			// serial_println(queuePtr->head->name);
			
			return 1;
    	}

		else if(pcb_insert->priority < queuePtr->head->priority){
			//at at head
			//serial_println("adding at head");
			//queuePtr->head = pcb_insert;
			
			struct pcb* old_head = queuePtr->head;
			
			queuePtr->head = pcb_insert;
			pcb_insert->next_pcb = old_head;
			return 1;
			
		}
		else if(pcb_insert->priority <= queuePtr->tail->priority){
			// add at tail
			//serial_println("adding at tail");

			struct pcb* curr = queuePtr->head;
			while(curr->next_pcb != NULL){
				curr = curr->next_pcb;
			}
			curr->next_pcb = pcb_insert;
			queuePtr->tail = pcb_insert;
			
			// pcb_insert->next_pcb = queuePtr->tail;
			// queuePtr->head = pcb_insert;
			return 1;
			
			
			
		}
   		else{
			//inserting in middle
			//serial_println("Inserting based upon priority");
			int current_priority = pcb_insert->priority;
			struct pcb* temp_pcb = queuePtr->head;
			//struct pcb* prev;
			while((temp_pcb->next_pcb != NULL) && (temp_pcb->next_pcb->priority <= current_priority) ){
				
				temp_pcb = temp_pcb->next_pcb;
				//prev = temp_pcb;
				
			}
			//prev->next_pcb = pcb_insert;
			pcb_insert->next_pcb = temp_pcb->next_pcb;
			//pcb_insert->next_pcb = temp_pcb->next_pcb;
			temp_pcb->next_pcb = pcb_insert;
			
			return 1;
			

    	}
	}
	else if(queuePtr == blocked_q || queuePtr == sus_block_q){
		pcb_insert->next_pcb = NULL;
		//inserting in tail
    	if(is_empty(queuePtr)){
    	    queuePtr->tail = pcb_insert;
       		queuePtr->head = pcb_insert;
			return 1;

    	}

   		else{
			//inserting in the middle
			//serial_println("in else");
			struct pcb* temp_pcb = queuePtr->head;
			//struct pcb* prev;
			while(temp_pcb->next_pcb != NULL) {
				//prev = temp_pcb;
				temp_pcb = temp_pcb->next_pcb;
			}
			//prev->next_pcb = pcb_insert;
			pcb_insert->next_pcb = temp_pcb;
			//pcb_insert->next_pcb = temp_pcb;
			temp_pcb->next_pcb = pcb_insert;

			return 1;

    	}
	}

	return 0;

}

int block_pcb(char *block_name) {
	//Andrew
	struct pcb* ptr_pcb = find_pcb(block_name);
	if(ptr_pcb == NULL) {
		return 0;
	}
	// if(strcmp(ptr_pcb->name, "idle") || strcmp(ptr_pcb->name, "com_hand")){
	// 	serial_println("Cannot block comhand or idle priority");
	// 	return 0;
	// }
	else {
		remove_pcb(ptr_pcb);
		ptr_pcb->exec_state = 1;
		insert_pcb(ptr_pcb);
		return 1;
	}
}

int unblock_pcb(char *unblock_name) {
	//Andrew
	struct pcb* ptr_pcb = find_pcb(unblock_name);
	if(ptr_pcb == NULL) {
		return 0;
	} else {
		remove_pcb(ptr_pcb);
		ptr_pcb->exec_state = 0;
		insert_pcb(ptr_pcb);
		return 1;
	}
}

int is_empty(struct queue* queuePtr){
	//returns 1 or 0 for is empty and not empty
	return ((queuePtr->head == NULL) && (queuePtr->tail== NULL));
}
//For Show PCB, following the slides, display Name-Class-State-SuspendedState-Priority
void showpcb(struct pcb *pcbtoshow){
	// if (is_empty(ready_q)){
	// 	return;
	// }
	int buf =10;
	//serial_print("Name: ");
	sys_req(WRITE, COM1, "\n\nName:", &buf);
	//serial_println(pcbtoshow->name);
	sys_req(WRITE, COM1, pcbtoshow->name, &buf);

	if(pcbtoshow->class == 0){// 0 indicates system
		//serial_println("Class: System");
		sys_req(WRITE, COM1, "\nClass: System \n", &buf);
	}
	else if(pcbtoshow->class == 1){// 1 indicates user application
		//serial_println("Class: User Application");
		sys_req(WRITE, COM1, "\nClass: User \n", &buf);
	}

	if(pcbtoshow->exec_state == 0){// 0 indicates ready state
		//serial_println("State: Ready");
		sys_req(WRITE, COM1, "State: Ready \n", &buf);
	}
	else if(pcbtoshow->exec_state == 1){// 1 indicates blocked state
		//serial_println("State: Bloacked");
		sys_req(WRITE, COM1, "State: Blocked \n", &buf);
	}
	else if(pcbtoshow->exec_state == 2){// 2 indicates running state
		//serial_println("State: Running");
		sys_req(WRITE, COM1, "State: Running \n", &buf);
	}

	if(pcbtoshow->dispatch_state == 0){
		//serial_println("Suspended State Status: Suspended");
		sys_req(WRITE, COM1, "Suspended State Status: Suspended \n", &buf);
	}
	else if(pcbtoshow->dispatch_state == 1){
		//serial_println("Suspended State Status: Not Suspended");
		sys_req(WRITE, COM1, "Suspended State Status: Not Suspended \n", &buf);
	}
	char prior[0];
	//serial_println("Priority: ");
	sys_req(WRITE, COM1, "Priority: ", &buf);
	//serial_print(itoa(pcbtoshow->priority,prior,10));
	sys_req(WRITE, COM1, itoa(pcbtoshow->priority,prior,10), &buf);
	sys_req(WRITE, COM1, "\n", &buf);
}

void suspend_pcb() {
	int buf = 10;
	sys_req(WRITE,COM1,"Please enter name of PCB to suspend on next line:\n",&buf);

	char name[10];
	memset(name,'\0',10);
	int name_size = (int)sizeof(name);
	sys_req(READ,COM1,name,&name_size);

	struct pcb* ptr_pcb = find_pcb(name);

	if(ptr_pcb == NULL) {
		sys_req(WRITE,COM1,"Error: Invalid PCB name. Please enter command again and use valid name.\n",&buf);
	}

	if(strcmp(ptr_pcb->name, "idle") || strcmp(ptr_pcb->name, "com_hand")){
			serial_println("Cannot Suspend comhand or idle");
			return;
	}
	
	else{
		remove_pcb(ptr_pcb);
		ptr_pcb->dispatch_state = 0;
		// remove_pcb(ptr_pcb);
		insert_pcb(ptr_pcb);
		sys_req(WRITE,COM1,"PCB successfully suspended!\n",&buf);
	}
}

void resume_pcb(char *name) {

		struct pcb* ptr_pcb = find_pcb(name);

		remove_pcb(ptr_pcb);
		ptr_pcb->dispatch_state = 1;
		insert_pcb(ptr_pcb);

}

void show_ready_pcb() {
	//Andrew
    struct pcb* readyQueue = ready_q->head;

    while(readyQueue != NULL){
    	showpcb(readyQueue);
    	readyQueue = readyQueue->next_pcb;
    }
}

void show_blocked_pcb() {
	//Andrew

    struct pcb* blockedQueue = blocked_q->head;
	
    while(blockedQueue != NULL){
    	showpcb(blockedQueue);
    	blockedQueue = blockedQueue->next_pcb;
    }
}

void show_all_pcb() {
	struct pcb* readyQueuePCB = ready_q->head;
	struct pcb* blockedQueuePCB = blocked_q->head;
	struct pcb* susReadyQueuePCB = sus_ready_q->head;
	struct pcb* susBlockQueuePCB = sus_block_q->head;
	int buf = 10;
	sys_req(WRITE,DEFAULT_DEVICE,"Ready Queue: \n",&buf);
	while(readyQueuePCB != NULL) {
		showpcb(readyQueuePCB);
		readyQueuePCB = readyQueuePCB->next_pcb;
	}
	sys_req(WRITE,DEFAULT_DEVICE,"\n",&buf);

	sys_req(WRITE,DEFAULT_DEVICE,"Blocked Queue: \n",&buf);
	while(blockedQueuePCB != NULL) {
		showpcb(blockedQueuePCB);
		blockedQueuePCB = blockedQueuePCB->next_pcb;
	}
	sys_req(WRITE,DEFAULT_DEVICE,"\n",&buf);

	sys_req(WRITE,DEFAULT_DEVICE,"Suspended and Ready Queue: \n",&buf);
	while(susReadyQueuePCB != NULL) {
		showpcb(susReadyQueuePCB);
		susReadyQueuePCB = susReadyQueuePCB->next_pcb;
	}
	sys_req(WRITE,DEFAULT_DEVICE,"\n",&buf);

	sys_req(WRITE,DEFAULT_DEVICE,"Suspended and Blocked Queue: \n",&buf);
	while(susBlockQueuePCB != NULL) {
		showpcb(susBlockQueuePCB);
		susBlockQueuePCB = susBlockQueuePCB->next_pcb;
	}
	sys_req(WRITE,DEFAULT_DEVICE,"\n",&buf);

}

void setpriority(char *name, char *priority){
	
	struct pcb* pcb;
	pcb = find_pcb(name);
	int buf = 10;

	char pri[2];
	pri[0] = priority[0];
	pri[1] = '\0';

	int pri_int = atoi(pri);

	//int prio = atoi(pri);
	//klogv((int) *pri);
	if(pcb != NULL){//Pcb found
		if(pcb->priority != pri_int){
			if(pri_int <= 0 || pri_int <= 9){//checks to see if priority entered is valid *prio
				
				remove_pcb(pcb);
				//pcb->priority = 0;
				pcb->priority = pri_int;
				insert_pcb(pcb);
				sys_req(WRITE, COM1, "Priority has been set. Great success!\n", &buf);

			} else {//entered priority is not 0-9
				sys_req(WRITE, COM1, "The priority entered is not valid.\n", &buf);
			}
		} else {//pcb priority matches the priority entered to set
			sys_req(WRITE, COM1, "The PCB is already set to this priority.\n", &buf);
		}

	}else{//pcb not found, does not exist
		sys_req(WRITE, COM1, "The PCB you have entered does not exist.\n", &buf);
	}
}

int remove_pcb(struct pcb* target){
	
    //temp is used to freeing the memory
    if(target == NULL){
		//serial_println("invalid PCB");
		return 1;
	}
	struct queue* queuePtr = NULL;

	int pcb_exec_state= target->exec_state; //2 - running, 1 - blocked, 0 - ready
	int pcb_dis_state = target->dispatch_state; //0 is suspended queue and 1 is non suspended queue

	if((pcb_exec_state == 1) && (pcb_dis_state == 1)){
		//set queue pointer to blocked queue
		//blocked_q = queuePtr;
		queuePtr = blocked_q;
	}
	else if((pcb_exec_state) == 0 && (pcb_dis_state == 1)){
		//set queue pointer to ready queue
		//ready_q = queuePtr;
		queuePtr = ready_q;
	}
	if((pcb_exec_state) == 0 && (pcb_dis_state == 0)){
		//set queue pointer to suspended queue
		//sus_ready_q = queuePtr;
		queuePtr = sus_ready_q;
	}
	else if((pcb_exec_state == 1) && (pcb_dis_state == 0)){
		//set queue pointer to suspended block queue
		//sus_block_q = queuePtr;
		queuePtr = sus_block_q;
	}

      //key found on the head node.
      //move to head node to the next and free the head.
	//struct pcb* to_remove = queuePtr->head;
	if(queuePtr->head == target && target == queuePtr->tail){
		//serial_println("removing head and tail");
		queuePtr->head = queuePtr->tail = NULL;
		queuePtr->head->next_pcb = queuePtr->tail->next_pcb = NULL;
	}
    if(queuePtr->head == target){ 
		//serial_println("deleting head");
		struct pcb *curr = queuePtr->head;
        queuePtr->head = queuePtr->head->next_pcb;
		curr->next_pcb = NULL;
		return 0;
    }
    else{
		//serial_println("removing from middle");
        struct pcb* current  = queuePtr->head;
		struct pcb* prev = NULL;
        while(current->next_pcb != NULL && !is_empty(queuePtr)){
        	if(current== target){
                //pcb will be disconnected from the queue
				//serial_println("removing");
                prev->next_pcb = current->next_pcb;
 				//queuePtr->head = NULL;
                return 0;
            }
             //Otherwise, move the current node and proceed
			 prev = current;
             current = current->next_pcb;
		}
		
	}
	
 	return 1;
	
}