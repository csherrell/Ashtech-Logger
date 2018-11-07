#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../gdbm/gdbm_dictionary.h"

void
binary_search_snv_history_insert(GDBM_FILE laas_dictionary,
          struct str_binary_search_array *emh,
          int prn,
          int aode,
          time_t time)
{
     printf("EPHEMERIS_ARCHIVE_SIZE: %d, Head: %d\n", EPHEMERIS_ARCHIVE_SIZE, emh->head);
     if(emh->head < EPHEMERIS_ARCHIVE_SIZE)
     {
          emh->history[emh->head].time = time;
          emh->history[emh->head].aode = aode;
          emh->head++;
     } else {
          if(gdbm_snv_history_remove(laas_dictionary, prn, emh->history[0].aode) == 0)
          {
               printf("gdbm_snv_history_remove: Successful\n");
          } else {
               printf("gdbm_snv_history_remove: Failed\n");
          }
          /* Move the list up by one slot */
          memcpy(&emh->history[0], &emh->history[1], sizeof(struct str_binary_search_node) * (EPHEMERIS_ARCHIVE_SIZE - 1));

          /* Add the new data to the last slot */
          emh->history[EPHEMERIS_ARCHIVE_SIZE - 1].time = time;
          emh->history[EPHEMERIS_ARCHIVE_SIZE - 1].aode = aode;
     }
     gdbm_binary_search_array_update(laas_dictionary, emh, prn);
}

void
binary_search_snv_history_print(struct str_binary_search_array *emh)
{
     int i = 0;
     for(i=0;i < emh->head; i++)
     {
          printf("%02d. Time:%d\n", i, emh->history[i].time);
     }
}

unsigned int
binary_search(struct str_binary_search_array *emh, unsigned int value)
{
     int low  = 0;
     int mid  = 0;
     int high = emh->head - 1;

     printf("Searching for: %d\n", value);
     printf("Array Head: %d\n", emh->head);
     if(value <= emh->history[low].time)
          return low;
     else if(value >= emh->history[high].time)
          return high;

     while(low <= high)
     {
          mid = (low + high) / 2;

          if (emh->history[mid].time > value)
          {
               high = mid - 1;
          } else if (emh->history[mid].time < value) {
               low = mid + 1;
          }
          else {
               printf("Mid Found: %d\n", mid);
               return mid;
          }
     }

     printf("Value:      %d\n", value);
     printf("Low:    %d, %5d, %5d\n", low,  emh->history[low].time, abs(value - emh->history[low].time));
     printf("Mid:    %d, %5d, %5d\n", mid,  emh->history[mid].time, abs(value - emh->history[mid].time));
     printf("High:   %d, %5d, %5d\n", high, emh->history[high].time,abs(value - emh->history[high].time));

     if(abs(value - emh->history[low].time) < abs(value - emh->history[high].time))
     {
          printf("Low:    %d\n", emh->history[low].time);
          return low;
     } else {
          printf("High:    %d\n", emh->history[high].time);
          return high;
     }
}
