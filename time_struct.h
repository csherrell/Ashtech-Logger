#ifndef TIME_STRUCT_H
#define TIME_STRUCT_H 1

#include <time.h>
#include <sys/time.h>

struct str_time_gps
{
/* GPS Time stamp */
     int week;
     unsigned int rcvtime;
};

struct str_time_unix
{
     time_t      sec;     /* seconds */
     suseconds_t usec;    /* microseconds */
};

struct str_time
{
     struct str_time_gps  tm_gps;
     struct str_time_unix tm_unix;
};

#endif

