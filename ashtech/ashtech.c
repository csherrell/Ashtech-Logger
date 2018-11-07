#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <gdbm.h>
#include <stdlib.h>
#include "../find_string.h"
#include "../conversions.h"
#include "../gps.h"
#include "../time_struct.h"
#include "../time_conversions.h"
#include "../laas.h"
#include "ashtech.h"

void ashtech_parse_mca(unsigned char *x, struct str_ashtech_mca *mca, int index)
{
     mca->channel[index].new_message = true;
     mca->channel[index].log_message = true;

     mca->channel[index].sequence    = (unsigned int) raw_to_ushort(&x[0]);
     mca->channel[index].left        = (unsigned int) x[2];
     mca->channel[index].svprn       = (unsigned int) x[3];
     mca->channel[index].elev        = (unsigned int) x[4];
     mca->channel[index].azim        = (unsigned int) x[5];
     mca->channel[index].chnind      = (unsigned int) x[6];
     mca->channel[index].warning     = (unsigned int) x[7];
     mca->channel[index].goodbad     = (unsigned int) x[8];
     mca->channel[index].polarity    = (int) x[9];
     mca->channel[index].ireg        = (unsigned int) x[10];
     mca->channel[index].qa_phase    = (unsigned int) x[11];
     mca->channel[index].full_phase  = raw_to_double(&x[12]);
     mca->channel[index].raw_range   = raw_to_double(&x[20]);
     mca->channel[index].doppler     = raw_to_int(&x[28]);
     mca->channel[index].smoothing   = raw_to_uint(&x[32]);
     mca->channel[index].checksum    = (unsigned int) x[36];
}

void ashtech_print_mca(struct str_ashtech_mca *mca, int i, unsigned int rcvtime)
{
     if(mca->csv)
     {
/*
          fprintf(mca->csv,"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %.9f, %.54f, %d, %d\n",
                    unix_to_gps_rcvtime(),
*/
          fprintf(mca->csv,"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %.9f, %.54f, %d, %d\n",
                    rcvtime,
                    mca->channel[i].sequence, mca->channel[i].left,
                    mca->channel[i].svprn, mca->channel[i].elev,
                    mca->channel[i].azim, mca->channel[i].chnind, 
                    mca->channel[i].warning, mca->channel[i].goodbad,
                    mca->channel[i].polarity, mca->channel[i].ireg,
                    mca->channel[i].qa_phase, mca->channel[i].full_phase,
                    mca->channel[i].raw_range, mca->channel[i].doppler,
                    mca->channel[i].smoothing);
          fflush(mca->csv);
     }
}

void ashtech_print_mca_all(struct str_ashtech_mca *mca, unsigned int rcvtime)
{
     int i = 0; 

     for(i = 0; i < MAX_ASHTECH_CHANNELS; i++)
     {
          if( mca->channel[i].log_message == true)
          {
               ashtech_print_mca(mca, i, rcvtime);
          }
          mca->channel[i].log_message = false;
     }
}

void ashtech_parse_mis(unsigned char *x, struct str_ashtech_mis *mis)
{
     mis->new_message = true;

     mis->rcvtime         =                raw_to_uint(&x[0]);
     mis->rcvtimefrac     = (unsigned int) raw_to_ushort(&x[4]);
     mis->numGpsSatsUsed  = (unsigned int) x[6];
     mis->numGloSatsUsed  = (unsigned int) x[7];
     mis->numGpsSatsTrkd  = (unsigned int) x[8];
     mis->numGloSatsTrkd  = (unsigned int) x[9];
     mis->positionMode    = (unsigned int) raw_to_ushort(&x[10]);
     mis->geoidalSep      = (int) raw_to_short(&x[12]);
     mis->hdop            = (unsigned int) raw_to_ushort(&x[14]);
     mis->vdop            = (unsigned int) raw_to_ushort(&x[16]);
     mis->ModZcnt         = (unsigned int) raw_to_ushort(&x[18]);
     mis->gloTimeShift    = raw_to_float(&x[20]);
     mis->gpsNavFlags     = raw_to_uint(&x[24]);
     mis->gloNavFlags     = raw_to_uint(&x[28]);
     mis->gpsExcSats      = raw_to_uint(&x[32]);
     mis->gloExcSats      = raw_to_uint(&x[36]);
     mis->checksum        = (unsigned int) raw_to_ushort(&x[40]);
}

void ashtech_print_mis(struct str_ashtech_mis *mis)
{
     if(mis->csv)
     {
          fprintf(mis->csv,"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %u, %u, %u, %u\n",
                    mis->rcvtime, mis->rcvtimefrac, mis->numGpsSatsUsed, mis->numGloSatsUsed,
                    mis->numGpsSatsTrkd, mis->numGloSatsTrkd, mis->positionMode, mis->geoidalSep,
                    mis->hdop, mis->vdop, mis->ModZcnt, mis->gloTimeShift, mis->gpsNavFlags, mis->gloNavFlags,
                    mis->gpsExcSats, mis->gloExcSats);
          fflush(mis->csv);
     }
}

void ashtech_parse_pbn(unsigned char *x, struct str_ashtech_pbn *pbn)
{
     pbn->new_message = true;

     pbn->rcvtime    = raw_to_uint(&x[0]);
     pbn->name[0]    = x[4];
     pbn->name[1]    = x[5];
     pbn->name[2]    = x[6];
     pbn->name[3]    = x[7];
     pbn->navx       = raw_to_double(&x[8]);
     pbn->navy       = raw_to_double(&x[16]);
     pbn->navz       = raw_to_double(&x[24]);
     pbn->navt       = raw_to_float(&x[32]);
     pbn->navxdot    = raw_to_float(&x[36]);
     pbn->navydot    = raw_to_float(&x[40]);
     pbn->navzdot    = raw_to_float(&x[44]);
     pbn->navtdot    = raw_to_float(&x[48]);
     pbn->pdop       = (unsigned int) raw_to_ushort(&x[52]);
     pbn->checksum   = (unsigned int) raw_to_ushort(&x[54]);
}


void ashtech_print_pbn(struct str_ashtech_pbn *pbn)
{
     if(pbn->csv)
     {
          fprintf(pbn->csv, "%d, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %d\n",
                    pbn->rcvtime, pbn->navx, pbn->navy, pbn->navz, pbn->navt,
                    pbn->navxdot, pbn->navydot, pbn->navzdot, pbn->navtdot,
                    pbn->pdop);
          fflush(pbn->csv);
     }
}

void ashtech_parse_sal(unsigned char *x, struct str_ashtech_sal *sal)
{
     sal->new_message = true;

     sal->prn      = (int) raw_to_short(&x[0]);
     sal->health   = (int) raw_to_short(&x[2]);
     sal->e        = raw_to_float(&x[4]);
     sal->toa      = raw_to_int(&x[8]);
     sal->i0       = raw_to_float(&x[12]);
     sal->omegadot = raw_to_float(&x[16]);
     sal->roota    = raw_to_double(&x[20]);
     sal->omega0   = raw_to_double(&x[28]);
     sal->omega    = raw_to_double(&x[36]);
     sal->m0       = raw_to_double(&x[44]);
     sal->af0      = raw_to_float(&x[52]);
     sal->af1      = raw_to_float(&x[56]);
     sal->wna      = (int) raw_to_short(&x[60]);
     sal->wn       = (int) raw_to_short(&x[62]);
     sal->tow      = raw_to_int(&x[64]);
     sal->checksum = (unsigned int) raw_to_ushort(&x[68]);
}


void ashtech_print_sal(struct str_ashtech_sal *sal, unsigned int rcvtime)
{
     if(sal->csv)
     {
          fprintf(sal->csv, "%d, %d, %d, %24.24f, %d, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %d, %d, %d\n",
                    rcvtime,
                    sal->prn, sal->health, sal->e, sal->toa, sal->i0, 
                    sal->omegadot, sal->roota, sal->omega0, sal->omega,
                    sal->m0, sal->af0, sal->af1, sal->wna, sal->wn, sal->tow);

          fflush(sal->csv);
     }
}

void ashtech_print_sal_stdio(struct str_ashtech_sal *sal, unsigned int rcvtime)
{
     printf("%d, %d, %d, %24.24f, %d, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %d, %d, %d\n",
               rcvtime,
               sal->prn, sal->health, sal->e, sal->toa, sal->i0, 
               sal->omegadot, sal->roota, sal->omega0, sal->omega,
               sal->m0, sal->af0, sal->af1, sal->wna, sal->wn, sal->tow);
}

int ashtech_store_sal_old(struct str_ashtech_sal *sal, GDBM_FILE dbf)
{
     /* struct str_ashtech_sal forTest; */
     char key_string[MAX_LINE_LENGTH];

     datum key;
     datum content;

     memset(key_string, 0x00, MAX_LINE_LENGTH);

     if(sal->prn + 1 > NUMBER_OF_GPS_SATELLITES)
     {
          printf("Invalid SAL PRN: %d\n", sal->prn);
          return -4;
     }

     /* N for New */
     sprintf(key_string,"SAL-N-%d",sal->prn);

     key.dptr = key_string;
     key.dsize = strlen(key_string);

     content.dptr =  (char *) sal;
     content.dsize = sizeof(struct str_ashtech_sal);

     switch(gdbm_store(dbf, key, content, GDBM_REPLACE))
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

void ashtech_zero_snv(struct str_ashtech_snv *snv)
{
     snv->is_set   = false;
     snv->valid    = true;

     snv->time_received.tm_unix.sec   = 0;
     snv->time_received.tm_unix.usec  = 0;
     snv->time_received.tm_gps.rcvtime  = 0;
     snv->time_received.tm_gps.week     = 0;

     snv->wn       = 0;
     snv->tow      = 0;
     snv->tgd      = 0;
     snv->aodc     = 0;
     snv->toc      = 0;
     snv->af2      = 0;
     snv->af1      = 0;
     snv->af0      = 0;
     snv->aode     = 0;
     snv->deltan   = 0;
     snv->m0       = 0;
     snv->e        = 0;
     snv->roota    = 0;
     snv->toe      = 0;
     snv->cic      = 0;
     snv->crc      = 0;
     snv->cis      = 0;
     snv->crs      = 0;
     snv->cuc      = 0;
     snv->cus      = 0;
     snv->omega0   = 0;
     snv->omega    = 0;
     snv->i0       = 0;
     snv->omegadot = 0;
     snv->idot     = 0;
     snv->accuracy = 0;
     snv->health   = 0;
     snv->fit      = 0;
     snv->prnnum   = 0;
     snv->res      = 0;
     snv->checksum = 0;
}


void ashtech_parse_snv(unsigned char *x, struct str_ashtech_snv *snv)
{

     snv->is_set = true;
     snv->valid  = true;

     snv->wn         = (int)          raw_to_short(   &x[0]);
     snv->tow        =                raw_to_int(     &x[2]);
     snv->tgd        =                raw_to_float(   &x[6]);
     snv->aodc       =                raw_to_int(     &x[10]);
     snv->toc        =                raw_to_int(     &x[14]);
     snv->af2        =                raw_to_float(   &x[18]);
     snv->af1        =                raw_to_float(   &x[22]);
     snv->af0        =                raw_to_float(   &x[26]);
     snv->aode       =                raw_to_int(     &x[30]);
     snv->deltan     =                raw_to_float(   &x[34]);
     snv->m0         =                raw_to_double(  &x[38]);
     snv->e          =                raw_to_double(  &x[46]);
     snv->roota      =                raw_to_double(  &x[54]);
     snv->toe        =                raw_to_int(     &x[62]);
     snv->cic        =                raw_to_float(   &x[66]);
     snv->crc        =                raw_to_float(   &x[70]);
     snv->cis        =                raw_to_float(   &x[74]);
     snv->crs        =                raw_to_float(   &x[78]);
     snv->cuc        =                raw_to_float(   &x[82]);
     snv->cus        =                raw_to_float(   &x[86]);
     snv->omega0     =                raw_to_double(  &x[90]);
     snv->omega      =                raw_to_double(  &x[98]);
     snv->i0         =                raw_to_double(  &x[106]);
     snv->omegadot   =                raw_to_float(   &x[114]);
     snv->idot       =                raw_to_float(   &x[118]);
     snv->accuracy   = (int)          raw_to_short(   &x[122]);
     snv->health     = (int)          raw_to_short(   &x[124]);
     snv->fit        = (int)          raw_to_short(   &x[126]);
     snv->prnnum     = (int)          x[128];
     snv->res        = (int)          x[129];
     snv->checksum   = (unsigned int) x[130];
}

void ashtech_print_snv(struct str_ashtech_snv *snv)
{
     if(snv->csv)
     {
          fprintf(snv->csv, "%d, %d, %d, %24.24f, %d, %d, %24.24f, %24.24f, %24.24f, %d, %24.24f, %24.24f, %24.24f, %24.24f, %d, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %24.24f, %d, %d, %d, %d, %d\n",
                    snv->time_received.tm_gps.rcvtime,
                    snv->wn, snv->tow, snv->tgd, snv->aodc, snv->toc,
                    snv->af2, snv->af1, snv->af0, snv->aode, snv->deltan,
                    snv->m0, snv->e, snv->roota, snv->toe,
                    snv->cic, snv->crc, snv->cis, snv->crs, snv->cuc, snv->cus,
                    snv->omega0, snv->omega, snv->i0, snv->omegadot, snv->idot,
                    snv->accuracy, snv->health, snv->fit, snv->prnnum, snv->res);

          fflush(snv->csv);
     }
}

int ashtech_store_snv_old(struct str_ashtech_snv *snv, GDBM_FILE dbf)
{
     /* struct str_ashtech_snv forTest; */
     char key_string[MAX_LINE_LENGTH];

     datum key;
     datum content;

     memset(key_string, 0x00, MAX_LINE_LENGTH);

     if(snv->prnnum + 1 > NUMBER_OF_GPS_SATELLITES)
     {
          printf("Invalid SNV PRN: %d\n", snv->prnnum);
          return -4;
     }

     /* N for New */
     sprintf(key_string,"SNV-N-%d",snv->prnnum);

     key.dptr = key_string;
     key.dsize = strlen(key_string);

     content.dptr =  (char *) snv;
     content.dsize = sizeof(struct str_ashtech_snv);

     switch(gdbm_store(dbf, key, content, GDBM_REPLACE))
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

/*
     memset(&forTest, 0, sizeof(struct str_ashtech_snv));
     content.dptr = NULL;

     content = gdbm_fetch (dbf, key);
     memcpy(&forTest, content.dptr, content.dsize);
     free(content.dptr);

     if(memcmp(&forTest, snv, sizeof(struct str_ashtech_snv)) != 0)
     { 
          printf("SNV Fetch Invalid\n");
     } else {
          printf("SNV Fetch Successful\n");
     }
*/
}


/*
 * compute GG12 XYZ message size (since it is variable)
 */
unsigned int ashtech_xyz_size(unsigned char *x)
{
     /*
      * size of fields:
      *  4 bytes:  time of week
      *  2 bytes:  number of satellites
      * 34 bytes:  XYZ data foreach satellite
      *  2 bytes:  checksum
      */

     return (8 + (34 * raw_to_ushort(&x[4])));
}


void ashtech_parse_xyz(unsigned char *x, struct str_ashtech_xyz *xyz)
{
     unsigned int i = 0;
     xyz->new_message = true;

     xyz->rcvtime =                raw_to_uint(&x[0]);
     xyz->numSats = (unsigned int) raw_to_short(&x[4]);

     for (i = 0; i < xyz->numSats; i++)
     {
          xyz->channel[i].sv      = (unsigned int) raw_to_short (   &x[34*i + 6]);
          xyz->channel[i].x       = raw_to_double(&x[34*i + 8]);
          xyz->channel[i].y       = raw_to_double(&x[34*i + 16]);
          xyz->channel[i].z       = raw_to_double(&x[34*i + 24]);
          xyz->channel[i].range   = raw_to_double(&x[34*i + 32]);
     }
     xyz->checksum = raw_to_ushort(&x[34*xyz->numSats + 6] );
}

void ashtech_print_xyz(struct str_ashtech_xyz *xyz)
{
     unsigned int i = 0;

     if(xyz->csv)
     {
          for (i = 0; i < xyz->numSats; i++)
          {
               fprintf(xyz->csv, "%d, %d, %d, %.9f, %.9f, %.9f, %.9f\n", 
                         xyz->rcvtime, xyz->numSats - i - 1,
                         xyz->channel[i].sv, xyz->channel[i].x, 
                         xyz->channel[i].y, xyz->channel[i].z, 
                         xyz->channel[i].range);
          }
          fflush(xyz->csv);
     }
}

int ashtech_checksum_8(unsigned char *in, int size, FILE *console)
{
     int i;
     unsigned char sum = 0;
     unsigned char check = 0;
     struct timeval rTime;
     char heh[80];
     unsigned char diff = 0;

     /* xor all 8-bit values in data... */
     /* excluding header '$PASHR,XXX,' and checksum (last 2 bytes) */

     for (i = 0; i < size-1; i++)
          sum ^= in[i];

     check = in[size-1];

     if (sum == check)
     {
          return 0;
     }

     /*
        memset(heh, 0x0, 80);
        snprintf(heh, 11, "%s", &in[-11]);

        ashtech_print_checksum_stats(serial_device, heh);
        */

     diff = sum ^ check;

     gettimeofday(&rTime,NULL);
     if(console)
     {
          fprintf(console,"%.4f,  invalid: %5d != %5d, %s, %d\n",
                    rTime.tv_sec + (rTime.tv_usec/1000000.0), sum, check, heh, diff);
          fflush(console);
     }

     return -1;  
}

/*
 * verify 16-bit checksum at end of message
 *
 * *in includes the $PASHR header to the \n\n ending
 */
int ashtech_checksum_16(unsigned char *in, int size, FILE *console)
{
     int i;

     struct timeval rTime;
     unsigned short sum = 0;
     unsigned short check = 0;
     char heh[80];

     /* add all 16-bit values in data... */
     /* excluding header '$PASHR,XXX,' and checksum (last 2 bytes) */

     for (i = 0; i < size-2; i+=2)
     {
          sum += raw_to_ushort(&in[i]);
     }

     check = raw_to_ushort(&in[size-2]);

     /*   
          for (i = 0; i < size-3; i++)
          printf("%02x", in[i]);
          printf("\n");
          */

     if (sum == check)
     {
          return 0;
     }

     memset(heh, 0x0, 80);
     /*
        snprintf(heh, 11, "%s", &in[-11]);
        ashtech_print_checksum_stats(ashtechDevice, heh);
        */

     gettimeofday(&rTime,NULL);
     if(console)
     {
          fprintf(console, "%.4f,  invalid: %5d != %5d, %s, %d\n",
                    rTime.tv_sec + (rTime.tv_usec/1000000.0), sum, check, heh, sum-check);
          fflush(console);
     }
     return -1;                                    /* invalid */
}

void ashtech_parse_mpc(unsigned char *x, struct str_ashtech_mpc *mpc, int index)
{
     mpc->channel[index].new_message = true;

     mpc->channel[index].sequence    = (unsigned int) raw_to_ushort(&x[0]);
     mpc->channel[index].left        = (unsigned int) x[2];
     mpc->channel[index].svprn       = (unsigned int) x[3];
     mpc->channel[index].elev        = (unsigned int) x[4];
     mpc->channel[index].azim        = (unsigned int) x[5];
     mpc->channel[index].chnind      = (unsigned int) x[6];

     mpc->channel[index].ca_warning     = (unsigned int) x[7];
     mpc->channel[index].ca_goodbad     = (unsigned int) x[8];
     mpc->channel[index].ca_reserved    = (unsigned int) x[9];
     mpc->channel[index].ca_ireg        = (unsigned int) x[10];
     mpc->channel[index].ca_spare       = (unsigned int) x[11];
     mpc->channel[index].ca_full_phase  = raw_to_double(&x[12]);
     mpc->channel[index].ca_raw_range   = raw_to_double(&x[20]);
     mpc->channel[index].ca_doppler     = raw_to_int(&x[28]);
     mpc->channel[index].ca_smoothing   = raw_to_uint(&x[32]);

     mpc->channel[index].l1_warning     = (unsigned int) x[36];
     mpc->channel[index].l1_goodbad     = (unsigned int) x[37];
     mpc->channel[index].l1_reserved    = (unsigned int) x[38];
     mpc->channel[index].l1_ireg        = (unsigned int) x[39];
     mpc->channel[index].l1_spare       = (unsigned int) x[40];
     mpc->channel[index].l1_full_phase  = raw_to_double(&x[41]);
     mpc->channel[index].l1_raw_range   = raw_to_double(&x[49]);
     mpc->channel[index].l1_doppler     = raw_to_int(&x[57]);
     mpc->channel[index].l1_smoothing   = raw_to_uint(&x[61]);

     mpc->channel[index].l2_warning     = (unsigned int) x[65];
     mpc->channel[index].l2_goodbad     = (unsigned int) x[66];
     mpc->channel[index].l2_reserved    = (unsigned int) x[67];
     mpc->channel[index].l2_ireg        = (unsigned int) x[68];
     mpc->channel[index].l2_spare       = (unsigned int) x[69];
     mpc->channel[index].l2_full_phase  = raw_to_double(&x[70]);
     mpc->channel[index].l2_raw_range   = raw_to_double(&x[78]);
     mpc->channel[index].l2_doppler     = raw_to_int(&x[86]);
     mpc->channel[index].l2_smoothing   = raw_to_uint(&x[90]);

     mpc->channel[index].checksum    = (unsigned int) x[94];
}

void ashtech_print_mpc(struct str_ashtech_mpc *mpc, int index, unsigned int rcvtime)
{

     if(mpc->csv)
     {

          fprintf(mpc->csv," %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %.9f, %.54f, %d, %d, %d, %d, %d, %d, %d, %.9f, %.54f, %d, %d, %d, %d, %d, %d, %d, %.9f, %.54f, %d, %d\n",
                    rcvtime,
                    mpc->channel[index].sequence,
                    mpc->channel[index].left,
                    mpc->channel[index].svprn,
                    mpc->channel[index].elev,
                    mpc->channel[index].azim,
                    mpc->channel[index].chnind,
                    mpc->channel[index].ca_warning,
                    mpc->channel[index].ca_goodbad,
                    mpc->channel[index].ca_reserved,
                    mpc->channel[index].ca_ireg,
                    mpc->channel[index].ca_spare,
                    mpc->channel[index].ca_full_phase,
                    mpc->channel[index].ca_raw_range,
                    mpc->channel[index].ca_doppler,
                    mpc->channel[index].ca_smoothing,
                    mpc->channel[index].l1_warning,
                    mpc->channel[index].l1_goodbad,
                    mpc->channel[index].l1_reserved,
                    mpc->channel[index].l1_ireg,
                    mpc->channel[index].l1_spare,
                    mpc->channel[index].l1_full_phase,
                    mpc->channel[index].l1_raw_range,
                    mpc->channel[index].l1_doppler,
                    mpc->channel[index].l1_smoothing,
                    mpc->channel[index].l2_warning,
                    mpc->channel[index].l2_goodbad,
                    mpc->channel[index].l2_reserved,
                    mpc->channel[index].l2_ireg,
                    mpc->channel[index].l2_spare,
                    mpc->channel[index].l2_full_phase,
                    mpc->channel[index].l2_raw_range,
                    mpc->channel[index].l2_doppler,
                    mpc->channel[index].l2_smoothing);
          fflush(mpc->csv);
     }
}

void ashtech_print_mpc_all(struct str_ashtech_mpc *mpc, unsigned int rcvtime)
{
     int i = 0;

     for(i = 0; i < MAX_ASHTECH_CHANNELS; i++)
     {
          if( mpc->channel[i].new_message == true)
          {
               ashtech_print_mpc(mpc, i, rcvtime);
          }
          mpc->channel[i].new_message = false;
     }
}

