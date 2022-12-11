#include "../r1.h"
#include "../mpx_supt.h" 
#include "string.h"
#include "date.h"
#include <core/io.h>

int print_time(){

	outb(RTC_INDEX_REGISTER,MINUTES);
	unsigned char minutes = inb(RTC_DATA_REGISTER);

	outb(RTC_INDEX_REGISTER,HOURS);
	unsigned char hours = inb(RTC_DATA_REGISTER);

	char minutes_s[3];
	minutes_s[0] = '0';
	char hours_s[3];

	int int_minutes = bcd_to_int(minutes);
	int int_hours = bcd_to_int(hours);

	char *real_minutes = itoa(int_minutes,minutes_s,10);
	char *real_hour = itoa(int_hours,hours_s,10);
  	int size = 30;
	sys_req(WRITE,COM1,real_hour, &size);
	sys_req(WRITE,COM1,":", &size);
	sys_req(WRITE,COM1,real_minutes, &size);
	return 1;

}

int print_date() {
	//function for command getdate

	outb(RTC_INDEX_REGISTER,MONTH);
	unsigned char month = inb(RTC_DATA_REGISTER);

	outb(RTC_INDEX_REGISTER,DAY_OF_MONTH);
	unsigned char day = inb(RTC_DATA_REGISTER);

	outb(RTC_INDEX_REGISTER,YEAR);
	unsigned char year = inb(RTC_DATA_REGISTER);
	

	//buffers for itoa
	char year_s[3];
	//char *month_s[1];
	char day_s[3];



	//converting to printable string
	char *real_year = itoa(bcd_to_int(year),year_s,10);
	//char *real_month = itoa(bcd_to_int(month),*month_s,10);
	char *real_day = itoa(bcd_to_int(day),day_s,10);

	//string for month 
  	char *letter_month;
  	//case switch to output name of month rather than num
  	switch (bcd_to_int(month)){
  	 case 1:
  	     letter_month = "Jan";
  	     break;
  	 case 2:
  	     letter_month = "Feb";
  	     break;
  	 case 3:
  	     letter_month = "Mar";
  	     break;
  	 case 4:
  	    letter_month = "Apr";
  	     break;
  	 case 5:
  	     letter_month = "May";
  	     break;
  	 case 6:
  	     letter_month = "Jun";
  	     break;
  	 case 7:
  	     letter_month = "Jul";
  	     break;
  	 case 8:
  	     letter_month = "Aug";
  	     break;
  	 case 9:
  	     letter_month = "Sept";
  	     break;
  	 case 10:
  	     letter_month = "Oct";
  	     break;
  	 case 11:
  	     letter_month = "Nov";
  	     break;
  	 case 12:
  	     letter_month = "Dec";
  	     break;
  	 }
	//outputting values to screen
    int size = 30;

    sys_req(WRITE,COM1,"20", &size); //placeholder
    sys_req(WRITE,COM1,real_year, &size);
	sys_req(WRITE,COM1," ", &size);
	sys_req(WRITE,COM1,letter_month, &size);
	sys_req(WRITE,COM1," ", &size);
	sys_req(WRITE,COM1,real_day, &size);
	sys_req(WRITE,COM1," ", &size);
	

 	return 1;
}

int bcd_to_int(unsigned char bcd){
	//function to take the unsigned char to int
	int lower = (int)(bcd & 0x0F);
	int upper = (int)((bcd >> 4) & 0x0F) * 10;

	//adds both nibbles together
	int decimal = upper + lower;

	return decimal;
}

unsigned char int_to_bcd(int dec){
	int upper_nibble = dec / 10;
	int lower_nibble = dec % 10;
	//char buffer[10];
	// itoa(upper_nibble, buffer, 10);
	// klogv(buffer);
	// itoa(lower_nibble, buffer, 10);
	// klogv(buffer);

	unsigned char bcd = 0;
	bcd = bcd | upper_nibble;
	bcd = bcd<< 4;
	bcd = bcd | lower_nibble;

	return bcd;
}

void swap(char *x, char *y) {
	//function that swaps char with another
    char temp = *x; 
    *x = *y; 
    *y = temp;
}
 
char* reverse(char *buffer, int i, int j){
	// Function to reverse `buffer[i…j]`
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
    return buffer;
}
 
char* itoa(int value, char* buffer, int base){
	// Iterative function to implement `itoa()` function in C
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }
 
    // consider the absolute value of the number
    int n = value;
 	
    int i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;

    }
 	
    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }
 
    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '0';
    }
    
 	if(value < 10){
    	//buffer++;
 		//added handling to enable the 0 to be added for minutes/hours 00 - 09
    	char temp = buffer[0];
    	buffer[0] = '0';
    	buffer[1] = temp;
    	buffer[2] = '\0';
    }
    else{
        buffer[i] = '\0'; // null terminate string
    }
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

int set_date(char date[30]){
	//function to set date after recieving command and new date
	//stop interrupt5
	cli();

	//parsing time into seperate values for hours minutes etc
	char *string_time = date;
    //sys_req(WRITE,COM1,&string_time[1], &time_size);
    // sys_req(WRITE,COM1,&month, &time_size);
    // sys_req(WRITE,COM1,&day, &time_size);
	//checking values that are inputted for reasonable inputs
	char month[3];
	month[0] = string_time[0];
	month[1] = string_time[1];
	month[2] = '\0';

	char day[3];
	day[0] = string_time[3];
	day[1] = string_time[4];
	day[2] = '\0';


	char year[3];
	year[0] = string_time[6];
	year[1] = string_time[7];
	year[2] = '\0';


	//converting string to int
	int months = atoi(month);
	int days = atoi(day);
	int years = atoi(year);

	int leap_year = 0;

	// checking leap year
	// int leap = 0;
	if(days > 31 || months > 12){
		return 0;
	}

	if ((years % 4 == 0) && (years % 400 == 0 || years % 100 != 0)){
	//leap years are divisible by 400 but not 100
		//then it is a leap year
		leap_year = 1;
	}
 
	if (months == 02){  //feb has 28 days unless leap = 1 then it has 29
		if (leap_year == 1){
			if (days > 29){
				//invalid date
				return 0;
			}
		}
		else{
			if (days > 28){
				//invalid date
				return 0;
			}
		}
	}

	//converitng int to bcd
	unsigned char unsigned_month = int_to_bcd(months);
	unsigned char unsigned_day = int_to_bcd(days);
	unsigned char unsigned_year = int_to_bcd(years);
	//putting bcd nums into registers
	//write index to RTC index reg with outb();
	//write new value for that index with outb();
	outb(RTC_INDEX_REGISTER,MONTH);
 	outb(RTC_DATA_REGISTER,unsigned_month);

	outb(RTC_INDEX_REGISTER,DAY_OF_MONTH);
	outb(RTC_DATA_REGISTER,unsigned_day);

	outb(RTC_INDEX_REGISTER,YEAR);
	outb(RTC_DATA_REGISTER,unsigned_year);
	//start interrupts
	sti();
    return 1;
}

int set_time(char time[30]){
	
	//parsing time into seperate values for hours minutes etc
	char *string_time = time;
	char hour[3];
	hour[0] = string_time[0];
	hour[1] = string_time[1];
	hour[2] = '\0';

	char minute[3];
	minute[0] = string_time[3];
	minute[1] = string_time[4];
	minute[2] = '\0';

	int hours = atoi(hour);
	int minutes = atoi(minute);

 	if(minutes > 60 || hours > 24){
		return 0;
	}
	unsigned char unsigned_hour = int_to_bcd(hours);
	unsigned char unsigned_minute = int_to_bcd(minutes);

	cli();
	outb(RTC_INDEX_REGISTER,MINUTES);
	outb(RTC_DATA_REGISTER,unsigned_minute);

	outb(RTC_INDEX_REGISTER,HOURS);
	outb(RTC_DATA_REGISTER,unsigned_hour);
	sti();
	return 1;
}