#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H 1

#include "../gps.h"

struct str_binary_search_node
{
     int aode;
     unsigned int time;
};

struct str_binary_search_array
{
     int head;
     struct str_binary_search_node history[EPHEMERIS_ARCHIVE_SIZE];
};

void binary_search_snv_history_insert(GDBM_FILE laas_dictionary,
     struct str_binary_search_array *emh,
     int prn,
     int aode,
     time_t time);
void binary_search_snv_history_print(struct str_binary_search_array *emh);
unsigned int binary_search(struct str_binary_search_array *emh, unsigned int value);

#endif
