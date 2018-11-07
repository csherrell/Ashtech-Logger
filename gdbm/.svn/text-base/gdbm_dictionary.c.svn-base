#include <stdarg.h>
#include <sys/time.h>
#include <time.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "../laas.h"
#include "../gps.h"
#include "gdbm_dictionary.h"

void gdbm_delete_list(GDBM_FILE laas_dictionary)
{
     # define items 14
     char list[items][80] =
{"SNV_ARCHIVE-31-1207610105",
"SNV_ARCHIVE-13-1207618332",
"SNV_ARCHIVE-22-1207616402",
"SNV_ARCHIVE-29-1207614603",
"SNV_ARCHIVE-02-1207619100",
"SNV_ARCHIVE-12-1207616402",
"SNV_ARCHIVE-19-1207619101",
"SNV_ARCHIVE-05-1207614036",
"SNV_ARCHIVE-28-1207614402",
"SNV_ARCHIVE-22-1207615502",
"SNV_ARCHIVE-15-1207614883",
"SNV_ARCHIVE-19-1207616148",
"SNV_ARCHIVE-29-1207615578",
"SNV_ARCHIVE-19-1207625286"};

     datum key;
     
     int i = 0;

     int prn = 0;
     unsigned int mytime;
     
     for(i=0; i < items; i++)
     {
          printf("STRING: %s\n", list[i]);
          sscanf(list[i],"SNV_ARCHIVE-%d-%u", &prn, &mytime);
          printf("PRN: %d, TIME: %d\n", prn, mytime);
          key.dptr = list[i];
          key.dsize = strlen(list[i]);
          if(gdbm_exists(laas_dictionary, key))
          {
               printf("KEY EXISTS\n");
               /*gdbm_delete_from_dictionary(laas_dictionary, key);*/
               gdbm_snv_history_remove(laas_dictionary, prn, mytime);
          }
     }
     printf("\n\n");
}

void gdbm_print_keys(GDBM_FILE laas_dictionary)
{
     char key_string[MAX_LINE_LENGTH];

     datum key;
     datum nextkey;

     key = gdbm_firstkey(laas_dictionary);
     printf("ALL KEYS:\n");
     while(key.dptr)
     {
          memset(key_string,0,MAX_LINE_LENGTH);
          memcpy(key_string,key.dptr, key.dsize);
          printf("ALL KEY: ->%s<-, Size: %d\n", key_string, key.dsize);

          nextkey = gdbm_nextkey(laas_dictionary, key);
          if(key.dptr)
          {
               free(key.dptr);
          }
          key = nextkey;
     }
     printf("\n\n");
}

void gdbm_delete_all(GDBM_FILE laas_dictionary)
{
     char key_string[MAX_LINE_LENGTH];

     datum key;
     datum nextkey;

     key = gdbm_firstkey(laas_dictionary);
     printf("ALL KEYS:\n");
     while(key.dptr)
     {
          memset(key_string,0x00,MAX_LINE_LENGTH);
          memcpy(key_string,key.dptr, key.dsize);

          nextkey = gdbm_firstkey(laas_dictionary);
          if(key.dptr)
          {
               free(key.dptr);
          }

          key.dptr = key_string;
          key.dsize = strlen(key_string);
          printf("KEY->%s<-\n",key.dptr);
          if(gdbm_exists(laas_dictionary, key))
          {
               /*printf("KEY EXISTS\n"); */
               gdbm_delete_from_dictionary(laas_dictionary, key);
          }
          key = nextkey;
     }
     printf("\n\n");
}

void gdbm_initialize_almanac_ephemeris(GDBM_FILE laas_dictionary,
     struct str_satellite_almanac_ephemeris_position satellites_position_information[])
{
     char key_string[MAX_LINE_LENGTH];
     int  i = 0;

     datum key;
     datum content;

     struct str_ashtech_sal sal_real;
     struct str_ashtech_sal *sal;
     sal = &sal_real;
     memset(sal,0,sizeof(struct str_ashtech_sal));

     /* gdbm_delete_list(laas_dictionary); */
     /* gdbm_print_keys(laas_dictionary); */
     /* gdbm_delete_all(laas_dictionary); */
     /* exit(0); */

     for(i=0; i < NUMBER_OF_GPS_SATELLITES; i++)
     {
          memset(key_string, 0x00, MAX_LINE_LENGTH);
          sprintf(key_string,"SNV_VALID-%02d",i);
          key.dptr = key_string;
          key.dsize = strlen(key_string);
          printf("Search ->%s<-\n",key.dptr);
          content.dptr  = NULL;
          content.dsize = 0;
          content = gdbm_fetch(laas_dictionary, key);
          if(content.dptr != NULL)
          {
               printf("SNV %d loaded from database\n", i);
               memcpy(&satellites_position_information[i].snv, content.dptr, content.dsize);
               satellites_position_information[i].snv.csv = NULL;
               free(content.dptr);
          } else {
               printf("SNV %d not found in the database\n", i);
          }

          memset(key_string, 0x00, MAX_LINE_LENGTH);
          sprintf(key_string,"SAL_VALID-%02d",i);
          key.dptr = key_string;
          key.dsize = strlen(key_string);
          content.dptr  = NULL;
          content.dsize = 0;
          printf("Search ->%s<-\n",key.dptr);
          content = gdbm_fetch(laas_dictionary, key);
          if(content.dptr != NULL)
          {
               printf("SAL %d loaded from database\n", i);
               memcpy(&satellites_position_information[i].sal, content.dptr, content.dsize);
               satellites_position_information[i].sal.csv = NULL;
               
               sal = &satellites_position_information[i].sal;
               free(content.dptr);
          } else {
               printf("SAL %d not found in the database\n", i);
          }
     }
}

void gdbm_initialize_binary_search_array(
          GDBM_FILE laas_dictionary,
          struct str_binary_search_array ephemeris_measurement[])
{
     char key_string[MAX_LINE_LENGTH];
     int  i = 0;

     datum key;
     datum content;

     for(i=0; i < NUMBER_OF_GPS_SATELLITES; i++)
     {
          memset(key_string, 0x00, MAX_LINE_LENGTH);
          sprintf(key_string,"BINARY_SEARCH_ARRAY-%02d", i);
          key.dptr = key_string;
          key.dsize = strlen(key_string);
          content.dptr  = NULL;
          content.dsize = 0;
          content = gdbm_fetch(laas_dictionary, key);
          if(content.dptr != NULL)
          {
               memcpy(&ephemeris_measurement[i], content.dptr, content.dsize);
               free(content.dptr);
          } else {
               printf("PRN Array %d not found in the database\n", i);
          }
     }
}

void gdbm_initialize_laas_data(
          GDBM_FILE laas_dictionary,
          struct str_satellite_almanac_ephemeris_position satellites_position_information[],
          struct str_binary_search_array ephemeris_measurement[])
{
     gdbm_initialize_binary_search_array(laas_dictionary, ephemeris_measurement);
     gdbm_initialize_almanac_ephemeris(laas_dictionary, satellites_position_information);
}

int gdbm_store_to_dictionary(GDBM_FILE laas_dictionary, datum key, datum content)
{
     /*
     printf("store_to_dictionary(): %s\n", key.dptr);
     printf("Memloc dptr:%p\n", content.dptr);
     */
     switch(gdbm_store(laas_dictionary, key, content, GDBM_REPLACE))
     {
          case -1:
               printf("Reader is trying to write\n");
               return -1;
               break;
          case 0:
               /*printf("Store Successful\n"); */
               return 0;
               break;
          case 1:
               printf("Key is already in the database\n");
               return 1;
               break;
          default:
               printf("GDBM Unknown Error\n");
               return -2;
               break;
     }
}

int gdbm_fetch_from_dictionary(GDBM_FILE laas_dictionary, datum key, datum *content)
{
     printf("fetch_from_dictionary():->%s<-\n", key.dptr);

     (*content) = gdbm_fetch(laas_dictionary, key);
     if(content->dptr != NULL)
     {
          printf("Fetch Successful - Size:%d\n", content->dsize);
          return true;
     } else {
          printf("Fetch Unsuccessful\n");
          return false;
     }
}

int gdbm_delete_from_dictionary(GDBM_FILE laas_dictionary, datum key)
{
     printf("delete_from_dictionary(): Size: %02d, ->%s<-\n", key.dsize, key.dptr);
     switch(gdbm_delete(laas_dictionary, key))
     {
          case -1:
               if(gdbm_exists(laas_dictionary, key))
               {
                    printf("Reader is trying to write\n");
               } else {
                    printf("Key Not Found\n");
               }
               return -1;
               break;
          case 0:
               printf("Delete Successful\n");
               return 0;
               break;
          default:
               printf("GDBM Unknown Error\n");
               return -2;
               break;
     }
}

int gdbm_binary_search_array_update(GDBM_FILE laas_dictionary, struct str_binary_search_array *emh, int prn)
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;
     datum content;

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"BINARY_SEARCH_ARRAY-%02d", prn);

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     content.dptr =  (char *) emh;
     content.dsize = sizeof(struct str_binary_search_array);

     return gdbm_store_to_dictionary(laas_dictionary, key, content); 
}

int gdbm_snv_archive_history(GDBM_FILE laas_dictionary, struct str_ashtech_snv *snv)
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;
     datum content;

     if(snv->prnnum + 1 > NUMBER_OF_GPS_SATELLITES)
     {
          printf("Invalid SNV PRN: %d\n", snv->prnnum);
          return -4;
     }

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"SNV_ARCHIVE-%02d-%04d", snv->prnnum, snv->aode);

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     content.dptr =  (char *) snv;
     content.dsize = sizeof(struct str_ashtech_snv);

     return gdbm_store_to_dictionary(laas_dictionary, key, content); 
}

int gdbm_snv_fetch_history(GDBM_FILE laas_dictionary, int prn, int aode, struct str_ashtech_snv *snv)
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;
     datum content;

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"SNV_ARCHIVE-%02d-%04d", prn, aode);
     printf("gdbm_snv_fetch_history: KEY:->%s<-\b", gdbm_key);

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     content.dptr  = NULL;
     content.dsize = 0;

     gdbm_fetch_from_dictionary(laas_dictionary, key, &content);

     if(content.dptr != NULL)
     {
          printf("SNV History Fetch Successful\n");
          memcpy(snv, content.dptr, content.dsize);
          free(content.dptr);
          return 0;
     } else { return -1; }
}

bool gdbm_snv_exists(GDBM_FILE laas_dictionary, int prn, int aode)
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"SNV_ARCHIVE-%02d-%04d", prn, aode);

     printf("KEY: Size:%d, ->%s<-\n", strlen(gdbm_key), gdbm_key);

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     return gdbm_exists(laas_dictionary, key);
}

int gdbm_snv_update_current(GDBM_FILE laas_dictionary, struct str_ashtech_snv *snv)
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;
     datum content;

     if(snv->prnnum + 1 > NUMBER_OF_GPS_SATELLITES)
     {
          printf("Invalid SNV PRN: %d\n", snv->prnnum);
          return -4;
     }

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"SNV_VALID-%02d", snv->prnnum);

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     content.dptr =  (char *) snv;
     content.dsize = sizeof(struct str_ashtech_snv);

     return gdbm_store_to_dictionary(laas_dictionary, key, content); 
}

int gdbm_sal_update_current(GDBM_FILE laas_dictionary, struct str_ashtech_sal *sal)
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;
     datum content;

     if(sal->prn + 1 > NUMBER_OF_GPS_SATELLITES)
     {
          printf("Invalid SAL PRN: %d\n", sal->prn);
          return -4;
     }

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"SAL_VALID-%02d", sal->prn);

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     content.dptr =  (char *) sal;
     content.dsize = sizeof(struct str_ashtech_sal);

     return gdbm_store_to_dictionary(laas_dictionary, key, content); 
}

int gdbm_snv_history_remove(GDBM_FILE laas_dictionary, int prn, int aode)
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"SNV_ARCHIVE-%02d-%04d", prn, aode);

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     return gdbm_delete_from_dictionary(laas_dictionary, key);
}

int gdbm_store_array_head(GDBM_FILE laas_dictionary, int array_head[])
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;
     datum content;

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"ARRAY_HEAD");

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     content.dptr =  (char *) array_head;
     content.dsize = sizeof(int) * NUMBER_OF_GPS_SATELLITES;

     return gdbm_store_to_dictionary(laas_dictionary, key, content); 
}

int gdbm_fetch_array_head(GDBM_FILE laas_dictionary, int array_head[])
{
     char gdbm_key[MAX_LINE_LENGTH];
     datum key;
     datum content;

     memset(gdbm_key, 0x00, MAX_LINE_LENGTH);
     sprintf(gdbm_key,"ARRAY_HEAD");

     key.dptr = gdbm_key;
     key.dsize = strlen(gdbm_key);

     content.dptr  = NULL;
     content.dsize = 0;

     gdbm_fetch_from_dictionary(laas_dictionary, key, &content);

     if(content.dptr != NULL)
     {
          memcpy(array_head, content.dptr, content.dsize);
          free(content.dptr);
          return 0;
     } else { return -1; }
}
