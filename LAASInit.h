#ifndef LAASINIT_H
#define LAASINIT_H 1

/* Returns -1 if error or number of key=value pairs read from fname */
#include "laas.h"
#include "gps.h"

#include "ashtech/ashtech.h"
#include "ashtech/gg12.h"
#include "ashtech/gg12w.h"
#include "ashtech/zxtreme.h"

#include "telerad/telerad.h"

void initialize_almanac_ephemeris_data(
          struct str_satellite_almanac_ephemeris_position satellites_position_information[],
          GDBM_FILE dbf);


int parseLAASConfigurationFile(const char *configuration_file,
                            struct str_limits *limits,
                            struct str_laas_logging_control *logging_control,
                            struct str_telerad_message *telerad_message,
                            struct str_serial_device serial_devices[],
                            struct str_gg12_device gg12s[],
                            struct str_gg12w_device gg12ws[], 
                            struct str_bline_device blines[], 
                            struct str_zxtreme_device zxtremes[], 
                            struct str_telerad_device telerads[],
                            struct str_far_field_monitor_device ffms[],
                            struct str_location reference_stations[],
                            struct str_location local_monitors[]);

int configure_laas_ground_facility(const char *configuration_file, 
          struct str_time *master_time, 
          struct str_limits *limits, 
          struct str_laas_logging_control *logging_control, 
          struct str_telerad_message *telerad_message, 
          struct str_serial_device serial_devices[], 
          struct str_gg12_device gg12s[], 
          struct str_gg12w_device gg12ws[], 
          struct str_bline_device blines[], 
          struct str_zxtreme_device zxtremes[], 
          struct str_telerad_device telerads[], 
          struct str_far_field_monitor_device ffms[],
          struct str_location reference_stations[],
          struct str_location local_monitors[],
          struct str_location *centroid_location,
          FILE **xyzae,
          FILE **alm_csv,
          FILE **eph_csv,
          FILE **laas_message_type1_csv);

void  close_log_files( struct str_serial_device serial_devices[], 
          struct str_gg12_device gg12s[],
          struct str_gg12w_device gg12ws[], 
          struct str_bline_device blines[], 
          struct str_zxtreme_device zxtremes[]);

int open_log_files(
          struct str_time *master_time, 
          struct str_limits *limits, 
          struct str_laas_logging_control *logging_control, 
          struct str_serial_device serial_devices[], 
          struct str_gg12_device gg12s[],
          struct str_gg12w_device gg12ws[], 
          struct str_bline_device blines[], 
          struct str_zxtreme_device zxtremes[],
          FILE **xyzae,
          FILE **alm_csv,
          FILE **eph_csv,
          FILE **laas_message_type1_csv);

#endif

