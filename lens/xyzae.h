#ifndef XYZAE_H
#define XYZAE_H 1

#include "../gps.h"
#include "xyzae.h"

/*X, Y, Z, Azimuth, Elevation */

void lens_print_xyzae(struct str_satellite_position *satellite_xyzae_position,
          unsigned int rcvtime,
          int aode,
          FILE *csv);

#endif
