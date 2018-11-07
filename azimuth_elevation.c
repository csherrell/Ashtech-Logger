#include <stdio.h>
#include <math.h>

#include "laas.h"
#include "gps.h"
#include "azimuth_elevation.h"
#include "mathematical_constants.h"

double get_elevation(
          struct str_satellite_position *xyzae,
          struct str_location *location)
{
     double delta_x = (xyzae->x - location->x);
     double delta_y = (xyzae->y - location->y);
     double delta_z = (xyzae->z - location->z);
     double distance = pow((pow(delta_x,2) + pow(delta_y,2) + pow(delta_z,2)), .5);

     double latitude  = location->latitude  * (PI / 180);
     double longitude = location->longitude * (PI / 180);

     return ((double)180 / PI) * asin(((delta_x*cos(latitude)*cos(longitude)) + (delta_y * cos(latitude)*sin(longitude)) + (delta_z*sin(latitude)))/ distance);
}

void azimuth_elevation(
          struct str_satellite_position *xyzae,
          double latitude, double longitude,
          double Xr, double Yr, double Zr)
{
/*
     struct str_satellite_position *xyzae = &satellite_position_information->xyzae;
*/

     double delta_x = (xyzae->x - Xr);
     double delta_y = (xyzae->y - Yr);
     double delta_z = (xyzae->z - Zr);
     double distance = pow((pow(delta_x,2) + pow(delta_y,2) + pow(delta_z,2)), .5);

     double pte = 0;
     double ptn = 0;
     double ptu = 0;

     latitude = latitude * ((double)PI / 180);
     longitude = longitude * ((double)PI / 180);

     ptu = ((delta_x*cos(latitude)*cos(longitude)) + (delta_y * cos(latitude)*sin(longitude)) + (delta_z*sin(latitude)))/ distance;
     pte = ((delta_x * (-1 * sin(longitude))) + (delta_y * cos(longitude))) / distance;
     ptn = ((delta_x*-1*sin(latitude)*cos(longitude)) + (delta_y * -1*sin(latitude)*sin(longitude)) + (delta_z*cos(latitude)))/ distance;

     xyzae->elevation = ( (double) 180 / PI) * asin(ptu);

     if(pte >= 0)
          xyzae->azimuth = ((double) 180 / PI) * acos(ptn / sin(acos(ptu)));
     else
          xyzae->azimuth = 360 - (((double) 180 / PI) * acos(ptn / sin(acos(ptu))));
}

