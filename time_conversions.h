#ifndef TIME_CONVERSIONS_H
#define TIME_CONVERSIONS_H 1

#include <time.h>
#include <sys/time.h>
#include "laas.h"

#include "time_struct.h"

#define MILLISECONDS_IN_DAY  86400000
#define MILLISECONDS_IN_HOUR  3600000

#define GPS_LEAP_SECONDS          14
#define UTIME_AT_GPS_EPOCH 315964800
#define SECONDS_IN_WEEK       604800 /* (7 * 24 * 60 * 60)  */

char * current_time_as_string(char current_time[]);
void date_init();
double f_date_gps_to_unix(unsigned int gps_week, unsigned int gps_rcvtime);
time_t date_gps_to_unix(unsigned int gps_week, unsigned int gps_rcvtime);
time_t date_unix_to_gps(struct str_time_gps *gt);
unsigned int unix_to_gps_rcvtime();
unsigned int unix_time_as_gps_time();

void time_gps_to_unix(struct str_time *time);
void time_unix_to_gps(struct str_time *time);
void gps_add_time(struct str_time *t, unsigned int value);
void gps_sub_time(struct str_time *t, unsigned int value);


#endif

