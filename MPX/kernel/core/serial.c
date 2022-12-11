/*
  ----- serial.c -----
  Description..: Contains methods and variables used for
    serial input and output.
*/

#include <stdint.h>
#include <string.h>
#include <modules/mpx_supt.h>
#include <core/io.h>
#include <core/serial.h>

#define NO_ERROR 0

// Active devices used for serial I/O
int serial_port_out = 0;
int serial_port_in = 0;

/*
  Procedure..: init_serial
  Description..: Initializes devices for user interaction, logging, ...
*/
int init_serial(int device)
{
  outb(device + 1, 0x00); //disable interrupts
  outb(device + 3, 0x80); //set line control register
  outb(device + 0, 115200/9600); //set bsd least sig bit
  outb(device + 1, 0x00); //brd most significant bit
  outb(device + 3, 0x03); //lock divisor; 8bits, no parity, one stop
  outb(device + 2, 0xC7); //enable fifo, clear, 14byte threshold
  outb(device + 4, 0x0B); //enable interrupts, rts/dsr set
  (void)inb(device);      //read bit to reset port
  return NO_ERROR;
}

/*
  Procedure..: serial_println
  Description..: Writes a message to the active serial output device.
    Appends a newline character.
*/
int serial_println(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  outb(serial_port_out,'\r');
  outb(serial_port_out,'\n');  
  return NO_ERROR;
}

/*
  Procedure..: serial_print
  Description..: Writes a message to the active serial output device.
*/
int serial_print(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  if (*msg == '\r') outb(serial_port_out,'\n');
  return NO_ERROR;
}

/*int (*student_read)(char *buffer, int *count);
  Procedure..: set_serial_out
  Description..: Sets serial_port_out to the given device address.
    All serial output, such as that from serial_println, will be
    directed to this device.
*/
int set_serial_out(int device)
{
  serial_port_out = device;
  return NO_ERROR;
}

/*
  Procedure..: set_serial_in
  Description..: Sets serial_port_in to the given device address.
    All serial input, such as console input via a virtual machine,
    QEMU/Bochs/etc, will be directed to this device.
*/
int set_serial_in(int device)
{
  serial_port_in = device;
  return NO_ERROR;
}

int *polling(char *buffer, int *count){
// insert your code to gather keyboard input via the technique of polling.
// You must validat each key and handle special keys such as delete, back space, and
// arrow keys

//remove the following line after implementing your module, this is present
//just to allow the program to compile before R1 is complete
//strlen(buffer);
//int buffer_index = 0;
int quit = 0;
int buffer_index = 0;
int cursour_index = 0;
//const char *down = "1583045442";
//int d = "^[[B";
sys_req(WRITE,COM1,"$",count);
  while(!quit){

    if(inb(COM1 + 5) & 1){
      
      char c = inb(COM1);

    if (c == 8){
      sys_req(WRITE,COM1,"\b ",count);
    }
    if (c == 127){
      
      sys_req(WRITE,COM1," \b",count);
    }
      //char UP_arrow[5] = c; 
      //left arrow key handler
    if(c == 27){
      c = inb(COM1);
      if(c == 91){
        c = inb(COM1);
        if(c == 'A'){

        }
        else if(c == 'B'){

        }
        else if(c == 'C'){
          if((cursour_index <= buffer_index-1)&&(buffer_index != 0)) {
            serial_print("\e[C");
            cursour_index++;
          }

        }
        else if(c == 'D'){
          if(cursour_index > 0) {
            sys_req(WRITE,COM1,"\e[D",count);
            cursour_index--;
          }
        }
      }
    } else {
      //takes in char c from serial port
      //assigns char a pointer at the index for the pointer
     
        buffer[buffer_index] = c;
        serial_print(buffer+buffer_index);

        if(buffer_index == 99){
          quit = TRUE;
        }

        buffer_index++;
        cursour_index++;
     
      }

    if(c == 13){
        quit = TRUE;
      }

    }
  }

return count;

}