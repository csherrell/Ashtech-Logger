#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>   
/*
#include <time.h>    
#include <sys/time.h>
*/
#include <stdbool.h>    
#include <limits.h>    
#include <math.h>    

#include <sys/types.h>   
#include <sys/stat.h>   
#include <fcntl.h>   

#include "conversions.h"
#include "laas.h"
#include "serial.h"
#include "key_value_parser.h"
#include "find_string.h"
#include "ashtech/ashtech.h"
#include "ashtech/gg12.h"
#include "ashtech/gg12w.h"
#include "ashtech/zxtreme.h"
#include "LAASInit.h"
#include "time_conversions.h"
#include "time_struct.h"
#include "gps.h"

#define LINE_LENGTH 256

void
initialize_almanac_ephemeris_data_DEPRECATED(
          struct str_satellite_almanac_ephemeris_position satellites_position_information[],
          GDBM_FILE dbf)
{
     char key_string[MAX_LINE_LENGTH];
     int  i = 0;

     datum key;
     datum nextkey;
     datum content;

     key = gdbm_firstkey(dbf);
     while(key.dptr)
     {
          key.dptr[key.dsize] = '\0';
          printf("KEY: ->%s<-, Size: %d\n",(char *) key.dptr, key.dsize);
          nextkey = gdbm_nextkey( dbf, key );
          if(key.dptr)
          {
               free(key.dptr);
          }
          key = nextkey;
     }

     for(i=0; i < NUMBER_OF_GPS_SATELLITES; i++)
     {
          memset(key_string, 0x00, MAX_LINE_LENGTH);
          sprintf(key_string,"SNV-VALID-%d",i);
          key.dptr = key_string;
          key.dsize = strlen(key_string);
          printf("Search ->%s<-\n",key.dptr);
          content.dptr  = NULL;
          content.dsize = 0;
          content = gdbm_fetch(dbf, key);
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
          sprintf(key_string,"SAL-VALID-%d",i);
          key.dptr = key_string;
          key.dsize = strlen(key_string);
          content.dptr  = NULL;
          content.dsize = 0;
          content = gdbm_fetch(dbf, key);
          if(content.dptr != NULL)
          {
               printf("SAL %d loaded from database\n", i);
               memcpy(&satellites_position_information[i].sal, content.dptr, content.dsize);
               satellites_position_information[i].sal.csv = NULL;
               free(content.dptr);
          } else {
               printf("SAL %d not found in the database\n", i);
          }
     }
}


/* Returns -1 if error or number of key=value pairs read from fname */
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
          struct str_location local_monitors[])
{
     char buf[LINE_LENGTH];
     char key[LINE_LENGTH   / 2];
     char value[LINE_LENGTH / 2];
     FILE *fp;

     int index = 0;
     int sector_mask_index = 0;
     int default_value = 0;

     unsigned int uIntTemp = 0;
     double doubleTemp = 0;

     printf("Sizeof limits: %d\n",sizeof(struct str_limits));
     memset(limits, 0, sizeof(struct str_limits));

     /* Open the the file return -1 if there is an error */
     if ((fp = fopen(configuration_file, "rt")) == NULL)
     {
          printf("%s: No such file\n", configuration_file);
          fflush(stdout);
          perror(configuration_file);
          return -1;
     }

     /* Read one line at a time till EOF */
     /*
        printf("KEY:\"%s\", VALUE:\"%s\"\n",key, value);
        printf("%s\n",key);
        printf("%s\n",value);
        */
     while (fgets(buf, LINE_LENGTH, fp) != NULL)
     {    
          if(parse_key_value_pairs(buf, key, value) > 0)
          {
               if(strcmp("LOG_DIRECTORY",key) == 0) {
                    sscanf(value,"%s",logging_control->log_directory);
                    printf("LOG_DIRECTORY: %s\n",logging_control->log_directory);
               } else if(strcmp("LOCATION_IDENTIFICATION",key) == 0) {
                    sscanf(value,"%s",logging_control->location_identification);
                    printf("LOCATION_IDENTIFICATION: %s\n",logging_control->location_identification);
               } else if(strcmp("DATA_SOURCE",key) == 0) {
                    logging_control->data_source = (strcmp("FILE",value) == 0) ? DATA_FILE : SERIAL;
                    printf("DATA_SOURCE: %d\n",logging_control->data_source);
               } else if(strcmp("SERIAL_PORT_DEFAULT_TTY",key) == 0) {
                    sscanf(value,"%d",&default_value);
                    for(index = 0; index < MAX_NUMBER_OF_SERIAL_DEVICES; index++)
                    {
                         sprintf(serial_devices[index].tty,"/dev/ttyS%d",default_value);
                         printf("SERIAL_PORT_%d_TTY: %s\n",index + 1, serial_devices[index].tty);
                         default_value++;
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_BAUD",key) == 0) {
                    sscanf(value,"%d",&default_value);
                    for(index = 0; index < MAX_NUMBER_OF_SERIAL_DEVICES; index++)
                    {
                         serial_devices[index].baud_rate = default_value;
                         printf("SERIAL_PORT_%d_BAUD: %d\n",index + 1, serial_devices[index].baud_rate);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_LOGGING_CONSOLE",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_SERIAL_DEVICES; index++)
                    {
                         logging_control->serial[index].logging_console = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_LOGGING_CONSOLE: %d\n",index + 1, logging_control->serial[index].logging_console);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_LOGGING_RAW",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_SERIAL_DEVICES; index++)
                    {
                         logging_control->serial[index].logging_raw = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_LOGGING_RAW: %d\n",index + 1, logging_control->serial[index].logging_raw);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_PARSING",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_SERIAL_DEVICES; index++)
                    {
                         logging_control->serial[index].parsing = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_PARSING: %d\n",index + 1, logging_control->serial[index].parsing);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_TRANSACTION_LOG",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_SERIAL_DEVICES; index++)
                    {
                         logging_control->serial[index].transaction_log = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_TRANSACTION_LOG: %d\n",index + 1, logging_control->serial[index].transaction_log);
                    } 
/*******************************************************************************************/
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12_LOGGING_PARSED_MCA",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12S; index++)
                    {
                         /* logging_control->serial[index].gg12_logging_parsed_mca = (strcmp("ON",value) == 0) ? true : false; */
                         gg12s[index].mca.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_MCA: %d\n",index + 1, gg12s[index].mca.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12_LOGGING_PARSED_MIS",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12S; index++)
                    {
                         /* logging_control->serial[index].gg12_logging_parsed_mis = (strcmp("ON",value) == 0) ? true : false; */
                         gg12s[index].mis.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_MIS: %d\n",index + 1, gg12s[index].mis.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12_LOGGING_PARSED_PBN",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12S; index++)
                    {
                         gg12s[index].pbn.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_PBN: %d\n",index + 1, gg12s[index].pbn.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12_LOGGING_PARSED_SAL",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12S; index++)
                    {
                         gg12s[index].sal.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_SAL: %d\n",index + 1, gg12s[index].sal.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12_LOGGING_PARSED_SNV",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12S; index++)
                    {
                         gg12s[index].snv.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_SNV: %d\n",index + 1, gg12s[index].snv.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12_LOGGING_PARSED_XYZ",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12S; index++)
                    {
                         gg12s[index].xyz.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_XYZ: %d\n",index + 1, gg12s[index].xyz.logging_parsed);
                    } 
                    /*******************************************************************************************/
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12W_LOGGING_PARSED_MCA",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12WS; index++)
                    {
                         gg12ws[index].mca.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12W_LOGGING_PARSED_MCA: %d\n",
                                   index + 1,
                                   gg12ws[index].mca.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12W_LOGGING_PARSED_MIS",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12WS; index++)
                    {
                         gg12ws[index].mis.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12W_LOGGING_PARSED_MIS: %d\n",
                                   index + 1,
                                   gg12ws[index].mis.logging_parsed);
                    }
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12W_LOGGING_PARSED_PBN",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12WS; index++)
                    { 
                         gg12ws[index].pbn.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12W_LOGGING_PARSED_PBN: %d\n",
                                   index + 1,
                                   gg12ws[index].pbn.logging_parsed);
                    }
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12W_LOGGING_PARSED_SAL",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12WS; index++)
                    {
                         gg12ws[index].sal.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12W_LOGGING_PARSED_SAL: %d\n",
                                   index + 1,
                                   gg12ws[index].sal.logging_parsed);
                    }
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12W_LOGGING_PARSED_SNV",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12WS; index++)
                    {
                         gg12ws[index].snv.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12W_LOGGING_PARSED_SNV: %d\n",
                                   index + 1,
                                   gg12ws[index].snv.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_GG12W_LOGGING_PARSED_XYZ",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_GG12WS; index++)
                    {
                         gg12ws[index].xyz.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12W_LOGGING_PARSED_XYZ: %d\n",
                                   index + 1,
                                   gg12ws[index].xyz.logging_parsed);
                    }
                    /*******************************************************************************************/
               } else if(strcmp("SERIAL_PORT_DEFAULT_ZXTREME_LOGGING_PARSED_MPC",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_ZXTREMES; index++)
                    {
                         zxtremes[index].mpc.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_ZXTREME_LOGGING_PARSED_MPC: %d\n",
                                   index + 1,
                                   zxtremes[index].mpc.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_ZXTREME_LOGGING_PARSED_PBN",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_ZXTREMES; index++)
                    { 
                         zxtremes[index].pbn.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_ZXTREME_LOGGING_PARSED_PBN: %d\n",
                                   index + 1,
                                   zxtremes[index].pbn.logging_parsed);
                    }
               } else if(strcmp("SERIAL_PORT_DEFAULT_ZXTREME_LOGGING_PARSED_SAL",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_ZXTREMES; index++)
                    {
                         zxtremes[index].sal.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_ZXTREME_LOGGING_PARSED_SAL: %d\n",
                                   index + 1,
                                   zxtremes[index].sal.logging_parsed);
                    }
               } else if(strcmp("SERIAL_PORT_DEFAULT_ZXTREME_LOGGING_PARSED_SNV",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_ZXTREMES; index++)
                    {
                         zxtremes[index].snv.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_ZXTREME_LOGGING_PARSED_SNV: %d\n",
                                   index + 1,
                                   zxtremes[index].snv.logging_parsed);
                    } 
               } else if(strcmp("SERIAL_PORT_DEFAULT_ZXTREME_LOGGING_PARSED_XYZ",key) == 0) {
                    for(index = 0; index < MAX_NUMBER_OF_ZXTREMES; index++)
                    {
                         zxtremes[index].xyz.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_ZXTREME_LOGGING_PARSED_XYZ: %d\n",
                                   index + 1,
                                   zxtremes[index].xyz.logging_parsed);
                    }
                    /*******************************************************************************************/
               } else if(strncmp("SERIAL_PORT_",key,12) == 0) {
                    sscanf(key,"SERIAL_PORT_%d_%s",&index, key);
                    --index;
                    if(index >= MAX_NUMBER_OF_SERIAL_DEVICES)
                    {
                         printf("Index: %d > MAX_NUMBER_OF_SERIAL_DEVICES: %d\n",index, MAX_NUMBER_OF_SERIAL_DEVICES);
                         exit(0);
                    }
                    printf("DEBUG1: %d %s %s\n",index + 1, key, value);
                    if(strcmp("TYPE",key) == 0) {
                         /* Number of serial devices is keyed off of SERIAL PORT TYPE */
                         limits->NUMBER_OF_SERIAL_DEVICES++;
                         printf("DEBUG2: %d %s %s\n",index + 1, key, value);
                         if(strcmp("GG12",value) == 0) {
                              if(limits->NUMBER_OF_GG12S >= MAX_NUMBER_OF_GG12S)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_GG12S: %d\n",index, MAX_NUMBER_OF_GG12S);
                                   exit(0);
                              }
                              if(limits->NUMBER_OF_GPS >= MAX_NUMBER_OF_GPS)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_GPS: %d\n",index, MAX_NUMBER_OF_GPS);
                                   exit(0);
                              }
                              serial_devices[index].type = GG12;
                              gg12s[limits->NUMBER_OF_GG12S].serial_device = &serial_devices[index];
                              serial_devices[index].type_index = limits->NUMBER_OF_GG12S;
                              limits->NUMBER_OF_GG12S++;
                              limits->NUMBER_OF_GPS++;
                         } else if(strcmp("GG12W",value) == 0) {
                              if(limits->NUMBER_OF_GG12WS >= MAX_NUMBER_OF_GG12WS)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_GG12WS: %d\n",index, MAX_NUMBER_OF_GG12WS);
                                   exit(0);
                              }
                              if(limits->NUMBER_OF_GPS >= MAX_NUMBER_OF_GPS)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_GPS: %d\n",index, MAX_NUMBER_OF_GPS);
                                   exit(0);
                              }
                              serial_devices[index].type = GG12W;
                              gg12ws[limits->NUMBER_OF_GG12WS].serial_device = &serial_devices[index];
                              serial_devices[index].type_index = limits->NUMBER_OF_GG12WS;
                              limits->NUMBER_OF_GG12WS++;
                              limits->NUMBER_OF_GPS++;
                         } else if(strcmp("ZXTREME",value) == 0) {
                              if(limits->NUMBER_OF_ZXTREMES >= MAX_NUMBER_OF_ZXTREMES)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_ZXTREMES: %d\n",index, MAX_NUMBER_OF_ZXTREMES);
                                   exit(0);
                              }
                              if(limits->NUMBER_OF_GPS >= MAX_NUMBER_OF_GPS)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_GPS: %d\n",index, MAX_NUMBER_OF_GPS);
                                   exit(0);
                              }
                              serial_devices[index].type = ZXTREME;
                              zxtremes[limits->NUMBER_OF_ZXTREMES].serial_device = &serial_devices[index];
                              serial_devices[index].type_index = limits->NUMBER_OF_ZXTREMES;
                              limits->NUMBER_OF_ZXTREMES++;
                              limits->NUMBER_OF_GPS++;
                         } else if(strcmp("BLINE",value) == 0) {
                              if(limits->NUMBER_OF_BLINES >= MAX_NUMBER_OF_BLINES)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_BLINES: %d\n",index, MAX_NUMBER_OF_BLINES);
                                   exit(0);
                              }
                              if(limits->NUMBER_OF_GPS >= MAX_NUMBER_OF_GPS)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_GPS: %d\n",index, MAX_NUMBER_OF_GPS);
                                   exit(0);
                              }
                              serial_devices[index].type = BLINE;
                              blines[limits->NUMBER_OF_BLINES].serial_device = &serial_devices[index];
                              serial_devices[index].type_index = limits->NUMBER_OF_BLINES;
                              limits->NUMBER_OF_BLINES++;
                              limits->NUMBER_OF_GPS++;
                         } else if(strcmp("FFM",value) == 0) {
                              if(limits->NUMBER_OF_FAR_FIELD_MONITORS >= MAX_NUMBER_OF_FAR_FIELD_MONITORS)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_FAR_FIELD_MONITORS: %d\n",index, MAX_NUMBER_OF_FAR_FIELD_MONITORS);
                                   exit(0);
                              }
                              serial_devices[index].type = FFM;
                              ffms[limits->NUMBER_OF_FAR_FIELD_MONITORS].serial_device = &serial_devices[index];
                              serial_devices[index].type_index = limits->NUMBER_OF_FAR_FIELD_MONITORS;
                              limits->NUMBER_OF_FAR_FIELD_MONITORS++;
                         } else if(strcmp("TELERAD",value) == 0) {
                              if(limits->NUMBER_OF_TELERADS >= MAX_NUMBER_OF_TELERADS)
                              {
                                   printf("Index: %d > MAX_NUMBER_OF_TELERADS: %d\n",index, MAX_NUMBER_OF_TELERADS);
                                   exit(0);
                              }
                              serial_devices[index].type = TELERAD;
                              telerads[limits->NUMBER_OF_TELERADS].serial_device = &serial_devices[index];
                              serial_devices[index].type_index = limits->NUMBER_OF_TELERADS;
                              limits->NUMBER_OF_TELERADS++;
                         } else {
                              printf("ERROR: SERIAL_PORT: TYPE: Unknown Value\n");
                         }
                         printf("SERIAL_PORT_%d_%s: %d\n",index + 1, key, serial_devices[index].type);
                    } else if(strcmp("TYPE_SEMANTIC",key) == 0) {
                         sscanf(value,"%s",serial_devices[index].type_semantic);
                         printf("SERIAL_PORT_%d_%s: %s\n",index + 1, key, serial_devices[index].type_semantic);
                    } else if(strcmp("NAME",key) == 0) {
                         sscanf(value,"%s",serial_devices[index].name);
                         printf("SERIAL_PORT_%d_%s: %s\n",index + 1, key, serial_devices[index].name);
                    } else if(strcmp("TTY",key) == 0) {
                         sscanf(value,"%s",serial_devices[index].tty);
                         printf("SERIAL_PORT_%d_%s: %s\n",index + 1, key, serial_devices[index].tty);
                    } else if(strcmp("BAUD",key) == 0) {
                         sscanf(value,"%d",&serial_devices[index].baud_rate);
                         printf("SERIAL_PORT_%d_%s: %d\n",index + 1, key, serial_devices[index].baud_rate);
                    } else if(strcmp("LOGGING_CONSOLE",key) == 0) {
                         logging_control->serial[index].logging_console = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_LOGGING_CONSOLE: %d\n",index + 1, logging_control->serial[index].logging_console);
                    } else if(strcmp("LOGGING_RAW",key) == 0) {
                         logging_control->serial[index].logging_raw = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_LOGGING_RAW: %d\n",index + 1, logging_control->serial[index].logging_raw);
                    } else if(strcmp("PARSING",key) == 0) {
                         logging_control->serial[index].parsing = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_PARSING: %d\n",index + 1, logging_control->serial[index].parsing);
                    } else if(strcmp("GG12_LOGGING_PARSED_MCA",key) == 0) {
                         gg12s[serial_devices[index].type_index].mca.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_SNV: %d\n",index + 1, gg12s[serial_devices[index].type_index].mca.logging_parsed);
                    } else if(strcmp("TRANSACTION_LOG",key) == 0) {
                         logging_control->serial[index].transaction_log = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_TRANSACTION_LOG: %d\n",index + 1, logging_control->serial[index].transaction_log);
                    } else if(strcmp("GG12_LOGGING_PARSED_MIS",key) == 0) {
                         gg12s[serial_devices[index].type_index].mis.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_SNV: %d\n",index + 1, gg12s[serial_devices[index].type_index].mis.logging_parsed);
                    } else if(strcmp("GG12_LOGGING_PARSED_PBN",key) == 0) {
                         gg12s[serial_devices[index].type_index].pbn.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_SNV: %d\n",index + 1, gg12s[serial_devices[index].type_index].pbn.logging_parsed);
                    } else if(strcmp("GG12_LOGGING_PARSED_SAL",key) == 0) {
                         gg12s[serial_devices[index].type_index].sal.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_SNV: %d\n",index + 1, gg12s[serial_devices[index].type_index].sal.logging_parsed);
                    } else if(strcmp("GG12_LOGGING_PARSED_SNV",key) == 0) {
                         gg12s[serial_devices[index].type_index].snv.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_SNV: %d\n",index + 1, gg12s[serial_devices[index].type_index].snv.logging_parsed);
                    } else if(strcmp("GG12_LOGGING_PARSED_XYZ",key) == 0) {
                         gg12s[serial_devices[index].type_index].xyz.logging_parsed = (strcmp("ON",value) == 0) ? true : false;
                         printf("SERIAL_PORT_%d_GG12_LOGGING_PARSED_XYZ: %d\n",index + 1, gg12s[serial_devices[index].type_index].xyz.logging_parsed);
                    } else {
                         printf("ERROR: SERIAL_PORT: Unknown Value\n");
                    }
               } else if(strncmp("RS_",key,3) == 0) {
                    sscanf(key,"RS_%d_%s",&index, key);
                    --index;
                    if(strcmp("X",key) == 0){
                         limits->NUMBER_OF_REFERENCE_STATIONS++;
                         sscanf(value,"%lf",&reference_stations[index].x);
                         printf("RS_%d_X: %f\n",index + 1, reference_stations[index].x);
                    } else if(strcmp("Y", key) == 0) {
                         sscanf(value,"%lf",&reference_stations[index].y);
                         printf("RS_%d_Y: %f\n",index + 1, reference_stations[index].y);
                    } else if(strcmp("Z", key) == 0) {
                         sscanf(value,"%lf",&reference_stations[index].z);
                         printf("RS_%d_Z: %f\n",index + 1, reference_stations[index].z);
                    } else if(strcmp("LATITUDE", key) == 0) {
                         sscanf(value,"%lf",&reference_stations[index].latitude);
                         printf("RS_%d_LATITUDE: %f\n",index + 1, reference_stations[index].latitude);
                    } else if(strcmp("LONGITUDE", key) == 0) {
                         sscanf(value,"%lf",&reference_stations[index].longitude);
                         printf("RS_%d_LONGITUDE: %f\n",index + 1, reference_stations[index].longitude);
                    } else if(strcmp("ALTITUDE", key) == 0) {
                         sscanf(value,"%lf",&reference_stations[index].altitude);
                         printf("RS_%d_ALTITUDE: %f\n",index + 1, reference_stations[index].altitude);
                    } else if(strncmp("SECTOR_MASK_", key, 12) == 0) {
                         sscanf(key,"SECTOR_MASK_%d_%s",&sector_mask_index, key);
                                        --sector_mask_index;
                         if(strcmp("MINIMUM_ELEVATION", key) == 0) {
                              printf("RS_%d_SECTOR_MASK_%d_MINIMUM_ELEVATION: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MAXIMUM_ELEVATION", key) == 0) {
                              printf("RS_%d_SECTOR_MASK_%d_MAXIMUM_ELEVATION: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MINIMUM_AZIMUTH", key) == 0) {
                              printf("RS_%d_SECTOR_MASK_%d_MINIMUM_AZIMUTH: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MAXIMUM_AZIMUTH", key) == 0) {
                              printf("RS_%d_SECTOR_MASK_%d_MAXIMUM_AZIMUTH: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else {
                              printf("ERROR: RS_%d_SECTOR_MASK: Unknown Value\n", index + 1);
                         }
                    } else {
                         printf("ERROR: RS: Unknown Value\n");
                    }
               } else if(strncmp("LM_",key,3) == 0) {
                    sscanf(key,"LM_%d_%s",&index, key);
                    --index;
                    if(strcmp("X",key) == 0){
                         limits->NUMBER_OF_LOCAL_MONITORS++;
                         sscanf(value,"%lf",&local_monitors[index].x);
                         printf("LM_%d_X: %f\n",index + 1, local_monitors[index].x);
                    } else if(strcmp("Y", key) == 0) {
                         sscanf(value,"%lf",&local_monitors[index].y);
                         printf("LM_%d_Y: %f\n",index + 1, local_monitors[index].y);
                    } else if(strcmp("Z", key) == 0) {
                         sscanf(value,"%lf",&local_monitors[index].z);
                         printf("LM_%d_Z: %f\n",index + 1, local_monitors[index].z);
                    } else if(strcmp("LATITUDE", key) == 0) {
                         sscanf(value,"%lf",&local_monitors[index].latitude);
                         printf("LM_%d_LATITUDE: %f\n",index + 1, local_monitors[index].latitude);
                    } else if(strcmp("LONGITUDE", key) == 0) {
                         sscanf(value,"%lf",&local_monitors[index].longitude);
                         printf("LM_%d_LONGITUDE: %f\n",index + 1, local_monitors[index].longitude);
                    } else if(strcmp("ALTITUDE", key) == 0) {
                         sscanf(value,"%lf",&local_monitors[index].altitude);
                         printf("LM_%d_ALTITUDE: %f\n",index + 1, local_monitors[index].altitude);
                    } else if(strncmp("SECTOR_MASK_", key, 12) == 0) {
                         sscanf(key,"SECTOR_MASK_%d_%s",&sector_mask_index, key);
                                        --sector_mask_index;
                         if(strcmp("MINIMUM_ELEVATION", key) == 0) {
                              printf("LM_%d_SECTOR_MASK_%d_MINIMUM_ELEVATION: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MAXIMUM_ELEVATION", key) == 0) {
                              printf("LM_%d_SECTOR_MASK_%d_MAXIMUM_ELEVATION: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MINIMUM_AZIMUTH", key) == 0) {
                              printf("LM_%d_SECTOR_MASK_%d_MINIMUM_AZIMUTH: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MAXIMUM_AZIMUTH", key) == 0) {
                              printf("LM_%d_SECTOR_MASK_%d_MAXIMUM_AZIMUTH: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else {
                              printf("ERROR: LM_%d_SECTOR_MASK: Unknown Value\n", index + 1);
                         }
                    } else {
                         printf("ERROR: LM: Unknown Value\n");
                    }
               } else if(strncmp("FFM_",key,3) == 0) {
                    sscanf(key,"FFM_%d_%s",&index, key);
                    --index;
                    if(strcmp("X",key) == 0){
                         printf("FFM_%d_X: %s\n",index + 1, value);
                    } else if(strcmp("Y", key) == 0) {
                         printf("FFM_%d_Y: %s\n",index + 1, value);
                    } else if(strcmp("Z", key) == 0) {
                         printf("FFM_%d_Z: %s\n",index + 1, value);
                    } else if(strcmp("LATITUDE", key) == 0) {
                         printf("FFM_%d_LATITUDE: %s\n",index + 1, value);
                    } else if(strcmp("LONGITUDE", key) == 0) {
                         printf("FFM_%d_LONGITUDE: %s\n",index + 1, value);
                    } else if(strcmp("ALTITUDE", key) == 0) {
                         printf("FFM_%d_ALTITUDE: %s\n",index + 1, value);
                    } else if(strncmp("SECTOR_MASK_", key, 12) == 0) {
                         sscanf(key,"SECTOR_MASK_%d_%s",&sector_mask_index, key);
                                        --sector_mask_index;
                         if(strcmp("MINIMUM_ELEVATION", key) == 0) {
                              printf("FFM_%d_SECTOR_MASK_%d_MINIMUM_ELEVATION: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MAXIMUM_ELEVATION", key) == 0) {
                              printf("FFM_%d_SECTOR_MASK_%d_MAXIMUM_ELEVATION: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MINIMUM_AZIMUTH", key) == 0) {
                              printf("FFM_%d_SECTOR_MASK_%d_MINIMUM_AZIMUTH: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else if(strcmp("MAXIMUM_AZIMUTH", key) == 0) {
                              printf("FFM_%d_SECTOR_MASK_%d_MAXIMUM_AZIMUTH: %s\n",
                                        index + 1, 
                                        sector_mask_index +1, 
                                        value);
                         } else {
                              printf("ERROR: FFM_%d_SECTOR_MASK: Unknown Value\n", index + 1);
                         }
                    } else {
                         printf("ERROR: FFM: Unknown Value\n");
                    }
               } else if(strcmp("TELERAD_PACKET_SYNC", key) == 0) {
                    sscanf(value,"%x",&uIntTemp);
                    telerad_message->ps[0] = uIntTemp >> 8;
                    telerad_message->ps[1] = uIntTemp;
                    printf("TELERAD_PACKET_SYNC: 0x%04X\n",telerad_message->ps[0]<<8 | telerad_message->ps[1]);
               }
               else if(strcmp("TELERAD_TDMA_SLOT", key) == 0)
               {
                    sscanf(value,"%x",&telerad_message->tlm.tdma);
                    printf("TELERAD_TDMA_SLOT: 0x%02X\n", telerad_message->tlm.tdma);
               }
               else if(strcmp("TELERAD_FREQUENCY", key) == 0)
               {
                    sscanf(value,"%lf",&doubleTemp);
                    printf("TELERAD_FREQUENCY: %f\n", doubleTemp);
               }
               else if(strcmp("TELERAD_OUTPUT_POWER", key) == 0)
               {
                    sscanf(value,"%u",&uIntTemp);
                    printf("TELERAD_OUTPUT_POWER: %d\n", uIntTemp);
               }
               else if(strcmp("LAAS_MESSAGE_BLOCK_IDENTIFIER", key) == 0)
               {
                    sscanf(value,"%x",&telerad_message->tlm.lm.mbi);
                    printf("LAAS_MESSAGE_BLOCK_IDENTIFIER: 0x%02X\n", telerad_message->tlm.lm.mbi);
               }
               else if(strcmp("LAAS_GBAS_ID", key) == 0)
               {
                    sscanf(value,"%4s",telerad_message->tlm.lm.gbas_id);
                    printf("LAAS_GBAS_ID: %s\n", telerad_message->tlm.lm.gbas_id);
               }
               else if(strcmp("LMT2_GROUND_STATION_REFERENCE_RECEIVERS", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.gsrr);
                    printf("LMT2_GROUND_STATION_REFERENCE_RECEIVERS: %d\n", telerad_message->tlm.lm.mt2.gsrr);
               }
               else if(strcmp("LMT2_GROUND_STATION_ACCURACY_DESIGNATOR", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.gsad);
                    printf("LMT2_GROUND_STATION_ACCURACY_DESIGNATOR: %d\n", telerad_message->tlm.lm.mt2.gsad);
               }
               else if(strcmp("LMT2_GROUND_STATION_CONTINUITY_INTEGRITY_DESIGNATOR", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.gcid);
                    printf("LMT2_GROUND_STATION_CONTINUITY_INTEGRITY_DESIGNATOR: %d\n", telerad_message->tlm.lm.mt2.gcid);
               }
               else if(strcmp("LMT2_LOCAL_MAGNETIC_VARIATION", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.lmv);
                    printf("LMT2_LOCAL_MAGNETIC_VARIATION: %f\n", telerad_message->tlm.lm.mt2.lmv);
               }
               else if(strcmp("LMT2_SIGMA_VERT_IONO_GRADIENT", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.svig);
                    printf("LMT2_SIGMA_VERT_IONO_GRADIENT: %f\n", telerad_message->tlm.lm.mt2.svig);
               }
               else if(strcmp("LMT2_REFRACTIVITY_INDEX", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.ri);
                    printf("LMT2_REFRACTIVITY_INDEX: %d\n", telerad_message->tlm.lm.mt2.ri);
               }
               else if(strcmp("LMT2_SCALE_HEIGHT", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.sh);
                    printf("LMT2_SCALE_HEIGHT: %d\n", telerad_message->tlm.lm.mt2.sh);
               }
               else if(strcmp("LMT2_REFRACTIVITY_UNCERTAINTY", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.ru);
                    printf("LMT2_REFRACTIVITY_UNCERTAINTY: %d\n", telerad_message->tlm.lm.mt2.ru);
               }
               else if(strcmp("LMT2_LATITUDE", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.lat);
                    printf("LMT2_LATITUDE: %f\n", telerad_message->tlm.lm.mt2.lat);
               }
               else if(strcmp("LMT2_LONGITUDE", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.lon);
                    printf("LMT2_LONGITUDE: %f\n", telerad_message->tlm.lm.mt2.lon);
               }
               else if(strcmp("LMT2_REFERENCE_POINT_HEIGHT", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.rph);
                    printf("LMT2_REFERENCE_POINT_HEIGHT: %f\n", telerad_message->tlm.lm.mt2.rph);
               }
               else if(strcmp("LMT2_ADDITIONAL_DATA_BLOCK", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.adb);
                    printf("LMT2_ADDITIONAL_DATA_BLOCK: %d\n", telerad_message->tlm.lm.mt2.adb);
               }
               else if(strcmp("LMT2_REFERENCE_STATION_DATA_SELECTOR", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.rsds);
                    printf("LMT2_REFERENCE_STATION_DATA_SELECTOR: %d\n", telerad_message->tlm.lm.mt2.rsds);
               }
               else if(strcmp("LMT2_MAXIMUM_USE_DISTANCE", key) == 0)
               {
                    sscanf(value,"%u",&telerad_message->tlm.lm.mt2.mud);
                    printf("LMT2_MAXIMUM_USE_DISTANCE: %d\n", telerad_message->tlm.lm.mt2.mud);
               }
               else if(strcmp("LMT2_KMD_E_POS_GPS", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.kmepg);
                    printf("LMT2_KMD_E_POS_GPS: %f\n", telerad_message->tlm.lm.mt2.kmepg);
               }
               else if(strcmp("LMT2_KMD_E_CAT1_GPS", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.kmecg);
                    printf("LMT2_KMD_E_CAT1_GPS: %f\n", telerad_message->tlm.lm.mt2.kmecg);
               }
               else if(strcmp("LMT2_KMD_E_POS_GLONASS", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.kmepgl);
                    printf("LMT2_KMD_E_POS_GLONASS: %f\n", telerad_message->tlm.lm.mt2.kmepgl);
               }
               else if(strcmp("LMT2_KMD_E_CAT1_GLONASS", key) == 0)
               {
                    sscanf(value,"%lf",&telerad_message->tlm.lm.mt2.kmecgl);
                    printf("LMT2_KMD_E_CAT1_GLONASS: %f\n", telerad_message->tlm.lm.mt2.kmecgl);
                    /******************************************************************************/
               }
               else if(strncmp("LMT4_FAS_",key,9) == 0)
               {
                    sscanf(key,"LMT4_FAS_%d_%s",&index, key);
                    printf("LMT4_FAS_%02d_%s: ",index, key);
                    index--;
                    if(strcmp("OPERATION_TYPE", key) == 0)
                    {
                         sscanf(value,"%u",&telerad_message->tlm.lm.mt4.fas[index].ot);
                         printf("%d\n", telerad_message->tlm.lm.mt4.fas[index].ot);
                    }
                    else if(strcmp("SBAS_SERVICE_PROVIDER", key) == 0)
                    {
                         sscanf(value,"%u",&telerad_message->tlm.lm.mt4.fas[index].ssp);
                         printf("%d\n", telerad_message->tlm.lm.mt4.fas[index].ssp);
                    }
                    else if(strcmp("AIRPORT_ID", key) == 0)
                    {
                         sscanf(value,"%4s",telerad_message->tlm.lm.mt4.fas[index].aid);
                         printf("%s\n", telerad_message->tlm.lm.mt4.fas[index].aid);
                    }
                    else if(strcmp("RUNWAY_NUMBER", key) == 0)
                    {
                         sscanf(value,"%u",&telerad_message->tlm.lm.mt4.fas[index].rn);
                         printf("%d\n", telerad_message->tlm.lm.mt4.fas[index].rn);
                    }
                    else if(strcmp("RUNWAY_LETTER", key) == 0)
                    {
                         sscanf(value,"%c",&telerad_message->tlm.lm.mt4.fas[index].rl);
                         (telerad_message->tlm.lm.mt4.fas[index].rl != (char) NULL) ?
                              printf("\"%c\"\n", telerad_message->tlm.lm.mt4.fas[index].rl) :
                              printf("\"\"\n");
                    }
                    else if(strcmp("APPROACH_PERFORMANCE_DESIGNATOR", key) == 0)
                    {
                         sscanf(value,"%u",&telerad_message->tlm.lm.mt4.fas[index].apd);
                         printf("%d\n", telerad_message->tlm.lm.mt4.fas[index].apd);
                    }
                    else if(strcmp("ROUTE_INDICATOR", key) == 0)
                    {
                         sscanf(value,"%c",&telerad_message->tlm.lm.mt4.fas[index].ri);
                         printf("\"%c\"\n", telerad_message->tlm.lm.mt4.fas[index].ri);
                    }
                    else if(strcmp("REFERENCE_PATH_DATA_SELECTOR", key) == 0)
                    {
                         sscanf(value,"%u",&telerad_message->tlm.lm.mt4.fas[index].rpds);
                         printf("%d\n", telerad_message->tlm.lm.mt4.fas[index].rpds);
                    }
                    else if(strcmp("REFERENCE_PATH_ID", key) == 0)
                    {
                         telerad_message->tlm.lm.mt4.fas_count++;
                         sscanf(value,"%4s",telerad_message->tlm.lm.mt4.fas[index].rpid);
                         printf("%s\n", telerad_message->tlm.lm.mt4.fas[index].rpid);
                    }
                    else if(strcmp("LTP_FTP_LATITUDE", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].lat);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].lat);
                    }
                    else if(strcmp("LTP_FTP_LONGITUDE", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].lon);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].lon);
                    }
                    else if(strcmp("LTP_FTP_HEIGHT", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].height);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].height);
                    }
                    else if(strcmp("DELTA_FPAP_LATITUDE", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].d_lat);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].d_lat);
                    }
                    else if(strcmp("DELTA_FPAP_LONGITUDE", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].d_lon);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].d_lon);
                    }
                    else if(strcmp("APPROACH_THRESHOLD_CROSSING_HEIGHT", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].atch);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].atch);
                    }
                    else if(strcmp("APPROACH_TCH_UNITS_SELECTOR", key) == 0)
                    {
                         sscanf(value,"%u",&telerad_message->tlm.lm.mt4.fas[index].atus);
                         printf("%d\n", telerad_message->tlm.lm.mt4.fas[index].atus);
                    }
                    else if(strcmp("GLIDE_PATH_ANGLE", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].gpa);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].gpa);
                    }
                    else if(strcmp("COURSE_WIDTH_AT_THRESHOLD", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].cwat);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].cwat);
                    }
                    else if(strcmp("DELTA_LENGTH_OFFSET", key) == 0)
                    {
                         sscanf(value,"%u",&telerad_message->tlm.lm.mt4.fas[index].dlo);
                         printf("%d\n", telerad_message->tlm.lm.mt4.fas[index].dlo);
                    }
                    else if(strcmp("VERTICAL_ALERT_LIMIT", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].fas_val);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].fas_val);
                    }
                    else if(strcmp("LATERAL_ALERT_LIMIT", key) == 0)
                    {
                         sscanf(value,"%lf",&telerad_message->tlm.lm.mt4.fas[index].fas_lal);
                         printf("%f\n", telerad_message->tlm.lm.mt4.fas[index].fas_lal);
                    } else {
                         printf("Unmapped Key Value Pair: LMT4\n");
                         printf("Key=\"%s\"\t\tValue=\"%s\"\n", key, value);
                    }


                    /***************************************************************************************************/
               } else {
                    printf("ERROR: Unknown Value\n");
               }
          }
     } 
     printf("LMT4_FAS_COUNT: %d\n", telerad_message->tlm.lm.mt4.fas_count);
     fclose(fp);
     return limits->NUMBER_OF_SERIAL_DEVICES;
}

/*
   int
   initializeLAASGroundFacility()
   {
   int ret = 0;
   int i   = 0;

   int (*StoreValues)(const char *, const char *) = NULL;

   memset(&tm, 0, sizeof(struct telerad_message));


   StoreValues = &storeLAASMessageValues;

   tm.ps[0]    = 0xFF;
   tm.ps[1]    = 0x00;
   tm.length   = 0;
   tm.checksum = 0;
   tm.tlm.ms   = 0;
   tm.tlm.rssi = 0;
   tm.ms_id    = 0x5C;


   ret = parse_key_value_pairs(LAAS_CONFIG_FILE, StoreValues);
   printf("\nparse() returned %d\n", ret);

   for(i=0;i<tm.tlm.lm.mt4.fas_count;i++)
   tm.tlm.lm.mt4.fas[i].dsl = 41;

   tm.checksum_valid = true;
   tm.tlm.lm.valid = true;

   return ret == -1 ? EXIT_FAILURE : EXIT_SUCCESS;
   }
   */



void  close_log_files( struct str_serial_device serial_devices[], 
          struct str_gg12_device gg12s[],
          struct str_gg12w_device gg12ws[], 
          struct str_bline_device blines[], 
          struct str_zxtreme_device zxtremes[])
{
     int i = 0;

     for(i = 0; i < MAX_NUMBER_OF_SERIAL_DEVICES; i++)
     {
          if(serial_devices[i].raw) {
               /* Need to check return values */
               serial_devices[i].raw = close(serial_devices[i].raw);
          }

          if(serial_devices[i].console) {
               /* Need to check return values */
               fclose(serial_devices[i].console);
               serial_devices[i].console = NULL;
          }

          if(serial_devices[i].transaction) {
               /* Need to check return values */
               fclose(serial_devices[i].transaction);
               serial_devices[i].transaction = NULL;
          }
     }

     for(i = 0; i < MAX_NUMBER_OF_GG12S; i++)
     {
          if(gg12s[i].mca.csv)
          {
               fclose(gg12s[i].mca.csv);
               gg12s[i].mca.csv = NULL;
          }

          if(gg12s[i].mis.csv)
          {
               fclose(gg12s[i].mis.csv);
               gg12s[i].mis.csv = NULL;
          }

          if(gg12s[i].pbn.csv)
          {
               fclose(gg12s[i].pbn.csv);
               gg12s[i].pbn.csv = NULL;
          }

          if(gg12s[i].sal.csv)
          {
               fclose(gg12s[i].sal.csv);
               gg12s[i].sal.csv = NULL;
          }

          if(gg12s[i].snv.csv)
          {
               fclose(gg12s[i].snv.csv);
               gg12s[i].snv.csv = NULL;
          }

          if(gg12s[i].xyz.csv)
          {
               fclose(gg12s[i].xyz.csv);
               gg12s[i].xyz.csv = NULL;
          }
     }

     for(i = 0; i < MAX_NUMBER_OF_GG12WS; i++)
     {
          if(gg12ws[i].mca.csv)
          {
               fclose(gg12ws[i].mca.csv);
               gg12ws[i].mca.csv = NULL;
          }

          if(gg12ws[i].mis.csv)
          {
               fclose(gg12ws[i].mis.csv);
               gg12ws[i].mis.csv = NULL;
          }

          if(gg12ws[i].pbn.csv)
          {
               fclose(gg12ws[i].pbn.csv);
               gg12ws[i].pbn.csv = NULL;
          }

          if(gg12ws[i].sal.csv)
          {
               fclose(gg12ws[i].sal.csv);
               gg12ws[i].sal.csv = NULL;
          }

          if(gg12ws[i].snv.csv)
          {
               fclose(gg12ws[i].snv.csv);
               gg12ws[i].snv.csv = NULL;
          }

          if(gg12ws[i].xyz.csv)
          {
               fclose(gg12ws[i].xyz.csv);
               gg12ws[i].xyz.csv = NULL;
          }
     }
     for(i = 0; i < MAX_NUMBER_OF_ZXTREMES; i++)
     {
          if(zxtremes[i].mpc.csv)
          {
               fclose(zxtremes[i].mpc.csv);
               zxtremes[i].mpc.csv = NULL;
          }

          if(zxtremes[i].pbn.csv)
          {
               fclose(zxtremes[i].pbn.csv);
               zxtremes[i].pbn.csv = NULL;
          }

          if(zxtremes[i].sal.csv)
          {
               fclose(zxtremes[i].sal.csv);
               zxtremes[i].sal.csv = NULL;
          }

          if(zxtremes[i].snv.csv)
          {
               fclose(zxtremes[i].snv.csv);
               zxtremes[i].snv.csv = NULL;
          }

          if(zxtremes[i].xyz.csv)
          {
               fclose(zxtremes[i].xyz.csv);
               zxtremes[i].xyz.csv = NULL;
          }
     }
}

int open_data_sources(struct str_limits *limits, 
          struct str_laas_logging_control *logging_control, 
          struct str_serial_device serial_devices[])
{
     int i = 0;
     for(i = 0; i < limits->NUMBER_OF_SERIAL_DEVICES; i++)
     {
          if(logging_control->data_source == DATA_FILE)
          {
               /* Open raw data file */
               serial_devices[i].fid = open(serial_devices[i].tty, O_RDONLY);
          } else {
               /* Open serial port */
               serial_devices[i].fid = open(serial_devices[i].tty, O_RDWR | O_NOCTTY | O_ASYNC);
          }

          /* Check to see if the port was opened */
          if ( serial_devices[i].fid < 0) {
               perror(serial_devices[i].tty);
          } else {
               printf("Opened device %s with fid %d.\n", serial_devices[i].tty, serial_devices[i].fid);
               /*fcntl(serial_devices[i].fid, F_SETFL, FNDELAY); */
               if(logging_control->data_source == SERIAL)
               {
                    fcntl(serial_devices[i].fid, F_SETFL, O_NDELAY);
                    if (init_serial_settings9600(serial_devices[i].fid) != 0)
                    {
                         printf("init_serial_settings() failed on device %s\n",serial_devices[i].tty);
                         return -1;
                    }

                    if (serial_set_speed(serial_devices[i].fid, serial_devices[i].baud_rate) != 0)
                    {
                         printf("serial_set_speed(%s, %d) failed\n",serial_devices[i].tty, serial_devices[i].baud_rate);
                         return -1;
                    } else {
                         printf("serial_set_speed(%s, %d) successfull\n",serial_devices[i].tty, serial_devices[i].baud_rate);
                    }
               }
          }
     }
     return 0;
}

/*
   t = date_gps_to_unix(gg12,);
   */

int open_log_files(
          struct str_time *master_time, 
          struct str_limits *limits, 
          struct str_laas_logging_control *logging_control, 
          struct str_serial_device serial_devices[], 
          struct str_gg12_device gg12s[],
          struct str_gg12w_device gg12ws[], 
          struct str_bline_device blines[], 
          struct str_zxtreme_device zxtremes[],
          FILE   **xyzae,
          FILE   **alm_csv,
          FILE   **eph_csv,
          FILE   **laas_message_type1_csv)
{
     int i = 0;
     char date[16];
     char current_time[MAX_LINE_LENGTH];
     struct timeval tv_current_time;
     struct tm *tm_time = NULL;
     char file_name[MAX_LINE_LENGTH];
     int type_index = 0;

     /*
        time_t t = 0;
        */

     memset(date, 0, 16);
     memset(current_time, 0, MAX_LINE_LENGTH);
     memset(file_name, 0, MAX_LINE_LENGTH);
     memset(&tv_current_time, 0, sizeof(struct timeval));

     /* Change to the /logs directory */
     if((chdir(logging_control->log_directory)) != 0)
     {
          printf("LOG_DIRECTORY: %s: ", logging_control->log_directory);
          fflush(stdout); /* makes the printf statement print before perror */
          perror("");
          exit(0);
     } else {
          printf("cd %s: successful\n", logging_control->log_directory);
     }

     /* Get the current date */
     gettimeofday(&tv_current_time, NULL);
     /* Add a few minutes on to the time to get past leap seconds and UTC offset */
     tv_current_time.tv_sec += 300;
     tm_time = gmtime(&tv_current_time.tv_sec);
     sprintf(date,"%d%02d%02d",tm_time->tm_year+1900,tm_time->tm_mon+1,tm_time->tm_mday);

/*
     time_t c_time = 0;
     c_time = date_gps_to_unix(master_time->week, master_time->rcvtime);
     c_time += 300; *//* Add a few minutes on to the time to get past leap seconds and UTC offset */
/*
     tm_time = gmtime(&c_time);
     sprintf(date,"%d%02d%02d",tm_time->tm_year+1900,tm_time->tm_mon+1,tm_time->tm_mday);
*/
     printf("DATE: %s",date);

     /* Create the directory if it does not exist */
     if(mkdir(date, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
     {
          if(!((errno == 0) || (errno == EEXIST)))
          {
               printf("mkdir %s/%s: ", logging_control->log_directory, date);
               fflush(stdout); /* makes the printf statement print before perror */
               perror("");
               exit(0);
          } else {
               printf("%s/%s: Already Exist\n", logging_control->log_directory, date);
          }
     } else {
          printf("mkdir %s/%s: successful\n", logging_control->log_directory, date);
     }

     /* Change to the date directory */
     if((chdir(date)) != 0)
     {
          printf("LOG_DIRECTORY: %s: ", date);
          fflush(stdout); /* makes the printf statement print before perror */
          perror("");
          exit(0);
     } else {
          printf("cd %s: successful\n", date);
     }

     printf("LIMITS: NUMBER_OF_SERIAL_DEVICES: %d\n", limits->NUMBER_OF_SERIAL_DEVICES);

     for(i = 0; i < limits->NUMBER_OF_SERIAL_DEVICES; i++)
     {

          printf("NAME: %s\n", serial_devices[i].name);
          printf("TYPE_INDEX: %d\n", serial_devices[i].type_index);
          /******************************************************************************************/
          if(logging_control->serial[i].logging_console == true)
          {
               sprintf(file_name, "%s-%s-%s-%s.console", date, logging_control->location_identification, 
                         serial_devices[i].type_semantic, serial_devices[i].name);

               if(serial_devices[i].console)
               {
                    fclose(serial_devices[i].console);
                    serial_devices[i].console = NULL;
                    printf("DONE: Close Console: %d\n", i);
               }
               if ((serial_devices[i].console = fopen(file_name, "a")) != NULL) {
                    printf("Created file %s for logging\n", file_name);
                    fprintf(serial_devices[i].console,"Logging started at %s.\n", current_time_as_string(current_time));
                    fflush(serial_devices[i].console);
               } else {
                    perror("ERROR:DEBUG:Failed to open file for writing");
               }
          }
          /******************************************************************************************/
          if(logging_control->serial[i].transaction_log == true)
          {
               sprintf(file_name, "%s-%s-%s-%s.transaction", date, logging_control->location_identification, 
                         serial_devices[i].type_semantic, serial_devices[i].name);

               if(serial_devices[i].transaction)
               {
                    fclose(serial_devices[i].transaction);
                    serial_devices[i].transaction = NULL;
                    printf("DONE: Close transaction: %d\n", i);
               }
               if ((serial_devices[i].transaction = fopen(file_name, "a")) != NULL) {
                    printf("Created file %s for logging\n", file_name);
                    fprintf(serial_devices[i].console,"Logging started at %s.\n", current_time_as_string(current_time));
                    fflush(serial_devices[i].console);
               } else {
                    perror("ERROR:DEBUG:Failed to open file for writing");
               }
          }
          /******************************************************************************************/

          if(logging_control->serial[i].logging_raw == true)
          {
               sprintf(file_name, "%s-%s-%s-%s.raw", date, logging_control->location_identification, 
                         serial_devices[i].type_semantic, serial_devices[i].name);
               if(serial_devices[i].raw)
               {
                    serial_devices[i].raw = close(serial_devices[i].raw);
               }
               if ((serial_devices[i].raw = open(file_name, O_WRONLY | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR )) > 0) {
                    printf("Created file %s for logging\n", file_name);
               } else {
                    perror("ERROR:RAW:Failed to open for writing");
               }
          }

          type_index = serial_devices[i].type_index;

          if(serial_devices[i].type == GG12)
          {
               if(logging_control->serial[i].parsing == true)
               {
                    if(gg12s[type_index].mca.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.mca.csv", date, logging_control->location_identification, 
                                   serial_devices[i].type_semantic, serial_devices[i].name);

                         if ((gg12s[type_index].mca.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(serial_devices[i].console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(serial_devices[i].console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(gg12s[type_index].mis.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.mis.csv", date, logging_control->location_identification, 
                                   serial_devices[i].type_semantic, serial_devices[i].name);
                         if ((gg12s[type_index].mis.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(serial_devices[i].console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(serial_devices[i].console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(gg12s[type_index].pbn.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.pbn.csv", date, logging_control->location_identification, 
                                   serial_devices[i].type_semantic, serial_devices[i].name);
                         if ((gg12s[type_index].pbn.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(serial_devices[i].console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(serial_devices[i].console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(gg12s[type_index].sal.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.sal.csv", date, logging_control->location_identification, 
                                   serial_devices[i].type_semantic, serial_devices[i].name);
                         if ((gg12s[type_index].sal.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(serial_devices[i].console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(serial_devices[i].console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(gg12s[type_index].snv.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.snv.csv", date, logging_control->location_identification, 
                                   serial_devices[i].type_semantic, serial_devices[i].name);
                         if ((gg12s[type_index].snv.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(serial_devices[i].console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(serial_devices[i].console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }
                    if(gg12s[type_index].xyz.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.xyz.csv", date, logging_control->location_identification, 
                                   serial_devices[i].type_semantic, serial_devices[i].name);
                         if ((gg12s[type_index].xyz.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(serial_devices[i].console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(serial_devices[i].console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }
               }
          } else if(serial_devices[i].type == GG12W) {
               if(logging_control->serial[i].parsing == true)
               {
                    if(gg12ws[type_index].mca.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.mca.csv", date, logging_control->location_identification, 
                                   gg12ws[type_index].serial_device->type_semantic, gg12ws[type_index].serial_device->name);

                         if ((gg12ws[type_index].mca.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(gg12ws[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(gg12ws[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(gg12ws[type_index].mis.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.mis.csv", date, logging_control->location_identification, 
                                   gg12ws[type_index].serial_device->type_semantic, gg12ws[type_index].serial_device->name);
                         if ((gg12ws[type_index].mis.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(gg12ws[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(gg12ws[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(gg12ws[type_index].pbn.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.pbn.csv", date, logging_control->location_identification, 
                                   gg12ws[type_index].serial_device->type_semantic, gg12ws[type_index].serial_device->name);
                         if ((gg12ws[type_index].pbn.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(gg12ws[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(gg12ws[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(gg12ws[type_index].sal.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.sal.csv", date, logging_control->location_identification, 
                                   gg12ws[type_index].serial_device->type_semantic, gg12ws[type_index].serial_device->name);
                         if ((gg12ws[type_index].sal.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(gg12ws[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(gg12ws[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(gg12ws[type_index].snv.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.snv.csv", date, logging_control->location_identification, 
                                   gg12ws[type_index].serial_device->type_semantic, gg12ws[type_index].serial_device->name);
                         if ((gg12ws[type_index].snv.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(gg12ws[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(gg12ws[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }
                    if(gg12ws[type_index].xyz.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.xyz.csv", date, logging_control->location_identification, 
                                   gg12ws[type_index].serial_device->type_semantic, gg12ws[type_index].serial_device->name);
                         if ((gg12ws[type_index].xyz.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(gg12ws[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(gg12ws[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }
               }
          } else if(serial_devices[i].type == ZXTREME) {
               if(logging_control->serial[i].parsing == true)
               {
                    if(zxtremes[type_index].mpc.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.mpc.csv", date, logging_control->location_identification, 
                                   zxtremes[type_index].serial_device->type_semantic, zxtremes[type_index].serial_device->name);

                         if ((zxtremes[type_index].mpc.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(zxtremes[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(zxtremes[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(zxtremes[type_index].pbn.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.pbn.csv", date, logging_control->location_identification, 
                                   zxtremes[type_index].serial_device->type_semantic, zxtremes[type_index].serial_device->name);
                         if ((zxtremes[type_index].pbn.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(zxtremes[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(zxtremes[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(zxtremes[type_index].sal.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.sal.csv", date, logging_control->location_identification, 
                                   zxtremes[type_index].serial_device->type_semantic, zxtremes[type_index].serial_device->name);
                         if ((zxtremes[type_index].sal.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(zxtremes[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(zxtremes[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }

                    if(zxtremes[type_index].snv.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.snv.csv", date, logging_control->location_identification, 
                                   zxtremes[type_index].serial_device->type_semantic, zxtremes[type_index].serial_device->name);
                         if ((zxtremes[type_index].snv.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(zxtremes[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(zxtremes[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }
                    if(zxtremes[type_index].xyz.logging_parsed == true)
                    {
                         sprintf(file_name, "%s-%s-%s-%s.xyz.csv", date, logging_control->location_identification, 
                                   zxtremes[type_index].serial_device->type_semantic, zxtremes[type_index].serial_device->name);
                         if ((zxtremes[type_index].xyz.csv = fopen(file_name, "a")) != NULL) {
                              printf("Created file %s for logging\n", file_name);
                              fprintf(zxtremes[type_index].serial_device->console,"Logging started at %s.\n", current_time_as_string(current_time));
                              fflush(zxtremes[type_index].serial_device->console);
                         } else {
                              perror("ERROR:DEBUG:Failed to open file for writing");
                         }
                    }
               }
          }
     }

     sprintf(file_name, "%s-%s.xyzae", date, logging_control->location_identification);
     if((*xyzae))
     {
          fclose((*xyzae));
          (*xyzae) = NULL;
          printf("DONE: Close Console: %d\n", i);
     }
     if (((*xyzae) = fopen(file_name, "a")) != NULL) {
          printf("Created file %s for logging\n", file_name);
          fprintf(serial_devices[0].console,"Logging started at %s.\n", current_time_as_string(current_time));
          fflush(serial_devices[0].console);
     } else {
          perror("ERROR:DEBUG:Failed to open file for writing");
     }

     sprintf(file_name, "%s-%s.alm.csv", date, logging_control->location_identification);
     if((*alm_csv))
     {
          fclose((*alm_csv));
          (*alm_csv) = NULL;
          printf("DONE: Close Console: %d\n", i);
     }
     if (((*alm_csv) = fopen(file_name, "a")) != NULL) {
          printf("Created file %s for logging\n", file_name);
          fprintf(serial_devices[0].console,"Logging started at %s.\n", current_time_as_string(current_time));
          fflush(serial_devices[0].console);
          fprintf((*alm_csv),"rcvtime, prn, current, 6, 12, ephemeris_update_successful\n");
          fflush((*alm_csv));
     } else {
          perror("ERROR:DEBUG:Failed to open file for writing");
     }

     sprintf(file_name, "%s-%s.eph.csv", date, logging_control->location_identification);
     if((*eph_csv))
     {
          fclose((*eph_csv));
          (*eph_csv) = NULL;
          printf("DONE: Close Console: %d\n", i);
     }
     if (((*eph_csv) = fopen(file_name, "a")) != NULL) {
          printf("Created file %s for logging\n", file_name);
          fprintf(serial_devices[0].console,"Logging started at %s.\n", current_time_as_string(current_time));
          fflush(serial_devices[0].console);
     } else {
          perror("ERROR:DEBUG:Failed to open file for writing");
     }

     return 0;
}

/* Need to fix the buffers */
/* Try setting the read pointer back instead of using multiple buffers */

void set_time_mark(
          struct str_time *master_time, 
          struct str_gg12_device gg12s[], 
          struct str_limits *limits)
{
     unsigned char buffer[MAX_BUFFER_SIZE];
     int i = 0;

     int size = 0;

     int  end[MAX_NUMBER_OF_GG12S];

     bool rcvtime_found = false;
     bool week_found = false;

     unsigned char *header = NULL;

     memset(buffer,    0, sizeof(char) * MAX_BUFFER_SIZE);
     memset(end,       0, sizeof(int)  * MAX_NUMBER_OF_GG12S);

     for(i=0; i < limits->NUMBER_OF_GG12S; i++)
     {
          rcvtime_found = false;
          week_found = false;
          while((rcvtime_found == false) || (week_found == false))
          {
               size = read(gg12s[i].serial_device->fid, &buffer[end[i]], MAX_BUFFER_SIZE - end[i]);

               if(size == -1)
               {
                    rcvtime_found = true;
                    week_found = true;
               }

               if(!rcvtime_found)
               {
                    header = (unsigned char *) find_string((void *) buffer, size, "$PASHR,PBN,", PASH_HEADER_TOTAL);
                    if(header)
                    {
                         if (ashtech_checksum_16(&header[PASH_HEADER_TOTAL], PBN_PACKET_SIZE-PASH_HEADER_TOTAL, NULL) >= 0)
                         {
                              gg12s[i].serial_device->time_received.tm_gps.rcvtime = raw_to_uint(&header[PASH_HEADER_TOTAL]);
                              rcvtime_found = true;
                         } else {
                              printf("PBN Checksum\n");
                         }
                    } else {
                         header = (unsigned char *) find_string((void *) buffer, size, "$PASHR,MIS,", PASH_HEADER_TOTAL);
                         if(header)
                         {
                              if (ashtech_checksum_16(&header[PASH_HEADER_TOTAL], MIS_PACKET_SIZE-PASH_HEADER_TOTAL, NULL) >= 0)
                              {
                                   gg12s[i].serial_device->time_received.tm_gps.rcvtime = raw_to_uint(&header[PASH_HEADER_TOTAL]);
                                   rcvtime_found = true;
                              } else {
                                   printf("MIS Checksum\n");
                              }
                         } else {
                              printf("%s: No PBN or MIS header found to set time mark\n", gg12s[i].serial_device->name);
                         }
                    }
               }

               if(!week_found)
               {
                    header = (unsigned char *) find_string((void *) buffer, size, "$PASHR,SNV,", PASH_HEADER_TOTAL);
                    if(header)
                    {
                         if (ashtech_checksum_16(&header[PASH_HEADER_TOTAL], SNV_PACKET_SIZE-PASH_HEADER_TOTAL, NULL) >= 0)
                         {
                              gg12s[i].serial_device->time_received.tm_gps.week = raw_to_short(&header[PASH_HEADER_TOTAL]);
                              printf("SNV header found: %d\n", gg12s[i].serial_device->time_received.tm_gps.week);
                              week_found = true;
                         } else {
                              printf("SNV Checksum\n");
                         }
                    } else {
                         printf("No SNV header found to set time mark\n");
                    }
               }
               memmove(buffer, &buffer[MAX_BUFFER_SIZE - 10], 10);
               end[i] = 10;
          }
     }

     master_time->tm_gps.week    = INT_MAX;
     master_time->tm_gps.rcvtime = UINT_MAX;

     for(i=0; i < limits->NUMBER_OF_GG12S; i++)
     {
          /* Find the minimum starting point */
          master_time->tm_gps.week    = (gg12s[i].serial_device->time_received.tm_gps.week    < master_time->tm_gps.week)    ? gg12s[i].serial_device->time_received.tm_gps.week    : master_time->tm_gps.week;
          master_time->tm_gps.rcvtime = (gg12s[i].serial_device->time_received.tm_gps.rcvtime < master_time->tm_gps.rcvtime) ? gg12s[i].serial_device->time_received.tm_gps.rcvtime : master_time->tm_gps.rcvtime;
          /* Reset the file pointer to begining of file */
          lseek(gg12s[i].serial_device->fid, 0, SEEK_SET);
     }

     /** Sync the unix time to the GPS time */
     time_gps_to_unix(master_time);
     printf("Master Time: Week: %d, Time: %d\n", master_time->tm_gps.week, master_time->tm_gps.rcvtime);
}


void calculate_centriod_location(struct str_location reference_stations[],
          struct str_limits   *limits,
          struct str_location *centroid_location)
{
     int i = 0;
     memset(centroid_location, 0x00, sizeof(struct str_location));
     for(i=0;i<limits->NUMBER_OF_REFERENCE_STATIONS;i++)
     {
          printf("Reference Station: X:%f, Y:%f, Z:%f\n", reference_stations[i].x, reference_stations[i].y, reference_stations[i].z);
          printf("Reference Station: Latitude:%f, Longitude:%f, Altitude:%f\n", reference_stations[i].latitude, reference_stations[i].longitude, reference_stations[i].altitude);
          centroid_location->x += reference_stations[i].x;
          centroid_location->y += reference_stations[i].y;
          centroid_location->z += reference_stations[i].z;
          centroid_location->latitude  += reference_stations[i].latitude;
          centroid_location->longitude += reference_stations[i].longitude;
          centroid_location->altitude  += reference_stations[i].altitude;
     }
     centroid_location->x = centroid_location->x / limits->NUMBER_OF_REFERENCE_STATIONS;
     centroid_location->y = centroid_location->y / limits->NUMBER_OF_REFERENCE_STATIONS;
     centroid_location->z = centroid_location->z / limits->NUMBER_OF_REFERENCE_STATIONS;
     centroid_location->latitude  = centroid_location->latitude  / limits->NUMBER_OF_REFERENCE_STATIONS;
     centroid_location->longitude = centroid_location->longitude / limits->NUMBER_OF_REFERENCE_STATIONS;
     centroid_location->altitude  = centroid_location->altitude  / limits->NUMBER_OF_REFERENCE_STATIONS;
     printf("Centrod Location: X:%f, Y:%f, Z:%f\n", centroid_location->x, centroid_location->y, centroid_location->z);
     printf("Centrod Location: Latitude:%f, Longitude:%f, Altitude:%f\n", centroid_location->latitude, centroid_location->longitude, centroid_location->altitude);
}

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
          FILE **laas_message_type1_csv)
{

     int i = 0;

     /* Parse the configuration file and return the number of serial devices */
     /* NUMBER_OF_SERIAL_DEVICES = max_fd */
     printf("Parse Configuration File\n");
     parseLAASConfigurationFile(configuration_file, 
               limits,
               logging_control, 
               telerad_message, 
               serial_devices,
               gg12s,
               gg12ws,
               blines,
               zxtremes,
               telerads,
               ffms,
               reference_stations,
               local_monitors);
     printf("DONE: Parse Configuration File\n");

     calculate_centriod_location(reference_stations, limits, centroid_location);

     date_init();

     if(logging_control->data_sources_configured == false)
     {
          logging_control->data_sources_configured = true;
          open_data_sources(limits, logging_control, serial_devices);
          if(logging_control->data_source == DATA_FILE)
          {
               printf("Setting Master Time From Data Files\n");
               set_time_mark(master_time, gg12s, limits);
          } else {
               /* Set GPS to the current unix time */
               date_unix_to_gps(&master_time->tm_gps);

               /* Backup 10 Seconds */
               master_time->tm_gps.rcvtime -= 10000;

               /* Truncate */
               master_time->tm_gps.rcvtime = ((master_time->tm_gps.rcvtime / 1000) * 1000);
               printf("Set Master Time to Unix Time: Week: %d, Time: %d\n", master_time->tm_gps.week, master_time->tm_gps.rcvtime);
               /** Sync the gps time to unix */
               time_gps_to_unix(master_time);
               
               for(i = 0; i < limits->NUMBER_OF_SERIAL_DEVICES; i++)
               {
                    memcpy(&serial_devices[i].time_received, master_time, sizeof(struct str_time));
                    /*
                    serial_devices[i].time_received.tm_gps.week    = master_time->tm_gps.week;
                    serial_devices[i].time_received.tm_gps.rcvtime = master_time->tm_gps.rcvtime;
                    */
               }
          }
     }

     close_log_files(serial_devices, gg12s, gg12ws, blines, zxtremes);
     open_log_files(master_time, limits, logging_control, serial_devices, gg12s, gg12ws, blines, zxtremes, xyzae, alm_csv, eph_csv, laas_message_type1_csv);

     /*
        open_log_files(limits, logging_control, serial_devices, gg12s, telerads, ffms);
        */

     printf("NUMBER_OF_REFERENCE_SATATIONS:%02d MAX: %02d\n", limits->NUMBER_OF_REFERENCE_STATIONS, MAX_NUMBER_OF_REFERENCE_STATIONS);
     printf("NUMBER_OF_LOCAL_MONITORS:     %02d MAX: %02d\n", limits->NUMBER_OF_LOCAL_MONITORS, MAX_NUMBER_OF_LOCAL_MONITORS);
     printf("NUMBER_OF_GG12S:              %02d MAX: %02d\n", limits->NUMBER_OF_GG12S, MAX_NUMBER_OF_GG12S);
     printf("NUMBER_OF_GG12WS:             %02d MAX: %02d\n", limits->NUMBER_OF_GG12WS, MAX_NUMBER_OF_GG12WS);
     printf("NUMBER_OF_ZXTREMES:           %02d MAX: %02d\n", limits->NUMBER_OF_ZXTREMES, MAX_NUMBER_OF_ZXTREMES);
     printf("NUMBER_OF_BLINES:             %02d MAX: %02d\n", limits->NUMBER_OF_BLINES, MAX_NUMBER_OF_BLINES);
     printf("NUMBER_OF_GPS:                %02d MAX: %02d\n", limits->NUMBER_OF_GPS, MAX_NUMBER_OF_GPS);
     printf("NUMBER_OF_TELERADS:           %02d MAX: %02d\n", limits->NUMBER_OF_TELERADS, MAX_NUMBER_OF_TELERADS);
     printf("NUMBER_OF_FAR_FIELD_MONITORS: %02d MAX: %02d\n", limits->NUMBER_OF_FAR_FIELD_MONITORS, MAX_NUMBER_OF_FAR_FIELD_MONITORS);
     printf("NUMBER_OF_SERIAL_DEVICES:     %02d MAX: %02d\n", limits->NUMBER_OF_SERIAL_DEVICES, MAX_NUMBER_OF_SERIAL_DEVICES);

     printf("Master Time: Week: %d, Time: %d\n", master_time->tm_gps.week, master_time->tm_gps.rcvtime);

     return serial_devices[limits->NUMBER_OF_SERIAL_DEVICES-1].fid;
}
