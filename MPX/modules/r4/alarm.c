void alarm()
{
  
  int i;

  // repeat forever if termination fails
  while(1){
 

    for(i=0; i<5; i++){
      //serial_println("proc1 dispatched");
	 
     
      sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
      
    }
    
    sys_req(EXIT, DEFAULT_DEVICE, NULL, NULL);
   
  }
}