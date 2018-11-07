#ifndef AZIMUTH_ELEVATION_H
#define AZIMUTH_ELEVATION_H 1

#include "gps.h"
#include "ashtech/ashtech.h"

/**
  aep - Almanac / Ephemeris / Position
  struct str_satellite_almanac_ephemeris_position *satellite_position_information,
*/

double
get_elevation(
          struct str_satellite_position *satellite_xyzae_position,
          struct str_location *location);

void
azimuth_elevation(
          struct str_satellite_position *satellite_xyzae_position,
          double latitude, double longitude,
          double Xr, double Yr, double Zr);

#endif

