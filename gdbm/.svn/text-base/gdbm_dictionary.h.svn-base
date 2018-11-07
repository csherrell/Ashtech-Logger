#ifndef GDBM_DICTIONARY_H
#define GDBM_DICTIONARY_H 1

#include <gdbm.h>
#include "../binary_search/binary_search.h"

void gdbm_delete_list(GDBM_FILE laas_dictionary);

void gdbm_initialize_laas_data(GDBM_FILE laas_dictionary,
     struct str_satellite_almanac_ephemeris_position satellites_position_information[],
     struct str_binary_search_array ephemeris_measurement[]);

void gdbm_initialize_binary_search_array(GDBM_FILE laas_dictionary,
     struct str_binary_search_array ephemeris_measurement[]);

void gdbm_initialize_almanac_ephemeris(GDBM_FILE laas_dictionary,
     struct str_satellite_almanac_ephemeris_position satellites_position_information[]);

int gdbm_store_to_dictionary(GDBM_FILE laas_dictionary, datum key, datum content);
int gdbm_fetch_from_dictionary(GDBM_FILE laas_dictionary, datum key, datum *content);
int gdbm_delete_from_dictionary(GDBM_FILE laas_dictionary, datum key);

int gdbm_binary_search_array_update(GDBM_FILE laas_dictionary, struct str_binary_search_array *emh, int prn);

int gdbm_snv_update_current(GDBM_FILE laas_dictionary, struct str_ashtech_snv *snv);
int gdbm_sal_update_current(GDBM_FILE laas_dictionary, struct str_ashtech_sal *sal);

int gdbm_snv_archive_history(GDBM_FILE laas_dictionary, struct str_ashtech_snv *snv);
int gdbm_snv_fetch_history(GDBM_FILE laas_dictionary, int prn, int aode, struct str_ashtech_snv *snv);
int gdbm_snv_history_remove(GDBM_FILE laas_dictionary, int prn, int aode);
bool gdbm_snv_exists(GDBM_FILE laas_dictionary, int prn, int aode);

int gdbm_store_array_head(GDBM_FILE laas_dictionary, int array_head[]);
int gdbm_fetch_array_head(GDBM_FILE laas_dictionary, int array_head[]);

#endif
