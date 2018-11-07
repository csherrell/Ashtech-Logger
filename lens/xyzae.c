#include <stdio.h>
#include "../gps.h"
#include "xyzae.h"

/*X, Y, Z, Azimuth, Elevation */

void lens_print_xyzae(struct str_satellite_position *xyzae,
          unsigned int rcvtime,
          int aode,
          FILE *csv)
{
     if(csv)
     {
          fprintf(csv,"%d, %d, %d, %f, %f, %f, %f, %f, %f, %0.1f, %0.24f\n",
                    rcvtime,
                    xyzae->svprn + 1,
                    aode,
                    xyzae->x,
                    xyzae->y,
                    xyzae->z,
                    xyzae->range,
                    xyzae->azimuth,
                    xyzae->elevation,
                    xyzae->sad,
                    xyzae->delta_tr);
          fflush(csv);
     }
}
