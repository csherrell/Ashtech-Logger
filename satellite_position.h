#ifndef SATELLITE_POSITION_H
#define SATELLITE_POSITION_H 1

/*
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
*/
#include "laas.h"
#include "ashtech/ashtech.h"
#include "gps.h"

double
get_satellite_distance(struct str_satellite_position *position1, struct str_satellite_position *position2);

double
get_satellite_range(struct str_satellite_position *position1, struct str_location *position2);

void
satellite_position(
          void *ashtech_varable,
          enum  ashtech_type  t,
          struct str_location *location,
          double PRr,
          struct str_time *master_time,
          struct str_satellite_position *xyzae);

/*
void
satellite_position(
          struct str_satellite_almanac_ephemeris_position *satellite_position_information,
          double Xu,
          double Yu,
          double Zu,
          double PRr);
*/

#endif

