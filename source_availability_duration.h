#ifndef SOURCE_AVAILABILITY_DURATION_H
#define SOURCE_AVAILABILITY_DURATION_H 1

#include "laas.h"
#include "gps.h"
#include "ashtech/ashtech.h"

void
source_availability_duration(
          struct str_satellite_almanac_ephemeris_position *satellites_position_information,
          struct str_location *location,
          struct str_time *master_time);

#endif

