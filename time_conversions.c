#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "time_conversions.h"

/* Returns the pointer to current_time  */
/* Useful for using in print statements */
char * current_time_as_string(char current_time[])
{
     time_t tt_current_time = 0;
     tt_current_time = time(NULL);
     strcpy(current_time,ctime(&tt_current_time));
     current_time[strlen(current_time)-1]=0;                             /* remove \n */
     return current_time;
}

void date_init()
{
/* Old Way */
/*
     char name[] = "TZ";
     char value[] = "UTC";
     setenv(name, value, 1);
*/
     /* These two line go together */
     putenv("TZ=UTC");
     tzset();
}

double f_date_gps_to_unix(unsigned int gps_week, unsigned int gps_rcvtime)
{
     /* Strctly speaking to get convert GPS to UTC time the GPS_LEAP_SECONDS need to be subtracted
        but since we are trying to extract the date, then the leap seconds are left out since they
        resolve to the previous day */
     return ((gps_week * SECONDS_IN_WEEK)
          + UTIME_AT_GPS_EPOCH 
          /*          - GPS_LEAP_SECONDS */                
          + ((double)gps_rcvtime/1000));
     /* Do not remove the (double) gps_rcvtime cast.
        It will result in the division being implicitly cast to an int */
             
}

time_t date_gps_to_unix(unsigned int gps_week, unsigned int gps_rcvtime)
{
     /* Strctly speaking to get convert GPS to UTC time the GPS_LEAP_SECONDS need to be subtracted
        but since we are trying to extract the date, then the leap seconds are left out since they
        resolve to the previous day */
     return (gps_week * SECONDS_IN_WEEK)
          + UTIME_AT_GPS_EPOCH 
          /*          - GPS_LEAP_SECONDS */                
          + (int) floor(gps_rcvtime/1000);
}

time_t date_unix_to_gps(struct str_time_gps *gps_time)
{
     struct str_time_unix current_time;
     struct tm *tm_time;

     gettimeofday((struct timeval *) &current_time,NULL);
     current_time.sec += GPS_LEAP_SECONDS;
     current_time.sec -= UTIME_AT_GPS_EPOCH;


     tm_time = localtime(&current_time.sec);
     /* printf("%d: %d/%d/%d -  %d:%d:%d\n", tm_time->tm_wday, tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec); */

     gps_time->week    = (unsigned int) (current_time.sec / SECONDS_IN_WEEK);
     gps_time->rcvtime = (unsigned int) ((current_time.sec % SECONDS_IN_WEEK) * 1000) + (current_time.usec / 1000);
     return (time_t) gps_time->rcvtime;
}

unsigned int unix_to_gps_rcvtime()
{
     struct str_time_unix current_time;

     gettimeofday((struct timeval *) &current_time,NULL);
     current_time.sec += GPS_LEAP_SECONDS;
     current_time.sec -= UTIME_AT_GPS_EPOCH;
     return (((current_time.sec % SECONDS_IN_WEEK) * 1000) + (current_time.usec / 1000));
}

void time_gps_to_unix(struct str_time *time)
{
     /* Strctly speaking to convert GPS to UTC time the GPS_LEAP_SECONDS need to be subtracted
        but since we are trying to extract the date, then the leap seconds are left out since they
        resolve to the previous day */
     time->tm_unix.sec = (time->tm_gps.week * SECONDS_IN_WEEK)
                         + UTIME_AT_GPS_EPOCH 
                         /* - GPS_LEAP_SECONDS */                
                         + (int) floor(time->tm_gps.rcvtime/1000);
     if((time->tm_gps.rcvtime - (floor(time->tm_gps.rcvtime/1000)*1000)) == 0)
     {
          time->tm_unix.usec = 0;
          /* printf("Time: 0\n"); */
     } else {
          time->tm_unix.usec = 500000;
          /* printf("Time: 500000\n"); */
     }
/*
     printf("time_gps_to_unix(): sec:  %d, usec:    %d\n", (int) time->tm_unix.sec, (int) time->tm_unix.usec);
     printf("time_gps_to_unix(): week: %d, rcvtime: %d\n", time->tm_gps.week, time->tm_gps.rcvtime);
*/
}

/** The GPS time part gets updated with the equivalent of the UNIX time part */
void time_unix_to_gps(struct str_time *time)
{
     struct str_time_unix temp_time;

     memcpy(&temp_time, &time->tm_unix, sizeof(struct str_time_unix));
     
     /* temp_time.sec += GPS_LEAP_SECONDS; */
     temp_time.sec -= UTIME_AT_GPS_EPOCH;

     time->tm_gps.week    = (unsigned int) (temp_time.sec / SECONDS_IN_WEEK);
     time->tm_gps.rcvtime = (unsigned int) ((temp_time.sec % SECONDS_IN_WEEK) * 1000) + (temp_time.usec / 1000);

     printf("time_unix_to_gps(): sec:  %d, usec:    %d\n", (int) time->tm_unix.sec, (int) time->tm_unix.usec);
     printf("time_unix_to_gps(): week: %d, rcvtime: %d\n", time->tm_gps.week, time->tm_gps.rcvtime);
}

void gps_add_time(struct str_time *t, unsigned int value)
{
     unsigned int t_hold = t->tm_gps.rcvtime;

     t->tm_gps.rcvtime = (t->tm_gps.rcvtime + value) % 604800500;

     if(t_hold > t->tm_gps.rcvtime)
     {
          t->tm_gps.rcvtime += 500;
          t->tm_gps.week++;
     }
     time_gps_to_unix(t);
}

void gps_sub_time(struct str_time *t, unsigned int value)
{
     if(t->tm_gps.rcvtime <= value)
     {
          t->tm_gps.rcvtime = 604800000 - (value - t->tm_gps.rcvtime);
          t->tm_gps.week--;
     } else {
          t->tm_gps.rcvtime = t->tm_gps.rcvtime - value;
     }
     time_gps_to_unix(t);
}
