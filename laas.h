#ifndef LAAS_H
#define LAAS_H 1

#include <stdbool.h>
#include "time_struct.h"

/* Parameters define how much memory is allocated */
#define MAX_LINE_LENGTH                   80
#define MAX_BUFFER_SIZE                   8000

#define MAX_NUMBER_OF_LOCAL_MONITORS      2
#define MAX_NUMBER_OF_REFERENCE_STATIONS  4
#define MAX_NUMBER_OF_REFERENCE_RECEIVERS 2 * MAX_NUMBER_OF_REFERENCE_STATIONS
#define MAX_NUMBER_OF_GG12S               12
#define MAX_NUMBER_OF_GG12WS              1
#define MAX_NUMBER_OF_BLINES              1
#define MAX_NUMBER_OF_ZXTREMES            1
#define MAX_NUMBER_OF_GPS                 MAX_NUMBER_OF_GG12S + MAX_NUMBER_OF_GG12WS + MAX_NUMBER_OF_BLINES + MAX_NUMBER_OF_ZXTREMES
#define MAX_NUMBER_OF_TELERADS            2
#define MAX_NUMBER_OF_FAR_FIELD_MONITORS  2
#define MAX_NUMBER_OF_SERIAL_DEVICES      MAX_NUMBER_OF_GPS + MAX_NUMBER_OF_TELERADS + MAX_NUMBER_OF_FAR_FIELD_MONITORS

#define MAX_NUMBER_OF_SECTOR_MASK         5    

enum enum_device_type {GG12, GG12W, TELERAD, FFM, BLINE, ZXTREME, RAW};
enum enum_data_source {SERIAL, DATA_FILE};

#define LAAS_HEADER_SIZE 6

/* Internation Alphabet subset */
/* D0-246B 2.4.2 */
static const char ia_5[] = {'\0', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '\0', '\0', '\0', '\0', '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0', '\0', '\0', '\0', '\0'};

static const unsigned char ia_5_to_bin[] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 0x20, '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0','\0', '\0', 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, '\0', '\0',  '\0',  '\0',  '\0',  '\0',  '\0', 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, '\0',  '\0',  '\0',  '\0',  '\0',  '\0',  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A};

struct str_location
{
     /* ECEF X, Y, Z */
     double x;
     double y;
     double z;
     double latitude;
     double longitude;
     double altitude;
};

struct str_sector_mask
{
     float minimum_elevation;
     float maximum_elevation;
     float minimum_azimuth;
     float maximum_azimuth;
};


/* Parameters define how much memory is used */
struct str_limits
{
     int NUMBER_OF_GG12S;
     int NUMBER_OF_GG12WS;
     int NUMBER_OF_BLINES;
     int NUMBER_OF_ZXTREMES;
     int NUMBER_OF_GPS;
     int NUMBER_OF_TELERADS;
     int NUMBER_OF_FAR_FIELD_MONITORS;
     int NUMBER_OF_REFERENCE_STATIONS;
     int NUMBER_OF_LOCAL_MONITORS;
     int NUMBER_OF_SERIAL_DEVICES;
};

struct str_serial_logging_control {
     int logging_console;
     int logging_raw;
     int parsing;
     /* These options are not used unless SERIAL PORT TYPE = GG12 */
     int transaction_log;

/*
     bool gg12_logging_parsed_mca;
     bool gg12_logging_parsed_mis;
     bool gg12_logging_parsed_pbn;
     bool gg12_logging_parsed_sal;
     bool gg12_logging_parsed_snv;
*/

};

struct str_laas_logging_control
{
     char log_directory[MAX_LINE_LENGTH];
     char location_identification[MAX_LINE_LENGTH];

     bool data_sources_configured;

     /* 0 = Read the data from a serial port */
     /* 1 = Read the data from a file */
     enum enum_data_source  data_source;

     struct str_serial_logging_control serial[MAX_NUMBER_OF_SERIAL_DEVICES];
};


struct str_serial_device
{
/*     char type[MAX_LINE_LENGTH]; */         /* GG12, GG12W, TELERAD...     */ 
     enum enum_device_type type;
     int  type_index;
     char type_semantic[MAX_LINE_LENGTH]; /* GG12, GG12W, TELERAD...     */
     char name[MAX_LINE_LENGTH];          /* RR-1.1, LM-1                */
     char tty[MAX_LINE_LENGTH];           /* /dev/tty# port name         */
     int  baud_rate;         /* 9600, 115200 most used      */
     int  fid;               /* device file descriptor      */
     int  raw;               /* File to log the raw data to */
     FILE *console;          /* Device output file */
     FILE *transaction;      /* Device transaction log */

/* GPS Time stamp */
/*
     int week;
     unsigned int rcvtime;
*/
     struct str_time time_received;

     unsigned char buffer[MAX_BUFFER_SIZE];
     int  buffer_end;
     int  bytes_read;
     unsigned int search_start;
};

struct str_far_field_monitor_device
{
     struct str_location       loc;
     struct str_serial_device *serial_device;
};

struct str_bline_device
{
     struct str_location       loc;
     struct str_serial_device *serial_device;
};

#endif

