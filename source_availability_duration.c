#define _ISOC9X_SOURCE  1 /* Used for lrint */
#define _ISOC99_SOURCE  1 /* Used for lrint */
#include <math.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "gps.h"
#include "mathematical_constants.h"
#include "satellite_position.h"
#include "azimuth_elevation.h"
#include "source_availability_duration.h"

#include <stdlib.h> /* only needed for the exit() function */

void
source_availability_duration(
          struct str_satellite_almanac_ephemeris_position *satellite_position_information,
          struct str_location *location,
          struct str_time *master_time)
{
     struct str_ashtech_snv *snv = &satellite_position_information->snv;

     double elevation_mask = 5;

     /** Set the error tolerance bound */
     /* double nom  = 0.0001; */
     double nom  = 0.00001;
     bool   done = false;
     int break_counter = 0;

     /** Set the lower_time to the current time */
     double time_lower = (master_time->tm_gps.rcvtime / 1000.0);
     /** Set the guess to 4 hours after the lower guess */
     double time_upper = (time_lower + 3600);

     double time_mid = 0;
     double time_old = 0;

     double elevation_lower = satellite_position_information->xyzae.elevation;
     double elevation_mid   = 0;
     double elevation_upper = 0;

     double sign_test = 0;

     struct str_satellite_position sat_pos_lower;
     struct str_satellite_position sat_pos_mid;
     struct str_satellite_position sat_pos_upper;

     int i = 0;
     int i_lower = 0;
     int i_upper = 0;

     struct str_time t;

     int h = 0;
     int m = 0;
     int s = 0;

     memset(&sat_pos_lower, 0x0, sizeof(struct str_satellite_position));
     memset(&sat_pos_mid,   0x0, sizeof(struct str_satellite_position));
     memset(&sat_pos_upper, 0x0, sizeof(struct str_satellite_position));
     memset(&t, 0x0, sizeof(struct str_time));

/*
     struct str_location l;
     struct str_location *location;
     location = &l;
     location->x =  -677736.175;
     location->y = -5171015.741;
     location->z =  3660053.531;
     location->latitude  =  35.24231911509;
     location->longitude = -97.4668771304;
     location->altitude  = 342.3904;
*/


/*
     printf("1. Sat: %02d Time Lower: %f \n", sat_pos_lower.xyzae.svprn, time_lower);
     printf("1. Sat: %02d Time Mid:   %f \n", sat_pos_lower.xyzae.svprn, time_mid);
     printf("1. Sat: %02d Time Upper: %f \n", sat_pos_lower.xyzae.svprn, time_upper);
*/

     /* Set the upper elevation */
     
     t.tm_gps.rcvtime = lrint(time_upper * 1000);
     satellite_position(snv, SNV, location, PRr_NOMINAL, &t,  &sat_pos_upper);
     elevation_upper = get_elevation(&sat_pos_upper, location);
     elevation_upper -= elevation_mask;

/*
     printf("SNV: PRN: %d, AODE:%d, Checksum:%d\n", snv->prnnum, snv->aode, snv->checksum);
     printf("Start - i: %d, Elevation: %f\n", i_upper, elevation_upper);
*/

     while(elevation_upper > 0)
     {
          /* Set the upper elevation **********************************************************/
          time_upper += 3600;
          t.tm_gps.rcvtime = lrint(time_upper * 1000);
          satellite_position(snv, SNV, location, PRr_NOMINAL, &t,  &sat_pos_upper);
          elevation_upper = get_elevation(&sat_pos_upper, location);
          elevation_upper -= elevation_mask;
          /************************************************************************************/
          ++i;
/*
          printf("Loop  - i: %d, Elevation: %f\n", i_upper, elevation_upper);
*/
     }

/*
     printf("End  - i: %d, Elevation: %f\n", i_upper, elevation_upper);
     printf("Elevation Upper: %f\n", elevation_upper);
     printf("Elevation lower: %f\n", elevation_lower);
*/

     time_mid = (time_lower + ((time_upper - time_lower) / 2.0));
     i = 0;
     i_upper = 0;
     i_lower = 0;

/*
     printf("2. Sat: %02d Time Lower: %f \n", sat_pos_lower.svprn, time_lower);
     printf("2. Sat: %02d Time Mid:   %f \n", sat_pos_lower.svprn, time_mid);
     printf("2. Sat: %02d Time Upper: %f \n", sat_pos_lower.svprn, time_upper);
*/

     while(!done)
     {
          /*printf("L1. Sat: %02d Time Mid:   %f, El: %f\n", sat_pos_lower.svprn, time_mid, elevation_mid); */
          break_counter++;
          if(break_counter > 50) exit(0);
          
          /* printf("In While Loop\n"); */
          time_old = time_mid;
          time_mid = time_upper - (elevation_upper * (time_lower - time_upper)) / (elevation_lower - elevation_upper);
          /* Set the midpoint elevation *******************************************************/
          t.tm_gps.rcvtime = lrint(time_mid * 1000);
          satellite_position(snv, SNV, location, PRr_NOMINAL, &t,  &sat_pos_mid);
          elevation_mid = get_elevation(&sat_pos_mid, location);
          elevation_mid -= elevation_mask;
          /************************************************************************************/

/*
          printf("L2. Sat: %02d Time Mid:   %f, El: %f\n", sat_pos_lower.svprn, time_mid, elevation_mid);
*/

          ++i;
          
          if(((double)(fabs(time_mid - time_old) / time_mid)) < nom)
          {
/*
               printf("NOM: %f\n", ((double)(fabs(time_mid - time_old) / time_mid))); 
*/
               done = true;
          }
     
          sign_test = elevation_lower * elevation_mid;
          
          if(sign_test < 0)
          {
               time_upper = time_mid; 
               /* Set the upper elevation **********************************************************/
               t.tm_gps.rcvtime = lrint(time_upper * 1000);
               satellite_position(snv, SNV, location, PRr_NOMINAL, &t,  &sat_pos_upper);
               elevation_upper = get_elevation(&sat_pos_upper, location);
               elevation_upper -= elevation_mask;
/*
               printf("L2. Sat: %02d Time Upper:   %f, El: %f\n", sat_pos_lower.svprn, time_mid, elevation_upper);
*/
               /************************************************************************************/
               i_upper = 0;
               i_lower++;
               if(i_lower >= 2)
               {
                    elevation_lower /= (double) 2.0;
               }
          } else {
               time_lower = time_mid;
               /* Set the lower elevation **********************************************************/
               t.tm_gps.rcvtime = lrint(time_lower * 1000);
               satellite_position(snv, SNV, location, PRr_NOMINAL, &t,  &sat_pos_lower);
               elevation_lower = get_elevation(&sat_pos_lower, location);
               elevation_lower -= elevation_mask;
/*
               printf("L2. Sat: %02d Time Lower:   %f, El: %f\n", sat_pos_lower.svprn, time_mid, elevation_lower);
*/
               /************************************************************************************/
               i_lower = 0;
               i_upper++;
               if(i_upper >= 2)
               {
                    elevation_upper /= (double) 2.0; 
               }
          }
/*
          printf("2. Sat: %02d Time Lower: %f \n", sat_pos_lower.svprn, time_lower);
          printf("2. Sat: %02d Time Mid:   %f \n", sat_pos_lower.svprn, time_mid);
          printf("2. Sat: %02d Time Upper: %f \n", sat_pos_lower.svprn, time_upper);
*/
     }

/*
     printf("3. Sat: %02d Time Lower: %f \n", sat_pos_lower.svprn, time_lower);
     printf("3. Sat: %02d Time Mid:   %f \n", sat_pos_lower.svprn, time_mid);
     printf("3. Sat: %02d Time Upper: %f \n", sat_pos_lower.svprn, time_upper);
*/

     /* Set the midpoint elevation *******************************************************/

     t.tm_gps.rcvtime = lrint(time_mid * 1000);
     satellite_position(snv, SNV, location, PRr_NOMINAL, &t,  &sat_pos_mid);
     elevation_mid = get_elevation(&sat_pos_mid, location);

     /************************************************************************************/
     /* floor will make my sad closer to chad davis' */
     satellite_position_information->xyzae.sad = floor(time_mid - (master_time->tm_gps.rcvtime / 1000.0));

     s = satellite_position_information->xyzae.sad;
     h = s;
     s = s % 3600;
     h -= s;
     h /= 3600;

     m = s;
     s = s % 60;
     m -= s;
     m /= 60;

/*
     printf("\nSat: %02d Time Current: %d, Sad: %f, Duration: %02d:%02d:%02d El: %f\n\n", snv->prnnum + 1, master_time->tm_gps.rcvtime, satellite_position_information->xyzae.sad, h,m,s,elevation_mid); 
*/
}

