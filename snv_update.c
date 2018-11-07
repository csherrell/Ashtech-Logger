#define _ISOC9X_SOURCE  1 /* Used for lrint */
#define _ISOC99_SOURCE  1 /* Used for lrint */
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdbm.h>

#include "laas.h"
#include "gps.h"
#include "time_conversions.h"
#include "satellite_position.h"
#include "ashtech/ashtech.h"
#include "mathematical_constants.h"
#include "gdbm/gdbm_dictionary.h"
#include "binary_search/binary_search.h"
#include "azimuth_elevation.h"
#include "source_availability_duration.h"


int snv_archive(GDBM_FILE dbf,
                struct str_ashtech_snv *snv,
                struct str_binary_search_array *binary_search_ephemeris)
{
     if(gdbm_snv_exists(dbf, snv->prnnum, snv->aode))
     {
          printf("Key Already Exists\n");
     } else {
          binary_search_snv_history_insert(dbf,
                    binary_search_ephemeris,
                    snv->prnnum,
                    snv->aode,
                    snv->time_received.tm_unix.sec);
          gdbm_snv_archive_history(dbf, snv);
     }
     return 0;
}

int snv_update_current(GDBM_FILE dbf, struct str_ashtech_snv *snv)
{
     return gdbm_snv_update_current(dbf, snv);
}

/*
unsigned int search_snv(int prn, unsigned int yesterday)
{
     return 0;
}
*/

void snv_failed_validation(
          struct str_ashtech_snv *snv_in,
          struct str_ashtech_snv *snv_current,
          struct str_ashtech_snv *snv_hold,
          struct str_time *time_received)
{
          printf("SNV FAILED VALIDATION: %d, %d, %d, %d, %d\n", 
                     time_received->tm_gps.rcvtime,
                     snv_in->prnnum + 1, 
                     snv_in->aode,
                     snv_hold->prnnum + 1,
                     snv_hold->aode);
          fflush(stdout);
          ashtech_zero_snv(snv_current);
          ashtech_zero_snv(snv_hold);

          snv_current->valid = true;
          snv_hold->valid    = true;

          snv_current->time_received.tm_gps.rcvtime = time_received->tm_gps.rcvtime;
}

bool
snv_validate_by_almanac(
          struct str_ashtech_sal *sal,
          struct str_ashtech_snv *snv_in,
          struct str_ashtech_snv *snv_current,
          struct str_ashtech_snv *snv_hold,
          struct str_location    *centroid_location,
          struct str_time *time_received,
          FILE *alm_csv,
          FILE *eph_csv)
{
     bool update_flag = true;
     double distance = 0;
     struct str_satellite_position snv_sat_pos;
     struct str_satellite_position sal_sat_pos;
     struct str_time time_next;

     printf("snv_validate_by_almanac()\n");
     printf("sal:         %d, %d\n", sal->prn, sal->wna);
     printf("snv_in:      %d, %d\n", snv_in->prnnum, snv_in->aode);
     printf("snv_current: %d, %d\n", snv_current->prnnum, snv_current->aode);

     memset(&snv_sat_pos, 0x00, sizeof(struct str_satellite_position));
     memset(&sal_sat_pos, 0x00, sizeof(struct str_satellite_position));
     memcpy(&time_next, &snv_in->time_received, sizeof(struct str_time));

     satellite_position((void *) snv_in, SNV, centroid_location, PRr_NOMINAL, &time_next, &snv_sat_pos);
     satellite_position((void *) sal, SAL, centroid_location, PRr_NOMINAL, &time_next, &sal_sat_pos);

     distance = get_satellite_distance(&snv_sat_pos, &sal_sat_pos);
     printf("SNV XYZ: %f, %f, %f\n", snv_sat_pos.x, snv_sat_pos.y, snv_sat_pos.z);
     printf("SAL XYZ: %f, %f, %f\n", sal_sat_pos.x, sal_sat_pos.y, sal_sat_pos.z);
     /* printf("Satellite Distance (Current): %f\n", distance); */
     fprintf(alm_csv, "%d, %d, %24.24f,", time_next.tm_gps.rcvtime, snv_in->prnnum + 1, distance);
     if(distance > 7000)
     {
          update_flag = false;
     }
     
     gps_add_time(&time_next,  6 * 60 * 60 * 1000); /* Add 6 Hours */
     satellite_position((void *) snv_in, SNV, centroid_location, PRr_NOMINAL, &time_next, &snv_sat_pos);
     satellite_position((void *) sal, SAL, centroid_location, PRr_NOMINAL, &time_next, &sal_sat_pos);
     distance = get_satellite_distance(&snv_sat_pos, &sal_sat_pos);
     /* printf("Satellite Distance (6 Hours): %f\n", distance); */
     fprintf(alm_csv, " %24.24f,", distance);
     if(distance > 7000)
     {
          update_flag = false;
     }
     
     gps_add_time(&time_next,  6 * 60 * 60 * 1000); /* Add another 6 Hours */
     satellite_position((void *) snv_in, SNV, centroid_location, PRr_NOMINAL, &time_next, &snv_sat_pos);
     satellite_position((void *) sal, SAL, centroid_location, PRr_NOMINAL, &time_next, &sal_sat_pos);
     distance = get_satellite_distance(&snv_sat_pos, &sal_sat_pos);
     /* printf("Satellite Distance (12 Hours): %f\n", distance); */
     fprintf(alm_csv, " %24.24f,", distance);
     if(distance > 7000)
     {
          update_flag = false;
     }

     fprintf(alm_csv, " %d\n", update_flag);
     fflush(alm_csv);
     return update_flag;
}

bool
snv_validate_by_almanac_test_case(
          struct str_ashtech_sal *sal,
          struct str_ashtech_snv *snv_in,
          struct str_ashtech_snv *snv_current,
          struct str_ashtech_snv *snv_hold,
          struct str_location    *centroid_location,
          struct str_time *time_received,
          FILE *alm_csv,
          FILE *eph_csv)
{
     bool status = snv_validate_by_almanac(
               sal,
               snv_in,
               snv_current,
               snv_hold,
               centroid_location,
               time_received,
               alm_csv,
               eph_csv);

     if(status == true)
     {
          /* printf("Copy to hold\n"); */
          memcpy(snv_hold,snv_in,sizeof(struct str_ashtech_snv));
     } else {
          printf("snv_validate_by_almanac_test_case calling snv_failed_validation\n");
          snv_failed_validation(snv_in, snv_current, snv_hold, time_received);
     }
     return status;
}


void snv_validate_by_yesterdays_ephemeris(
          struct str_ashtech_sal *sal_current,
          struct str_ashtech_snv *snv_in,
          struct str_ashtech_snv *snv_current,
          struct str_ashtech_snv *snv_hold,
          struct str_location    *centroid_location,
          struct str_time        *time_received,
          struct str_binary_search_array *binary_search_ephemeris,
          GDBM_FILE snv_archive_db,
          FILE *alm_csv,
          FILE *eph_csv)
{
     bool status = false;

     struct str_time yesterday;
     struct str_ashtech_snv snv_yesterday;
     int    snv_closest_index = 0;
     int    snv_closest_aode  = 0;
     time_t snv_closest_time  = 0;

     double distance = 0;
     struct str_satellite_position sat_pos_1;
     struct str_satellite_position sat_pos_2;

     printf("  snv_validate_by_yesterdays_ephemeris()\n");
     memcpy(&yesterday, time_received, sizeof(struct str_time));
     memset(&snv_yesterday, 0x00, sizeof(struct str_ashtech_snv));
     memset(&sat_pos_1, 0x00, sizeof(struct str_satellite_position));
     memset(&sat_pos_2, 0x00, sizeof(struct str_satellite_position));

     /** Since there is no previous ephemeris, yesterday's ephemeris is used as the prediction  */
     /** XYZ should be close to the same every sidereal day (1 sidereal day = 23.9344696 hours) */
     yesterday.tm_unix.sec = time_received->tm_unix.sec - lrint(SIDEREAL_DAY*60*60); /* *1000; (milliseconds) */
     time_unix_to_gps(&yesterday);

     /** the search_snv() function finds the ephemeris for the
          specified prn (i) closest to unix_time_yesterday */
     snv_closest_index = binary_search(binary_search_ephemeris, (unsigned int) yesterday.tm_unix.sec);
     snv_closest_time = binary_search_ephemeris->history[snv_closest_index].time;

     /** Declaration: int abs(int) */
     printf("Current:   %d\n", (unsigned int) time_received->tm_unix.sec);
     printf("Yesterday: %d\n", (unsigned int) yesterday.tm_unix.sec);
     printf("Closest:   %d\n", (unsigned int) snv_closest_time);
     printf("Time Difference:%d\n", abs(snv_closest_time - yesterday.tm_unix.sec));
     if(abs(snv_closest_time - yesterday.tm_unix.sec) > 2*60*60) /** (2 hours) */
     {

          /** Prediction cannot be made, no validation possible */
          printf("snv_validate_by_yesterdays_ephemeris(1) calling snv_failed_validation\n");
          snv_failed_validation(snv_in, snv_current, snv_hold, time_received);
          snv_in->valid = false;

          /** This ephemeris is archived even though it has not been validated           */
          /** If the YE-TE test passes 1 day later it is assumed this ephemeris is valid */
          snv_archive(snv_archive_db, snv_in, binary_search_ephemeris);

     } else {
          status = snv_validate_by_almanac(
                              sal_current,
                              snv_in,
                              snv_current,
                              snv_hold,
                              centroid_location,
                              time_received,
                              alm_csv,
                              eph_csv);

          if(status == true)
          {
               printf("Status True:\n");
               snv_closest_aode = binary_search_ephemeris->history[snv_closest_index].aode;
               gdbm_snv_fetch_history(snv_archive_db, snv_in->prnnum, snv_closest_aode, &snv_yesterday);
               /* ^- Check the return code from gdbm_snv_fetch_history */
               satellite_position((void *) snv_in,         SNV, centroid_location, PRr_NOMINAL, time_received, &sat_pos_1);
               satellite_position((void *) &snv_yesterday, SNV, centroid_location, PRr_NOMINAL, time_received, &sat_pos_2);
/*
               satellite_position((void *) &snv_yesterday, SNV, centroid_location, PRr_NOMINAL, yesterday.tm_gps.rcvtime, &sat_pos_2);
*/
               distance = get_satellite_distance(&sat_pos_1, &sat_pos_2);
               printf("Time: Current: %d Yesterday: %d Distance: %f\n", time_received->tm_gps.rcvtime, yesterday.tm_gps.rcvtime, distance);
               if(distance < MEDE)
               {
                    printf("Copy to hold\n");
                    snv_archive(snv_archive_db, snv_in, binary_search_ephemeris);
                    memcpy(snv_hold, snv_in, sizeof(struct str_ashtech_snv));
               } else {
                    printf("snv_validate_by_yesterdays_ephemeris(2) calling snv_failed_validation\n");
                    snv_failed_validation(snv_in, snv_current, snv_hold, time_received);
               }
          } else {
               printf("snv_validate_by_yesterdays_ephemeris(3) calling snv_failed_validation\n");
               snv_failed_validation(snv_in, snv_current, snv_hold, time_received);
          }
     }
}

void snv_validate_by_previous_ephemeris(
          struct str_ashtech_snv *snv_in,
          struct str_ashtech_snv *snv_current,
          struct str_ashtech_snv *snv_hold,
          struct str_location    *centroid_location,
          struct str_time        *time_received,
          struct str_binary_search_array *binary_search_ephemeris,
          GDBM_FILE snv_archive_db)
{
     double distance = 0;
     struct str_satellite_position sat_pos_in;
     struct str_satellite_position sat_pos_current;

     printf("  snv_validate_by_previous_ephemeris()");

     memset(&sat_pos_in, 0x00, sizeof(struct str_satellite_position));
     memset(&sat_pos_current, 0x00, sizeof(struct str_satellite_position));

     satellite_position((void *) snv_in, SNV, centroid_location, PRr_NOMINAL, time_received, &sat_pos_in);
     satellite_position((void *) snv_current, SNV, centroid_location, PRr_NOMINAL, time_received, &sat_pos_current);
     distance = get_satellite_distance(&sat_pos_in, &sat_pos_current);

     printf(": distance: %f\n", distance);

     if(distance < 250)
     {
          snv_archive(snv_archive_db, snv_in, binary_search_ephemeris);
          memcpy(snv_hold,snv_in,sizeof(struct str_ashtech_snv));
     } else {
          printf("snv_validate_by_previous_ephemeris calling snv_failed_validation\n");
          snv_failed_validation(snv_in, snv_current, snv_hold, time_received);
     }
}

int snv_validate(struct str_satellite_almanac_ephemeris_position *satellite_aep_information,
          struct str_ashtech_snv *snv_in,
          struct str_binary_search_array *binary_search_ephemeris,
          struct str_location    *centroid_location,
          struct str_time        *time_received,
          FILE *alm_csv,
          FILE *eph_csv,
          GDBM_FILE snv_archive_db)
{
/*
     centroid_location->x = -677736.175;
     centroid_location->y = -5171015.741;
     centroid_location->z = 3660053.531;
     centroid_location->latitude = 35.24231911509;
     centroid_location->longitude = -97.4668771304;
     centroid_location->altitude = 0;
*/

     /* printf("snv_validate()\n"); */
     if(satellite_aep_information->snv.valid == true)
     {
          /* Proceed with ephemeris validation */
          /* If the iode of the incoming snv does not match the "in use" snv or 
             the snv in hold then it is new and must be validated */
          if(snv_in->aode != satellite_aep_information->snv.aode      &&
             snv_in->aode != satellite_aep_information->snv_hold.aode &&
             snv_in->aode != satellite_aep_information->previous_aode)
          {
               /* If there is not a current "in use" ephemeris then the ephemeris has to be
                  validated with by the almanac, but if there is a currently "in use" ephemeris
                  then it will be used to preform the validation */

/* TEST_CASE: For testing only this*/
               snv_validate_by_almanac_test_case(
                         &satellite_aep_information->sal,
                         snv_in,
                         &satellite_aep_information->snv,
                         &satellite_aep_information->snv_hold,
                         centroid_location,
                         time_received,
                         alm_csv,
                         eph_csv);

/* REAL_CASE */
/*
               if(satellite_aep_information->snv.checksum == 0)
               {
                    snv_validate_by_yesterdays_ephemeris(
                              &satellite_aep_information->sal,
                              snv_in,
                              &satellite_aep_information->snv,
                              &satellite_aep_information->snv_hold,
                              centroid_location,
                              time_received,
                              binary_search_ephemeris,
                              snv_archive_db);
               } else {
                    snv_validate_by_previous_ephemeris(
                              snv_in,
                              &satellite_aep_information->snv,
                              &satellite_aep_information->snv_hold,
                              centroid_location,
                              time_received,
                              binary_search_ephemeris,
                              snv_archive_db);
               }
*/
          }
     } else {
          /* Wait for 10 Hours */
          /* 10 Hours * 60 Minutes in an hour * 60 Seconds in a minute * 1000 Milliseconds in a second) */
          if((time_received->tm_gps.rcvtime - satellite_aep_information->snv.time_received.tm_gps.rcvtime) >= 36000000)
          {
               satellite_aep_information->snv.valid = true;
          }
     }
     return 0;
}

int snv_hold_check(struct str_satellite_almanac_ephemeris_position *all_sats_aep_information,
          struct str_location *centroid_location,
          struct str_time *master_time,
          GDBM_FILE snv_archive_db)
{
     struct str_ashtech_snv    *snv_current = NULL;
     struct str_ashtech_snv       *snv_hold = NULL;
     struct str_satellite_position   *xyzae = NULL;

     /*printf("snv_hold_check()\n");*/

     snv_current = &all_sats_aep_information->snv;
     snv_hold    = &all_sats_aep_information->snv_hold;
     xyzae       = &all_sats_aep_information->xyzae;

     if(snv_hold->checksum != 0)
     {
          /* printf("%d: Wating: %d\n", snv_hold->prnnum, ((2 * 60) - ((master_time->tm_gps.rcvtime - snv_hold->time_received.tm_gps.rcvtime)/1000))); */
          if((master_time->tm_gps.rcvtime - snv_hold->time_received.tm_gps.rcvtime) > (2 * 60 * 1000))
          {
               all_sats_aep_information->previous_aode = snv_current->aode;

               /* Copy hold to current */
               /* printf("Copy to current\n"); */
               memcpy(snv_current, snv_hold, sizeof(struct str_ashtech_snv));
               /* Update current in the database binary search array*/
               snv_update_current(snv_archive_db, snv_current);
               /* Zero out hold */
               memset(snv_hold, 0x00, sizeof(struct str_ashtech_snv));
               snv_hold->valid = true;

               /* Update SAT POS and SAD for snv_current */
               satellite_position(
                         &all_sats_aep_information->snv, SNV,
                         centroid_location,
                         PRr_NOMINAL,
                         master_time,
                         &all_sats_aep_information->xyzae);

               azimuth_elevation(
                         &all_sats_aep_information->xyzae,
                         centroid_location->latitude, centroid_location->longitude,
                         centroid_location->x, centroid_location->y, centroid_location->z);

               /* printf("Calling source_availability_duration()\n"); */

               source_availability_duration(
                         all_sats_aep_information,
                         centroid_location, master_time);

               /* all_sats_aep_information->xyzae.sad += 0.5; */ /* Adding it on since the next block will subtract it off */
          }
     }
     return 0;
}

void clear_snv_current_and_hold(struct str_ashtech_snv *current, struct str_ashtech_snv *hold, struct str_satellite_position *xyzae)
{
     memset(current, 0x00, sizeof(struct str_ashtech_snv));
     current->valid = true;
     /* current->time_received.tm_gps.rcvtime = master_time->tm_gps.rcvtime; */

     memset(hold, 0x00, sizeof(struct str_ashtech_snv));
     hold->valid = true;
     /* hold->time_received.tm_gps.rcvtime = master_time->tm_gps.rcvtime; */

     memset(xyzae, 0x00, sizeof(struct str_satellite_position));
}

bool snv_elevation_check(struct str_satellite_almanac_ephemeris_position *all_sats_aep_information,
          struct str_time *master_time)
{
     bool elevation_check = true; /*need for testing */

     struct str_ashtech_snv    *snv_current = NULL;
     struct str_ashtech_snv       *snv_hold = NULL;
     struct str_satellite_position   *xyzae = NULL;

     snv_current = &all_sats_aep_information->snv;
     snv_hold    = &all_sats_aep_information->snv_hold;
     xyzae       = &all_sats_aep_information->xyzae;

     /*printf("snv_elevation_check()\n"); */
     if(xyzae->elevation > 0.1)
     {
          /* printf("elevation > 0.1\n"); */
          xyzae->sad = ((xyzae->sad - 0.5) > 0) ? (xyzae->sad - 0.5) : 0;

/*
          xyzae->sad -= 0.5;
          if(xyzae->sad < 0)
          {
               printf("SAD < %f\n", xyzae->sad);
               xyzae->sad = 0;
          }
*/

     } else {
          /* printf("elevation < 0.1\n"); */
          clear_snv_current_and_hold(snv_current, snv_hold, xyzae);
     }
     return elevation_check;
}
