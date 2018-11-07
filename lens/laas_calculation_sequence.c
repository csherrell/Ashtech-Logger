#include <stdio.h>
#include <string.h>
#include "../gps.h"
#include "../time_struct.h"
#include "../laas.h"
#include "../ashtech/ashtech.h"
#include "../ashtech/gg12.h"
#include "../satellite_position.h"
#include "../azimuth_elevation.h"
#include "../snv_update.h"
#include "xyzae.h"
#include "pseudorange_calculations.h"
#include "measurement_block.h"

void laas_calculation_sequence(
          struct str_time        *master_time_current,
          struct str_limits      *limits,
          struct str_location    *centroid_location,
          struct str_location    reference_stations[],
          struct str_location    local_monitors[],
          struct str_gg12_device gg12s[],
          struct str_satellite_almanac_ephemeris_position all_satellites_aep_information[],
          FILE *xyzae_file,
          GDBM_FILE laas_db)
{
     int prn_idx     = 0;
     int channel_idx = 0;
     int rr_idx      = 0;
     bool prsc_is_set[NUMBER_OF_GPS_SATELLITES];

     static bool initalized = false;
     static struct str_prsc_filter prsc_filter[NUMBER_OF_GPS_SATELLITES][MAX_NUMBER_OF_GG12S];
     static int Active[MAX_NUMBER_OF_REFERENCE_STATIONS];


     /* Why the indexing changed from one function to the next I will never know */
     double prsc_array_full[MAX_NUMBER_OF_GG12S][NUMBER_OF_GPS_SATELLITES];

     memset(prsc_array_full, 0x0, sizeof(double) * MAX_NUMBER_OF_GG12S * NUMBER_OF_GPS_SATELLITES);
     memset(prsc_is_set, false, sizeof(bool) * NUMBER_OF_GPS_SATELLITES);

     if(!initalized)
     {
          initalized = true;
          memset(prsc_filter, 0x0, sizeof(struct str_prsc_filter) * NUMBER_OF_GPS_SATELLITES * MAX_NUMBER_OF_GG12S);
          memset(Active, 0x0, sizeof(int) * MAX_NUMBER_OF_REFERENCE_STATIONS);
          Active[0] = 0;
          Active[1] = 1;
          Active[2] = 2;
          Active[3] = 3;
     }
     
     /** Preprocess the MCA data so that it can be reverence by prn. */
     for(rr_idx = 0; rr_idx < limits->NUMBER_OF_REFERENCE_STATIONS*2; rr_idx++)
     {
          for(channel_idx = 0; channel_idx < GG12_CHANNELS; channel_idx++)
          {
               if(gg12s[rr_idx].mca.channel[channel_idx].new_message == true)
               {
                    prn_idx = (gg12s[rr_idx].mca.channel[channel_idx].svprn - 1);
                    prsc_filter[prn_idx][rr_idx].goodbad    = gg12s[rr_idx].mca.channel[channel_idx].goodbad;
                    prsc_filter[prn_idx][rr_idx].raw_range  = gg12s[rr_idx].mca.channel[channel_idx].raw_range;
                    prsc_filter[prn_idx][rr_idx].phase      = gg12s[rr_idx].mca.channel[channel_idx].full_phase;
                    prsc_filter[prn_idx][rr_idx].snr        = gg12s[rr_idx].mca.channel[channel_idx].ireg;
                    /* printf("1. RCVTIME: %d, PRN: %d, IREG: %d\n", master_time_current->tm_gps.rcvtime, prn_idx, prsc_filter[prn_idx][rr_idx].snr); */
               }
               gg12s[rr_idx].mca.channel[channel_idx].new_message = false;
               gg12s[rr_idx].mca.channel[channel_idx].checksum    = 0;
          }
     }
     
     /* printf("Start LAAS Calculations\n"); */
     for(prn_idx = 0; prn_idx < NUMBER_OF_GPS_SATELLITES; prn_idx++)
     {
          snv_hold_check(&all_satellites_aep_information[prn_idx], centroid_location, master_time_current, laas_db);

          for(rr_idx = 0; rr_idx < limits->NUMBER_OF_REFERENCE_STATIONS*2; rr_idx++)
          {
/*
               printf("PRs: RCVTIME: %d, PRN %d, RR: %d\n", master_time_current->tm_gps.rcvtime, prn_idx + 1, rr_idx);
               printf("2. RCVTIME: %d, PRN: %d, IREG: %d\n", master_time_current->tm_gps.rcvtime, prn_idx, prsc_filter[prn_idx][rr_idx].snr);
               printf("3. RCVTIME: %d, PRN: %d, IREG: %d\n", master_time_current->tm_gps.rcvtime, prn_idx, prsc_filter[prn_idx][rr_idx].snr);
*/
               Calc_PR_s(&prsc_filter[prn_idx][rr_idx]);
          }

          /* printf("PRN Index: %d\n", prn_idx + 1); */
          if(all_satellites_aep_information[prn_idx].snv.checksum != 0 &&
             all_satellites_aep_information[prn_idx].snv.valid    == true)
          {
               /* printf("SNV Valid On Index: %d\n", prn_idx + 1); */
               /* printf("Calling: satellite_position()\n"); */
               satellite_position(
                         &all_satellites_aep_information[prn_idx].snv, SNV,
                         centroid_location,
                         PRr_NOMINAL,
                         master_time_current,
                         &all_satellites_aep_information[prn_idx].xyzae);

               /*printf("Calling: azimuth_elevation()\n"); */
               azimuth_elevation(
                         &all_satellites_aep_information[prn_idx].xyzae,
                         centroid_location->latitude, centroid_location->longitude,
                         centroid_location->x, centroid_location->y, centroid_location->z);

               /*printf("Calling: snv_elevation_check()\n"); */
               snv_elevation_check(&all_satellites_aep_information[prn_idx], master_time_current); 

               for(rr_idx = 0; rr_idx < limits->NUMBER_OF_REFERENCE_STATIONS*2; rr_idx++)
               {
                    /* printf("PRsc: RCVTIME: %d, PRN %d, RR: %d\n", master_time_current->tm_gps.rcvtime, prn_idx + 1, rr_idx); */
                    Calc_PR_sc(&all_satellites_aep_information[prn_idx].snv,
                              &all_satellites_aep_information[prn_idx].xyzae,
                              &reference_stations[rr_idx/2],
                              &prsc_filter[prn_idx][rr_idx]);
               }
          }

          if(all_satellites_aep_information[prn_idx].snv.checksum != 0)
          {
               /*printf("Calling: lens_print_xyzae()\n"); */
               lens_print_xyzae(&all_satellites_aep_information[prn_idx].xyzae, master_time_current->tm_gps.rcvtime, all_satellites_aep_information[prn_idx].snv.aode, xyzae_file);
          }
     }


     /* Convert the array from RRxPRN (8x32) to PRNxRR (32x8) */
     for(rr_idx = 0; rr_idx < limits->NUMBER_OF_REFERENCE_STATIONS*2; rr_idx++)
     {
          for(prn_idx = 0; prn_idx < NUMBER_OF_GPS_SATELLITES; prn_idx++)
          {
               prsc_array_full[rr_idx][prn_idx] = prsc_filter[prn_idx][rr_idx].prsc;
               if(prsc_array_full[rr_idx][prn_idx] != 0)
               {
                    prsc_is_set[prn_idx] = true;
               }
          }
     }

/* For printing only */
     for(prn_idx = 0; prn_idx < NUMBER_OF_GPS_SATELLITES; prn_idx++)
     {
          if(prsc_is_set[prn_idx])
          {
               printf("PRsc: %d, %d, ", master_time_current->tm_gps.rcvtime, prn_idx + 1);
               for(rr_idx = 0; rr_idx < limits->NUMBER_OF_REFERENCE_STATIONS*2 - 1; rr_idx++)
               {
                         printf("%f, ", prsc_filter[prn_idx][rr_idx].prsc);
               }
               printf("%f\n", prsc_filter[prn_idx][rr_idx].prsc);
          }
          prsc_is_set[prn_idx] = false;
     }

     Active[0] = Select_RS(prsc_array_full[0], 0, prsc_array_full[1], 1, Active[0]);
     Active[1] = Select_RS(prsc_array_full[2], 2, prsc_array_full[3], 3, Active[1]);
     Active[2] = Select_RS(prsc_array_full[4], 4, prsc_array_full[5], 5, Active[2]);
     Active[3] = Select_RS(prsc_array_full[6], 6, prsc_array_full[7], 7, Active[3]);
     /* printf("Active: %d, %d, %d, %d\n", Active[0], Active[1], Active[2], Active[3]); */ 

     /* prsc_array_full is an 8x32 multidemsional array. Active selects which of the 4 to use */
     Calc_PR_sca(prsc_array_full, Active);

     /* prsc_array_full is an 8x32 multidemsional array. */
     /* After the call to Calc_PR_Sca, 0-3 hold the prsca and 4-7 are unused */
     build_measurement_block(prsc_array_full, all_satellites_aep_information);
}
