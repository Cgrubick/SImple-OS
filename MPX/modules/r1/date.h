#ifndef DATE_H
#define DATE_H

#define RTC_INDEX_REGISTER 0x70
#define RTC_DATA_REGISTER 0x71
#define COM1 0x3f8
#define YEAR 0x09
#define MONTH 0x08
#define DAY_OF_MONTH 0x07
#define HOURS 0x04
#define MINUTES 0x02

int print_date();
int print_time();
int bcd_to_int(unsigned char bcd);
char* reverse(char *buffer, int i, int j);
char* itoa(int value, char* buffer, int base);
int set_date(char date[30]);
int set_time(char time[30]);
unsigned char int_to_bcd(int dec);

#endif