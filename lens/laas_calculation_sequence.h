void laas_calculation_sequence(
          struct str_time        *master_time_current,
          struct str_limits      *limits,
          struct str_location    *centroid_location,
          struct str_location    reference_stations[],
          struct str_location    local_monitors[],
          struct str_gg12_device gg12s[],
          struct str_satellite_almanac_ephemeris_position all_satellites_aep_information[],
          FILE *xyzae_file,
          GDBM_FILE snv_archive_db);
