#include "../r1.h"
#include "../mpx_supt.h"
#include "string.h"

int help(){
	char *line1 = "***************\n";
	char *line2 = "Permanent Commands \n\n";
	char *line3 = "1.  help - Provides this menu with a list of commands. \n\n";
	char *line4 = "2.  time - Prints the current time, relative to UTC. \n\n";
	char *line5 = "3.  date - Prints the current date, set in UTC. \n\n";
	char *line6 = "4.  settime - Allows the user to set the time. \n\n";
	char *line7 = "5.  setdate - Allows the user to set the date. \n\n";
	char *line8 = "6.  version - Prints the current version of codestorm OS. \n\n";
	char *line9 = "7.  quit - Exits the OS. \n\n";
	char *line10 = "8.  suspendpcb - Allows user to put a process in the suspended state \n";
	char *line11 = "                  by inputting the name of the process. \n\n";
	char *line12 = "9.  resumepcb - Allows user to put a process in the not suspended state \n";
	char *line13 = "                 by inputting the name of the process. \n\n";
	char *line14 = "10. setpcbpriority - Allows user to change a process's priority by \n";
	char *line15 = "                      inputting the name of the process and the new \n";
	char *line16 = "                      priority number 0(highest) - 9(lowest). \n\n";
	char *line17 = "11. showpcb - Allows the user to input a process name and displays \n";
	char *line18 = "               the name, class, state, suspended status, and \n";
	char *line19 = "               priority of the process. \n\n";
	char *line20 = "12. showready - For all processes in the ready state, displays the \n";
	char *line21 = "                 name, class, state, suspended status, and priority of each. \n\n";
	char *line22 = "13. showblocked - For all processes in the blocked state, displays the \n";
	char *line23 = "                   name, class, state, suspended status, and priority of each. \n\n";
	char *line24 = "14. show allocated - Shows the allocated MCBs in the heap \n\n";
	char *line25 = "15. show free - Iterates throught heap and shows the user all free MCBs \n\n";
	char *line26 = "16. free memory - Prompts the user in the next line to enter the corresponding address of the MCB, \n";
	char *line27 = "               to free, returns a message whether the free was successful or unsuccessful. \n\n";
	char *line28 = "17. allocate memory - Prompts the user for a number of bytes to allocate as an MCB. \n\n";
	// char *line32 = "18. showall - For all processes in any state, displays the name, class, \n";
	// char *line33 = "               state, suspended status, and priority of each. \n\n\n";
	// char *line34 = "Temporary Commands \n\n";
	// char *line35 = "1.  yield - Yields the Current Operating Process to the next ready process, \n";
	// char *line36 = "                 begins the dispatching of processes loaded into the ready queue\n";
	// char *line37 = "2.  loadr3 - Loads the set of processes designated for the ready queue \n";
	int count = 20;

	sys_req(WRITE,COM1,line1,&count);
	sys_req(WRITE,COM1,line2,&count);
	sys_req(WRITE,COM1,line3,&count);
	sys_req(WRITE,COM1,line4,&count);
	sys_req(WRITE,COM1,line5,&count);
	sys_req(WRITE,COM1,line6,&count);
	sys_req(WRITE,COM1,line7,&count);
	sys_req(WRITE,COM1,line8,&count);
	sys_req(WRITE,COM1,line9,&count);
	sys_req(WRITE,COM1,line10,&count);
	sys_req(WRITE,COM1,line11,&count);
	sys_req(WRITE,COM1,line12,&count);
	sys_req(WRITE,COM1,line13,&count);
	sys_req(WRITE,COM1,line14,&count);
	sys_req(WRITE,COM1,line15,&count);
	sys_req(WRITE,COM1,line16,&count);
	sys_req(WRITE,COM1,line17,&count);
	sys_req(WRITE,COM1,line18,&count);
	sys_req(WRITE,COM1,line19,&count);
	sys_req(WRITE,COM1,line20,&count);
	sys_req(WRITE,COM1,line21,&count);
	sys_req(WRITE,COM1,line22,&count);
	sys_req(WRITE,COM1,line23,&count);
	sys_req(WRITE,COM1,line24,&count);
	sys_req(WRITE,COM1,line25,&count);
	sys_req(WRITE,COM1,line26,&count);
	sys_req(WRITE,COM1,line27,&count);
	sys_req(WRITE,COM1,line28,&count);
	


	// sys_req(WRITE,COM1,line1,&count);
	
	// sys_req(WRITE,COM1,line26,&count);
	// sys_req(WRITE,COM1,line27,&count);
	// sys_req(WRITE,COM1,line28,&count);
	// sys_req(WRITE,COM1,line29,&count);

	// sys_req(WRITE,COM1,line1,&count);




	return 1;
}
