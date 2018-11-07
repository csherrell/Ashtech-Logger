/*
#include "laas.h"
#include "gps.h"
*/

#include "ashtech/ashtech.h"
#include "time_struct.h"
#include "gps.h"

int snv_validate(struct str_satellite_almanac_ephemeris_position *satellite_aep_information,
          struct str_ashtech_snv *snv_in,
          struct str_binary_search_array *binary_search_ephemeris,
          struct str_location    *centroid_location,
          struct str_time        *master_time,
          FILE *alm_csv,
          FILE *eph_csv,
          GDBM_FILE snv_archive_db);


int snv_hold_check(struct str_satellite_almanac_ephemeris_position *all_sats_aep_information,
          struct str_location *centroid_location,
          struct str_time     *master_time,
          GDBM_FILE snv_archive_db);

bool snv_elevation_check(struct str_satellite_almanac_ephemeris_position *all_sats_aep_information,
          struct str_time     *master_time);
