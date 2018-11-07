#ifndef GG12W_H
#define GG12W_H 1

#include "shift_buffer.h"
#include "ashtech.h"

#define GG12W_CHANNELS 12

struct str_gg12w_device
{
     struct str_serial_device *serial_device;

     bool processing_done;
     bool time_mark;

     struct str_location loc;     /* gps antenna location */

     struct str_ashtech_mca mca;     /* pseudorange data */
     struct str_ashtech_mis mis;     /* misc. timing info */
     struct str_ashtech_pbn pbn;     /* position solution data */
     struct str_ashtech_sal sal;     /* ephemeris almanac data */
     struct str_ashtech_snv snv;     /* ephemeris data */
     struct str_ashtech_xyz xyz;     /* Satelitte position */

     struct str_gg12w_stats
     {
          unsigned int count;
          int mca_count;
          int mca_checksum_count;
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

void gg12w_print_statistics(struct str_gg12w_device gg12w[], struct str_limits *limits);
int  gg12w_process_incoming(struct str_gg12w_device *c, GDBM_FILE dbf);

#endif

