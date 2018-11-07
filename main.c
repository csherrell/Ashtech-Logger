/** \mainpage 
     Be sure to set up the hosts file correctly to follow external links.<br>
     Linux   /etc/hosts<br>
     Windows C:\\system32\\drivers\\etc\\hosts<br>

     129.15.88.34   wiki
*/
#define _ISOC9X_SOURCE  1
#define _ISOC99_SOURCE  1
#include  <math.h>

#define __USE_POSIX 1
#define __EXTENSIONS__ 1
#include <signal.h>               /* Signal handling */

/* Needed by mkdir() */
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

/* */
#include <sys/time.h>
#include <sys/select.h>

/* */
#include <stdio.h>                /* Standard input/output definitions */
#include <stdlib.h>
#include <string.h>               /* String function definitions */
#include <unistd.h>               /* UNIX standard function definitions */
#include <fcntl.h>                /* File control definitions */
#include <termios.h>              /* POSIX terminal control definitions */
#include <time.h>    
#include <stdbool.h>    
#include <gdbm.h>

/* #include <errno.h> */          /* Error number definitions */ 
#include "LAASInit.h"
#include "serial.h"
#include "telerad/telerad.h"
#include "time_struct.h"
#include "time_conversions.h"
#include "gps.h"
#include "satellite_position.h"
#include "azimuth_elevation.h"
#include "snv_update.h"

#include "lens/xyzae.h"
#include "binary_search/binary_search.h"
#include "gdbm/gdbm_dictionary.h"
#include "lens/laas_calculation_sequence.h"

/*
   unsigned int gps_day = {86400000, 172800000, 259200000, 345600000, 432000000, 518400000, 604800000};
   */

void print_buffer_hex(unsigned char buf[],int size)
{
     int i = 0;
     for(i=0;i < size;i++)
     {
          printf("%02X",buf[i]);
     }
}

void print_buffer_ascii(unsigned char buf[],int size)
{
     int i = 0;
     for(i=0;i < size;i++)
     {
          printf("%c",buf[i]);
     }
}

void exit(int signo)
{
     printf("Exiting Application: %d\n", signo);     
     _exit(1);
}

unsigned int get_date()
{
     struct timeval tv_time;
     struct tm *tm_time = NULL;
     char date[16];
     memset(&tv_time, 0x00, sizeof(struct timeval));
     memset(date, 0x00, 16);
     /* check date to see if log files need rolled */
     gettimeofday(&tv_time, NULL);
     tm_time = localtime(&tv_time.tv_sec);

     sprintf(date,"%d%02d%02d",tm_time->tm_year+1900,tm_time->tm_mon+1,tm_time->tm_mday);
     /*
        sprintf(date,"%d%02d%02d%02d",tm_time->tm_year+1900,tm_time->tm_mon+1,tm_time->tm_mday,
        tm_time->tm_min);
        */

     return atoi(date);
}

bool check_date_old(unsigned int *old_date)
{
     struct timeval tv_time;
     struct tm *tm_time = NULL;
     char date[16];
     unsigned int new_date = 0;
     memset(&tv_time, 0x00, sizeof(struct timeval));
     memset(date, 0x00, 16);
     /* check date to see if log files need rolled */
     gettimeofday(&tv_time, NULL);
     tm_time = localtime(&tv_time.tv_sec);

     sprintf(date,"%d%02d%02d",tm_time->tm_year+1900,tm_time->tm_mon+1,tm_time->tm_mday);
     /*
        sprintf(date,"%d%02d%02d%02d",tm_time->tm_year+1900,tm_time->tm_mon+1,tm_time->tm_mday, tm_time->tm_min);
        */

     new_date = atoi(date);

     if (new_date > (*old_date))
     {
          (*old_date) = new_date;
          printf("Time to roll the log files\n");
          return true;
     }
     return false;
}

bool check_date(unsigned int *old_date)
{
     struct timeval tv_time;
     struct tm *tm_time = NULL;
     char date[16];
     unsigned int new_date = 0;
     memset(&tv_time, 0x00, sizeof(struct timeval));
     memset(date, 0x00, 16);
     /* check date to see if log files need rolled */
     gettimeofday(&tv_time, NULL);
     tm_time = localtime(&tv_time.tv_sec);

     sprintf(date,"%d%02d%02d",tm_time->tm_year+1900,tm_time->tm_mon+1,tm_time->tm_mday);
     /*
        sprintf(date,"%d%02d%02d%02d",tm_time->tm_year+1900,tm_time->tm_mon+1,tm_time->tm_mday, tm_time->tm_min);
        */

     new_date = atoi(date);

     if (new_date > (*old_date))
     {
          (*old_date) = new_date;
          printf("Time to roll the log files\n");
          return true;
     }
     return false;
}

void correlate_time_serial(
          struct str_time_gps                *master_time_current,
          struct str_time_gps                *master_time_next,
          struct str_limits                  *limits,
          /* struct str_laas_logging_control    *logging_control,*/
          /* struct str_serial_device            serial_devices[],*/
          struct str_gg12_device              gg12s[],
          /* struct str_gg12w_device             gg12ws[],*/
          /* struct str_bline_device             blines[],*/
          /* struct str_zxtreme_device           zxtremes[],*/
          /* unsigned int                        gg12_done,*/
          /* unsigned int                        gg12_process_mask,*/
          bool                               *clocktick)
{
     int time_mismatch = 0;
     int i = 0;
     int e_i = 0; /**< \brief Excluded Current */
     int e_j = 0; /**< \brief Excluded Next    */
     int g_i = 0; /**< \brief GPS Current      */
     int g_j = 0; /**< \brief GPS Next         */
     unsigned int delta_t = 0;

     static bool intialized = 0;
     static int excluded_gps_count = 0;
     static int excluded_gps[MAX_NUMBER_OF_GPS];
     static int good_gps = 0;
     static struct timeval start_time;
     static struct timeval finish_time;
     static unsigned int max_delta_t = 600000;
     static struct str_time_gps previous_master_time;

     unsigned int current_time = gg12s[good_gps].serial_device->time_received.tm_gps.rcvtime;

     /* printf("correlate_time_serial:start\n"); */

     if(!intialized)
     {
          intialized = true;
          gettimeofday(&start_time,NULL);
          gettimeofday(&finish_time,NULL);
          previous_master_time.week    = master_time_next->week;
          previous_master_time.rcvtime = master_time_next->rcvtime;
     }

     (*clocktick) = false;

     if(excluded_gps_count != 0)
     {
          g_j = g_i + 1;
          while(g_i < limits->NUMBER_OF_GG12S)
          {
               if(g_i != excluded_gps[e_i])
               {
                    if(g_j != excluded_gps[e_j])
                    {
                         if(gg12s[g_i].serial_device->time_received.tm_gps.rcvtime != gg12s[g_j].serial_device->time_received.tm_gps.rcvtime)
                         {
                              time_mismatch =  1;
                         }
                         g_i++;
                         g_j = (g_j + 1) % limits->NUMBER_OF_GG12S; 
                    } else { 
                         g_j = (g_j + 1) % limits->NUMBER_OF_GG12S; 
                         e_j = (e_j + 1) % excluded_gps_count;
                    }
               } else { 
                    /* printf("Excluding: %s\n", gg12s[g_i].serial_device->name); */
                    g_i++; 
                    e_i = (e_i + 1) % excluded_gps_count;
                    if(g_i == g_j) g_j++;
               }
          }
     } else {
          /** Simple solution: check all the receiver's time with the next receiver in a circular fashion.
              If there are 4 receivers then 1 and 2 are checked, followed by 2 and 3, 3 and 4, then 4 and 1. */
          for(i = 0; i < limits->NUMBER_OF_GG12S; i++)
          {
               if(gg12s[i].serial_device->time_received.tm_gps.rcvtime != gg12s[(i+1) % limits->NUMBER_OF_GG12S].serial_device->time_received.tm_gps.rcvtime)
               {
                    time_mismatch =  1;
               }
          }
     }

     if(!time_mismatch)
     {
          /** If all the receivers have the same time then the master time is set to the rcvtime plus 500ms */
          master_time_next->rcvtime = (current_time + 500) % 604800500;

          if(master_time_next->rcvtime == 0)
          {
               master_time_next->rcvtime = 500;
          }

          gettimeofday(&finish_time,NULL);

          if(master_time_next->rcvtime != previous_master_time.rcvtime)
          {
               (*clocktick) = true;

               delta_t = (((finish_time.tv_sec - start_time.tv_sec) * 1000000) + (finish_time.tv_usec - start_time.tv_usec));
               if(delta_t > max_delta_t)
               {
                    max_delta_t = delta_t;
               }

               excluded_gps_count = 0;

               /*
                  printf("Max Delta: %d, Delta: %d\n", max_delta_t, delta_t);
                  printf("Setting Master Timer: %d, %d\n", previous_master_time.rcvtime, gg12s[0].serial_device->time_received.tm_gps.rcvtime);
                  */
               if(master_time_next->rcvtime == 500)
               {
                    master_time_next->week++;
                    previous_master_time.rcvtime = 0;
               } else {
                    previous_master_time.rcvtime = master_time_next->rcvtime;
               }
               gettimeofday(&start_time,NULL);
          }
     }  else {
          /* There was a time mismatch, but this could be due to the data still being 
             accumulated into the serial buffer */

          /* Get the current time */
          gettimeofday(&finish_time,NULL);
          delta_t = (((finish_time.tv_sec - start_time.tv_sec) * 1000000) + (finish_time.tv_usec - start_time.tv_usec));
          /* See if the current time minus the time of the last clocktic is greater than the timeout delay. */
          /* If is is less than the timeout period then do nothing.  Wait for more data */
          if(delta_t > max_delta_t)
          {
               /* If the timeout period, then find what receiver has the problem */
               for(i = 0; i < limits->NUMBER_OF_GG12S; i++)
               {
                    /* Which receiver is not caught up with the master_time */
                    if(gg12s[i].serial_device->time_received.tm_gps.rcvtime < master_time_next->rcvtime)
                    {
                         /* Exclude receiver from timming routine */
                         printf("Trouble on receiver %s\n", gg12s[i].serial_device->name);
                         printf("Excluding for next iteration\n");
                         excluded_gps[excluded_gps_count++] = i;
                    } else { 
                         /* The receiver is considered good */
                         /* By doing this no one receiver will disable the system */
                         good_gps = i;
                    }
               }

               (*clocktick) = true;

               memcpy(master_time_current, master_time_next, sizeof(struct str_time_gps));

               master_time_next->rcvtime = (current_time + 500) % 604800500;

               if(master_time_next->rcvtime == 0)
               {
                    master_time_next->rcvtime = 500;
               }

               if(master_time_next->rcvtime == 500)
               {
                    master_time_next->week++;
                    previous_master_time.rcvtime = 0;
               } else {
                    previous_master_time.rcvtime = master_time_next->rcvtime;
               }

               gettimeofday(&start_time,NULL);
          }
     }
     /* printf("correlate_time_serial:end\n"); */
}

unsigned int correlate_time_file(
          struct str_time_gps                 *master_time_current,
          struct str_time_gps                 *master_time_next,
          struct str_limits                   *limits,
          /* struct str_laas_logging_control     *logging_control, */
          /* struct str_serial_device             serial_devices[], */
          struct str_gg12_device               gg12s[],
          /* struct str_gg12w_device              gg12ws[], */
          /* struct str_bline_device              blines[], */
          /* struct str_zxtreme_device            zxtremes[], */
          unsigned int                         gg12_done,
          unsigned int                         gg12_process_mask,
          bool                                *tick)
{
     /* Check to see if one or more receivers are behind */
     unsigned int        gg12_process = 0;
     unsigned int        gg12_process_xor = 0;
     double master_time_unix = f_date_gps_to_unix(master_time_next->week, master_time_next->rcvtime);
     double gg12_time_unix = 0;

     int i = 0;
     *tick = false;
     for(i = 0; i < limits->NUMBER_OF_GG12S; i++)
     {
          /* Convert GPS time to unix time */
          gg12_time_unix = f_date_gps_to_unix(gg12s[i].serial_device->time_received.tm_gps.week, gg12s[i].serial_device->time_received.tm_gps.rcvtime);

          /* If the time is behind more than one day assume there was 
             a week roll over and increment the week number of that receiver */
          if(gg12_time_unix < (master_time_unix - 86400000))
          {
               gg12s[i].serial_device->time_received.tm_gps.week++;
               gg12_time_unix = f_date_gps_to_unix(gg12s[i].serial_device->time_received.tm_gps.week, gg12s[i].serial_device->time_received.tm_gps.rcvtime);
          } 

          /* If the receiver is cought up with the master time them more data can be processed
             for that receiver */
          fprintf(gg12s[0].serial_device->console, "TIME: Receiver: %f, Master: %f\n",
                    gg12_time_unix, master_time_unix);
          if(gg12_time_unix >= master_time_unix)
          {
               gg12_process |= (1 << i);
          } 
     }

     fprintf(gg12s[0].serial_device->console, "gg12_process: %04X\n", gg12_process);

     /* If some are behine then let them catch up else keep processing the data */
     gg12_process_xor = gg12_process ^ gg12_process_mask;

     fprintf(gg12s[0].serial_device->console, "1: gg12_process_xor: %04X\n", gg12_process_xor);

     for(i = 0; i < limits->NUMBER_OF_GG12S; i++)
     {
          if((gg12s[i].serial_device->search_start == 0) && 
                    (((gg12_done>>i) & 0x1) == 1))
          {
               gg12_process_xor &= ~(1 << i);
          } 
     }

     fprintf(gg12s[0].serial_device->console, "2: gg12_process_xor: %04X\n", gg12_process_xor);

     if(gg12_process_xor != 0)
     {
          gg12_process = gg12_process_xor;
          fprintf(gg12s[0].serial_device->console, "PROCESS MORE DATA\n");
     } else {

          *tick = true;

          memcpy(master_time_current, master_time_next, sizeof(struct str_time_gps));

          master_time_next->rcvtime = ((master_time_next->rcvtime + 500) % 604800500);
/*
          printf("TICK: %d\n", master_time_next->rcvtime);
          printf("\n");
*/
          fprintf(gg12s[0].serial_device->console, "TICK: %d\n", master_time_next->rcvtime);
          fprintf(gg12s[0].serial_device->console, "\n");

          if(master_time_next->rcvtime == 0)
          {
               master_time_next->rcvtime = 500;
               master_time_next->week++;
          }

/*
          if((master_time->rcvtime % 86400000) == 500)
          {
               printf("Master Time: Week: %d, Rcvtime %d\n", master_time->week, master_time->rcvtime);
               close_log_files(serial_devices, gg12s, gg12ws, blines, zxtremes);
               open_log_files(master_time, limits, logging_control, serial_devices, gg12s, gg12ws, blines, zxtremes, xyzea);
          }
*/
     }

     /* Check to see if a receiver is ahead, if so let it coast till the others are caugt up with it */

     for(i = 0; i < limits->NUMBER_OF_GG12S; i++)
     {
          gg12_time_unix = f_date_gps_to_unix(gg12s[i].serial_device->time_received.tm_gps.week, gg12s[i].serial_device->time_received.tm_gps.rcvtime);

          if(gg12_time_unix > master_time_unix)
          {
               gg12_process &= ~(1 << i);
          } 
     }

     fprintf(gg12s[0].serial_device->console, "RCVTIME: %d: ", master_time_next->rcvtime);
     for(i = 0; i < limits->NUMBER_OF_GG12S - 1; i++)
     {
          fprintf(gg12s[0].serial_device->console, "%d, ", gg12s[i].serial_device->time_received.tm_gps.rcvtime);
     }
     fprintf(gg12s[0].serial_device->console, "%d\n", gg12s[limits->NUMBER_OF_GG12S-1].serial_device->time_received.tm_gps.rcvtime);

     fprintf(gg12s[0].serial_device->console, "PROCESS: ");
     for(i = 0; i < limits->NUMBER_OF_GG12S - 1; i++)
     {
          fprintf(gg12s[0].serial_device->console, "%d, ", (gg12_process>>i) & 0x01);
     }
     fprintf(gg12s[0].serial_device->console, "%d\n", (gg12_process>>(limits->NUMBER_OF_GG12S-1)) & 0x01);

     fflush(gg12s[0].serial_device->console);

     /*
        if(gg12_process == 0)
        {
        master_time->rcvtime = (gg12s[0].serial_device->time_received.tm_gps.rcvtime < gg12s[1].serial_device->time_received.tm_gps.rcvtime) ? gg12s[0].serial_device->time_received.tm_gps.rcvtime : gg12s[1].serial_device->time_received.tm_gps.rcvtime;
        master_time->rcvtime = ( master_time->rcvtime < gg12s[2].serial_device->time_received.tm_gps.rcvtime) ? master_time->rcvtime : gg12s[2].serial_device->time_received.tm_gps.rcvtime;
        master_time->rcvtime = ( master_time->rcvtime < gg12s[3].serial_device->time_received.tm_gps.rcvtime) ? master_time->rcvtime : gg12s[3].serial_device->time_received.tm_gps.rcvtime;
        master_time->rcvtime = ( master_time->rcvtime < gg12s[4].serial_device->time_received.tm_gps.rcvtime) ? master_time->rcvtime : gg12s[4].serial_device->time_received.tm_gps.rcvtime;
        master_time->rcvtime = ( master_time->rcvtime < gg12s[5].serial_device->time_received.tm_gps.rcvtime) ? master_time->rcvtime : gg12s[5].serial_device->time_received.tm_gps.rcvtime;
        master_time->rcvtime = ( master_time->rcvtime < gg12s[6].serial_device->time_received.tm_gps.rcvtime) ? master_time->rcvtime : gg12s[6].serial_device->time_received.tm_gps.rcvtime;
        master_time->rcvtime = ( master_time->rcvtime < gg12s[7].serial_device->time_received.tm_gps.rcvtime) ? master_time->rcvtime : gg12s[7].serial_device->time_received.tm_gps.rcvtime;
        master_time->rcvtime = ( master_time->rcvtime < gg12s[8].serial_device->time_received.tm_gps.rcvtime) ? master_time->rcvtime : gg12s[8].serial_device->time_received.tm_gps.rcvtime;
        printf("Coast Error 0x%08X\n", gg12_process);
        }
        */
     return gg12_process;
}

int main(int argc, char **argv)
{
     int count_down = 100;
     /* struct str_time_gps master_time; */
     struct str_time master_time_current;
     struct str_time master_time_next;
     struct str_limits limits;
     struct str_laas_logging_control logging_control;

     struct str_serial_device            serial_devices[MAX_NUMBER_OF_SERIAL_DEVICES];
     struct str_gg12_device              gg12s[MAX_NUMBER_OF_GG12S];
     struct str_gg12w_device             gg12ws[MAX_NUMBER_OF_GG12WS];
     struct str_bline_device             blines[MAX_NUMBER_OF_BLINES];
     struct str_zxtreme_device           zxtremes[MAX_NUMBER_OF_ZXTREMES];
     struct str_telerad_device           telerads[MAX_NUMBER_OF_TELERADS];
     struct str_far_field_monitor_device ffms[MAX_NUMBER_OF_FAR_FIELD_MONITORS];

     struct str_binary_search_array binary_search_ephemeris[NUMBER_OF_GPS_SATELLITES];


     struct str_location                 centroid_location;
     struct str_location                 lm_location;
     struct str_location                 reference_stations[MAX_NUMBER_OF_REFERENCE_STATIONS];
     struct str_location                 local_monitors[MAX_NUMBER_OF_REFERENCE_STATIONS];

     struct str_satellite_almanac_ephemeris_position all_satellites_aep_information[NUMBER_OF_GPS_SATELLITES]; 

     struct str_telerad_message telerad_message;

     int max_fd = 0;        /* Max fd used by select() */
     fd_set serial_input;   /* setup file descriptor set for select() call */

     unsigned char buffer[MAX_BUFFER_SIZE]; /* The read buffer */

     int bytes_written = 0;                 /* Number of bytes written to the raw file */

     /* Variables needed to keep track of when to roll the log files */
     struct timeval tv_time;
     /*
        unsigned int old_date = get_date();
        bool set_date = true;
        unsigned int date_change = 0;
        */

     char current_time[MAX_LINE_LENGTH];

     /* setup signal mask to handle SIGINT */
     static struct sigaction signal_action;
     int return_value = 0;

     int data_file_done = 0;
     int exit_application = 0;

     int i = 0;        /* Serial device index */
     int j = 0;        /* Serial device index */
     int gg12_i = 0;    /* GG12 device index */
     int gg12w_i = 0;   /* GG12W device index */
     int zxtreme_i = 0; /* Z-XTREME device index */

     struct stat stat_str;
     time_t access_time = 0;

     unsigned int buffer_end = 0;
     unsigned int gg12_done = 0;
     unsigned int gg12_process_mask = 0;
     unsigned int gg12_process      = 0xFFFFFFFF;
     bool clocktick      = false;

/*
     int count = 1;
     int isSet = 0;
     int e_i = 0;
     int e_j = 0;
     int g_i = 0;
     int g_j = 0;
     int good_gps = 0;
     unsigned int tick_toc = 0;
*/
     int tmProcessed = 0;
     int messageSize = 0;

     unsigned int svprn = 0;
     unsigned int prn = 0;

     int stat_return = 0;

     FILE *laas_message_type1_csv = NULL;
     FILE *xyzae = NULL;
     FILE *alm_csv = NULL;
     FILE *eph_csv = NULL;

     GDBM_FILE dbf;

     /* Clear the variables */

     memset(&master_time_current, 0x00, sizeof(struct str_time));
     memset(&master_time_next, 0x00, sizeof(struct str_time));
     memset(&logging_control, 0x00, sizeof(struct str_laas_logging_control));
     memset(&limits, 0x00, sizeof(struct str_limits));
     memset(&lm_location, 0x00, sizeof(struct str_location));
     memset(&centroid_location, 0x00, sizeof(struct str_location));
     memset(reference_stations, 0x00, sizeof(struct str_location) * MAX_NUMBER_OF_REFERENCE_STATIONS);
     memset(local_monitors, 0x00, sizeof(struct str_location) * MAX_NUMBER_OF_REFERENCE_STATIONS);

     memset(serial_devices, 0x00,  sizeof(struct str_serial_device) * MAX_NUMBER_OF_SERIAL_DEVICES);
     memset(gg12s, 0x00, sizeof(struct str_gg12_device) * MAX_NUMBER_OF_GG12S);
     memset(gg12ws, 0x00, sizeof(struct str_gg12w_device) * MAX_NUMBER_OF_GG12WS);
     memset(blines, 0x00, sizeof(struct str_bline_device) * MAX_NUMBER_OF_BLINES);
     memset(zxtremes, 0x00, sizeof(struct str_zxtreme_device) * MAX_NUMBER_OF_ZXTREMES);
     memset(telerads, 0x00, sizeof(struct str_telerad_device) * MAX_NUMBER_OF_TELERADS);
     memset(ffms, 0x00, sizeof(struct str_far_field_monitor_device) * MAX_NUMBER_OF_FAR_FIELD_MONITORS);

     memset(binary_search_ephemeris, 0x00, sizeof(struct str_binary_search_array) * NUMBER_OF_GPS_SATELLITES);

     memset(all_satellites_aep_information, 0x00, sizeof(struct str_satellite_almanac_ephemeris_position) * NUMBER_OF_GPS_SATELLITES);
     for(i=0; i < NUMBER_OF_GPS_SATELLITES; i++)
     {
          all_satellites_aep_information[i].snv.valid = true;
          all_satellites_aep_information[i].snv_hold.valid = true;
     }

     /*
        telerad_clear(&tm);
        */

     for(i = 0; i < MAX_NUMBER_OF_GG12S; i++)
     {
          for(j = 0; j < GG12_CHANNELS; j++)
          {
               gg12s[i].mca.channel[i].new_message = false;
          }
     }

     memset(&serial_input, 0x00, sizeof(fd_set));

     memset(&buffer, 0x00, MAX_BUFFER_SIZE);

     memset(&tv_time, 0x00, sizeof(struct timeval));

     memset(&signal_action, 0x00, sizeof(struct sigaction));

     /* Beginning of software execution */
     /* Check to see if the input files has been passed in */
     if(argc > 1)
     {
          printf("Starting %s\n", argv[0]);
     } else {
          printf("Please provice the configuration file\n");
          printf("%s laas.config\n", argv[0]);
          exit(0);
     }

     /* Local Monitory Location take from the wiki             */
     /* http://nc.ou.edu/index.php/Reference_Station_Locations */
     lm_location.x =  -677736.175;
     lm_location.y = -5171015.741;
     lm_location.z =  3660053.531;
     lm_location.latitude  =  35.24231911509;
     lm_location.longitude = -97.4668771304;
     lm_location.altitude  = 342.3904;
     /* Calculate the centriod */
     
     signal_action.sa_handler = exit;
     sigfillset(&(signal_action.sa_mask));
     sigaction(SIGINT, &signal_action, NULL);

     /* Clear the select input structure */
     FD_ZERO(&serial_input);

     printf("Logging started at %s.\n", current_time_as_string(current_time));


     if((dbf = gdbm_open ("/logs/gdbm/laas.db", 0, GDBM_WRCREAT | GDBM_SYNC, S_IRUSR | S_IWUSR, NULL)) == NULL)
     {
          printf("Error opening database\n");
     } else {
          /* Load the data from the database   */
          gdbm_initialize_laas_data(dbf,
                                    all_satellites_aep_information,
                                    binary_search_ephemeris);
     }

     /* Use argv[2] if it has been passed */

     /* Main Loop */
     printf("Sizeof(snv): %d\n", sizeof(struct str_ashtech_snv));
     while (!exit_application)
     {
          /* Check to see if the configuration file has changed */
          stat(argv[1], &stat_str);
          if(stat_return == 0)
          {
               if(access_time < stat_str.st_ctime)
               {
                    /* Reprocess the file if it has changed */
                    printf("Configuring the LAAS Ground Facility.\n");
                    access_time = stat_str.st_ctime;

                    max_fd = configure_laas_ground_facility(argv[1], 
                              &master_time_next, 
                              &limits, 
                              &logging_control, 
                              &telerad_message, 
                              serial_devices, 
                              gg12s, 
                              gg12ws, 
                              blines, 
                              zxtremes, 
                              telerads, 
                              ffms,
                              reference_stations,
                              local_monitors,
                              &centroid_location,
                              &xyzae,
                              &alm_csv,
                              &eph_csv,
                              &laas_message_type1_csv);

                    printf("Master Time: Week: %d, Rcvtime %d\n",
                              master_time_next.tm_gps.week,
                              master_time_next.tm_gps.rcvtime);

                    memcpy(&master_time_current, &master_time_next, sizeof(struct str_time));
                    master_time_current.tm_gps.rcvtime -= 500;
                    time_gps_to_unix(&master_time_current);

                    gg12_process_mask = 0;
                    for(i=0; i < limits.NUMBER_OF_GG12S; i++)
                    {
                         gg12_process_mask |= (1 << i);
                    }
               }
          }

          if(logging_control.data_source == DATA_FILE)
          {
               data_file_done = 0;
          } else {
               /* Initialize select */
               FD_ZERO(&serial_input);
               for(i = 0; i < limits.NUMBER_OF_SERIAL_DEVICES; i++)
               {
                    FD_SET(serial_devices[i].fid,&serial_input); 
               }

               /* Initiate select */
               return_value = select(max_fd+1, &serial_input, NULL, NULL, NULL);
               /* printf("Select Done: %d\n", return_value); */
          }

          /* Process data from all the serial buffers */
          for(i = 0; i < limits.NUMBER_OF_SERIAL_DEVICES; i++)
          {
               serial_devices[i].bytes_read = 0;
               if(logging_control.data_source == DATA_FILE)
               {
                    /* Read the data from the data file */
                    buffer_end = serial_devices[i].buffer_end;
                    serial_devices[i].bytes_read = read(serial_devices[i].fid,
                              &serial_devices[i].buffer[serial_devices[i].buffer_end],
                              MAX_BUFFER_SIZE - serial_devices[i].buffer_end);

                    /*
                       if(serial_devices[i].buffer_end < MAX_BUFFER_SIZE)
                       {
                       serial_devices[i].bytes_read = read(serial_devices[i].fid, &serial_devices[i].buffer[serial_devices[i].buffer_end], 1);
                       }
                       printf("Bytes Read: %d, %d, %d\n", i , serial_devices[i].bytes_read, serial_devices[i].buffer_end);
                    */

                    if(buffer_end != MAX_BUFFER_SIZE && serial_devices[i].bytes_read == 0) 
                    {
/*
                         printf("Done Bytes Read: %s, %d, %d\n",
                                   serial_devices[i].name,
                                   buffer_end,
                                   serial_devices[i].buffer_end);
*/
                         if(serial_devices[i].type == GG12)
                         {
                              gg12_done |= (1 << serial_devices[i].type_index);
                         }
                         data_file_done++;
                         if(data_file_done >= limits.NUMBER_OF_SERIAL_DEVICES)
                         {
                              exit_application = 1;
                              printf("Exiting Application\n");
                         }
                    }
               } else {
                    /* Read the data from serial port */
                    if(FD_ISSET(serial_devices[i].fid, &serial_input))
                    {
                         serial_devices[i].bytes_read = read(serial_devices[i].fid, &serial_devices[i].buffer[serial_devices[i].buffer_end], MAX_BUFFER_SIZE - serial_devices[i].buffer_end);
                         /* 
                         printf("Bytes Read: %d, %d, %d\n", i , serial_devices[i].bytes_read, serial_devices[i].buffer_end);
                         */
                    }
                    /* Emergency Read Stop */
                    if(serial_devices[i].buffer_end == MAX_BUFFER_SIZE)
                    {
                         count_down--;
                    }

                    if(count_down <= 0)
                         exit(0);
               }


               if((logging_control.serial[i].logging_raw == true) && (serial_devices[i].bytes_read > 0))
               {
                    bytes_written = write(serial_devices[i].raw, &serial_devices[i].buffer[serial_devices[i].buffer_end], serial_devices[i].bytes_read);

                    if(serial_devices[i].bytes_read != bytes_written){
                         fprintf(serial_devices[i].console,"bytes_read != bytes_written %d != %d\n",serial_devices[i].bytes_read, bytes_written);
                         fflush(serial_devices[i].console);
                    }
               }

               if(logging_control.serial[i].parsing == false)
               {
                    serial_devices[i].buffer_end  = 0;
               } else {
                    serial_devices[i].buffer_end  += serial_devices[i].bytes_read;
               }

               switch(serial_devices[i].type)
               {
                    case GG12: 
                         gg12_i = serial_devices[i].type_index;
                         if(logging_control.serial[i].parsing == true)
                         {
                              if(((gg12_process >> gg12_i) & 0x01) == 1)
                              { 
/*
                         printf("CASE: %s, 0x%08X\n", serial_devices[i].name, gg12_process);
*/
                                   gg12s[gg12_i].processing_done = false;
                                   while((gg12s[gg12_i].processing_done != true) &&
                                             (gg12_process_incoming(&gg12s[gg12_i],
                                                                    all_satellites_aep_information,
                                                                    binary_search_ephemeris,
                                                                    &centroid_location,
                                                                    &master_time_current,
                                                                    alm_csv,
                                                                    eph_csv,
                                                                    dbf) == 0))
     
                                   {
                                        if(gg12s[gg12_i].time_mark)
                                        {
                                             gg12s[gg12_i].time_mark = false;
                                             gg12s[gg12_i].processing_done = true;

                                             if(gg12s[gg12_i].mca.logging_parsed)
                                             {
                                                  ashtech_print_mca_all(
                                                            &gg12s[gg12_i].mca,
                                                            master_time_next.tm_gps.rcvtime);
                                             }
                                        }
                                   }
                              } /* else {process} */
                         }
                         break;
                    case GG12W: 
                         gg12w_i = serial_devices[i].type_index;
                         while(gg12w_process_incoming(&gg12ws[gg12w_i], dbf) == 0); 
                         break;
                    case TELERAD: 
                         tmProcessed = 1;
                         while(tmProcessed != 0)
                         {
                              tmProcessed = telerad_process_incoming(
                                        &telerad_message,
                                        telerads[0].serial_device->buffer,
                                        &telerads[0].serial_device->buffer_end,
                                        &messageSize);
/*                              printf("tmProcessed: %d\n",tmProcessed); */
                              if(tmProcessed == 1)
                              {

                                   /*
                                      print_buffer(buf,messageSize); 
                                      printf("\n"); 
                                      telerad_print(&tm,buf,messageSize);
                                      */

                                   /*
                                      if(telerad_message.ms_id == 0x96)
                                      {
                                      printf("Command Status: %X\n", telerad_message.trca.ca & CA_COMMAND_SUCCESSFUL);
                                      }
                                      */

                                   if(telerad_message.ms_id == 0x5C)
                                   {
                                        if(telerad_message.tlm.lm.mt == 1)
                                        {
                                             printf(", Message Type: %d\n", telerad_message.tlm.lm.mt);
                                             if(laas_message_type1_csv)
                                                  laasMessage_print_csv(&telerad_message.tlm.lm, laas_message_type1_csv);
                                             else
                                                  printf("MAIN: FILE NOT OPEN\n");
                                        }
                                   }
                                             
                                   telerads[0].serial_device->buffer_end -= messageSize;
                                   memmove(telerads[0].serial_device->buffer,
                                             &telerads[0].serial_device->buffer[messageSize],
                                             telerads[0].serial_device->buffer_end);
                                   memset(&telerads[0].serial_device->buffer[telerads[0].serial_device->buffer_end],'\0',messageSize);

                                   /*
                                      telerad_clear(&tm);
                                      */
                              }
                         }

                         break;
                    case FFM:
                         printf("FFM\n");
                         break;
                    case BLINE: 
                         printf("BLINE\n");
                         break;
                    case ZXTREME:
                         /* printf("Z-XTREAM\n"); */
                         zxtreme_i = serial_devices[i].type_index;
                         while(zxtreme_process_incoming(&zxtremes[zxtreme_i], dbf) == 0)
                         {
                              if(zxtremes[zxtreme_i].time_mark)
                              {
                                   zxtremes[zxtreme_i].time_mark = false;
                                   if(zxtremes[zxtreme_i].mpc.logging_parsed)
                                   {
                                        ashtech_print_mpc_all(&zxtremes[zxtreme_i].mpc, 
                                                  zxtremes[zxtreme_i].serial_device->time_received.tm_gps.rcvtime);
                                   }
                              }
                         }
                         break;
                    default:
                         printf("RAW\n");
                         break;
               }
          }

          if(logging_control.data_source == DATA_FILE)
          { 
               gg12_process = correlate_time_file(
                         &master_time_current.tm_gps, 
                         &master_time_next.tm_gps, 
                         &limits,
                         /* &logging_control,*/
                         /* serial_devices,*/
                         gg12s,
                         /* gg12ws,*/
                         /* blines,*/
                         /* zxtremes,*/
                         gg12_done,
                         gg12_process_mask,
                         &clocktick);
          } else {
               correlate_time_serial(
                         &master_time_current.tm_gps, 
                         &master_time_next.tm_gps, 
                         &limits,
                         /* &logging_control, */
                         /* serial_devices, */
                         gg12s,
                         /* gg12ws, */
                         /* blines, */
                         /* zxtremes, */
                         /* gg12_done, */
                         /* gg12_process_mask, */
                         &clocktick);
          }

          if(clocktick)
          {
               time_gps_to_unix(&master_time_current);
               time_gps_to_unix(&master_time_next);

               /*****************************************************/
/*
               printf("Master Time: Week: %d, Rcvtime %d Unix: %d, Offset: %d\n",
                         master_time_next.tm_gps.week,
                         master_time_next.tm_gps.rcvtime,
                         unix_to_gps_rcvtime(),
                         unix_to_gps_rcvtime() - master_time_next.tm_gps.rcvtime);
*/

               /** Check to see if there was a date rollover.
                   If so roll the logs over to the new date. */
               if((master_time_next.tm_gps.rcvtime % 86400000) == 500)
               {
                    /** Close the log files */
                    close_log_files(serial_devices, gg12s, gg12ws, blines, zxtremes);
                    /** Open new log files */
                    open_log_files(&master_time_next,
                              &limits,
                              &logging_control,
                              serial_devices,
                              gg12s,
                              gg12ws,
                              blines,
                              zxtremes,
                              &xyzae,
                              &alm_csv,
                              &eph_csv,
                              &laas_message_type1_csv);
               }
     
               /* printf("Master Time: %d\n",master_time_current.tm_gps.rcvtime); */
               laas_calculation_sequence(
                         &master_time_current, 
                         &limits, 
                         &centroid_location,
                         reference_stations,
                         local_monitors,
                         gg12s, 
                         all_satellites_aep_information,
                         xyzae,
                         dbf);
          }
          fflush(stdout);
     }

     /* Clear the reamining messages from the buffer */
     for(i = 0; i < limits.NUMBER_OF_GG12S; i++)
     {
          if(gg12s[i].mca.logging_parsed)
          {
               ashtech_print_mca_all(&gg12s[i].mca, master_time_next.tm_gps.rcvtime + 500);
          }
     }
     gg12_print_statistics(gg12s,&limits);
     printf("Exiting Application\n");
     return 0;
}
