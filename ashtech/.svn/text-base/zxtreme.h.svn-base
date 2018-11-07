#ifndef ZXTREME_H
#define ZXTREME_H 1

#include "shift_buffer.h"
#include "ashtech.h"

#define ZXTREME_CHANNELS 12

struct str_zxtreme_device
{
     struct str_location loc;     /* gps antenna location */
     struct str_serial_device *serial_device;

     bool processing_done;
     bool time_mark;


     struct str_ashtech_mpc mpc;  /* pseudorange data */

     struct str_ashtech_pbn pbn;     /* position solution data */
     struct str_ashtech_sal sal;     /* ephemeris almanac data */
     struct str_ashtech_snv snv;     /* ephemeris data */
     struct str_ashtech_xyz xyz;     /* Satelitte position */

     struct str_zxtreme_stats
     {
          unsigned int count;
          int mpc_count;
          int mpc_checksum_count;
          int mis_count;
          int mis_checksum_count;
          int pbn_count;
          int pbn_checksum_count;
          int sal_count;
          int sal_checksum_count;
          int snv_count;
          int snv_checksum_count;
          int xyz_count;
          int xyz_checksum_count;
          int other_count;
     } stats;

};

void zxtreme_print_statistics(struct str_zxtreme_device zxtreme[], struct str_limits *limits);
int  zxtreme_process_incoming(struct str_zxtreme_device *c, GDBM_FILE dbf);

#endif

