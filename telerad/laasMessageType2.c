#define _ISOC9X_SOURCE  1
#define _ISOC99_SOURCE  1
#include  <math.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils_string.h"
#include "laasMessageType2.h"
#include "../laas.h"

#define MASK_GSRR    0x00001
#define MASK_GSAD    0x00002
#define MASK_SPARE1  0x00004
#define MASK_GCID    0x00008
#define MASK_LMV     0x00010
#define MASK_SPARE2  0x00020
#define MASK_SVIG    0x00040
#define MASK_RI      0x00080
#define MASK_SH      0x00100
#define MASK_RU      0x00200
#define MASK_LAT     0x00400
#define MASK_LON     0x00800
#define MASK_RPH     0x01000
#define MASK_RSDS    0x02000
#define MASK_MUD     0x04000
#define MASK_KMEPG   0x08000
#define MASK_KMECG   0x10000
#define MASK_KMEPGL  0x20000
#define MASK_KMECGL  0x40000

void
setValidityLmt2(struct laasMessageType2 *msg)
{
    isValidLmt2Gsrr(msg);
    isValidLmt2Gsad(msg);
    isValidLmt2Gcid(msg);
    isValidLmt2Lmv(msg);
    isValidLmt2Svig(msg);
    isValidLmt2Ri(msg);
    isValidLmt2Sh(msg);
    isValidLmt2Lat(msg);
    isValidLmt2Lon(msg);
    isValidLmt2Rph(msg);
    isValidLmt2Rsds(msg);
    isValidLmt2Mud(msg);
    isValidLmt2Kmepg(msg);
    isValidLmt2Kmecg(msg);
    isValidLmt2Kmepgl(msg);
    isValidLmt2Kmecgl(msg);
}


void
laasMessageType2_process(struct laasMessageType2 *msgType2, unsigned char msgData[], unsigned int length)
{
    char c_temp = 0;

    laasMessageType2_clear(msgType2);

    msgType2->gsrr   = (msgData[0] & 0x03) + 2;        /* 0 = 2 refrence receivers installed */
    msgType2->gsad   = ((msgData[0] & 0x0C) >> 2);
    msgType2->spare1 = ((msgData[0] & 0x10) >> 4);
    msgType2->gcid   = ((msgData[0] & 0xE0) >> 5);

    c_temp = msgData[2] & 0x07;
    c_temp <<= 5;
    c_temp >>= 5;
    msgType2->lmv = (c_temp | msgData[1]) * 0.25;
    msgType2->spare2 = ((msgData[2] & 0xF8) >> 3);

    msgType2->svig = msgData[3] * 0.1 * 10e-6;

    c_temp = msgData[4];
    msgType2->ri = ((c_temp * 3) + 400);                     /* 0 = 400 -- 246B Page 28 */

    msgType2->sh = msgData[5] * 100;
    msgType2->ru = msgData[6];

    msgType2->lat = ((((msgData[10]<<24) | (msgData[9]<<16) | (msgData[8]<<8) | (msgData[7])) * 0.0005) / 3600);

    msgType2->lon  = ((((msgData[14]<<24) | (msgData[13]<<16) | (msgData[12]<<8) | (msgData[11])) * 0.0005) / 3600);

    msgType2->rph = ((msgData[17]<<16 | (msgData[16]<<8) | (msgData[15])) * 0.01);

    if (length == 34) {
        msgType2->rsds   = msgData[18];
        msgType2->mud    = msgData[19] * 2;
        msgType2->kmepg  = msgData[20] * 0.05;
        msgType2->kmecg  = msgData[21] * 0.05;
        msgType2->kmepgl = msgData[22] * 0.05;
        msgType2->kmecgl = msgData[23] * 0.05;

        msgType2->adb    = 1; /* Additional Data Block Flag */
    }
    setValidityLmt2(msgType2);
}

int
getLmt2AsByteArray(struct laasMessageType2 *msgType2, unsigned char msgData[])
{

    double dbl = 0;
    long   lng = 0;

    msgData[0] = ((msgType2->gsrr - 2)    & 0x03) |
                 ((msgType2->gsad   << 2) & 0x0C) | 
                 ((msgType2->spare1 << 4) & 0x10) |
                 ((msgType2->gcid   << 5) & 0xE0);

    dbl = msgType2->lmv / 0.25;
    lng = lrint(dbl);
    msgData[1] = (lng & 0xFF);
    msgData[2] = ((lng >> 8) & 0x07);
    msgData[2] |= ((msgType2->spare2 & 0x1F) << 3);

    dbl = (msgType2->svig / (0.1 * 10e-6));
    lng = lrint(dbl);
    msgData[3] = lng & 0xFF;
    msgData[4] = ((((int)msgType2->ri) - 400) / 3 ) & 0xFF;
    msgData[5] = msgType2->sh / 100;
    msgData[6] = msgType2->ru;

    dbl = ((msgType2->lat * 3600) / 0.0005) ;
    lng = lrint(dbl);
    msgData[7] =  ((lng)     & 0xFF);
    msgData[8] =  ((lng>>8)  & 0xFF);
    msgData[9] =  ((lng>>16) & 0xFF);
    msgData[10] = ((lng>>24) & 0xFF);

    dbl = ((msgType2->lon * 3600) / 0.0005) ;
    lng = lrint(dbl);
    msgData[11] = ((lng) & 0xFF); 
    msgData[12] = ((lng>>8) & 0xFF);
    msgData[13] = ((lng>>16) & 0xFF);
    msgData[14] = ((lng>>24) & 0xFF);

    dbl = (msgType2->rph / 0.01) ;
    lng = lrint(dbl);
    msgData[15] = (lng & 0xFF);
    msgData[16] = ((lng>>8) & 0xFF);
    msgData[17] = ((lng>>16) & 0xFF);

    if(msgType2->adb)
    { 
         msgData[18] = msgType2->rsds;
         msgData[19] = msgType2->mud / 2;

         dbl = (msgType2->kmepg / 0.05) ;
         lng = lrint(dbl);
         msgData[20] = lng & 0xFF;

         dbl = (msgType2->kmecg / 0.05) ;
         lng = lrint(dbl);
         msgData[21] = lng & 0xFF;

         dbl = (msgType2->kmepgl / 0.05) ;
         lng = lrint(dbl);
         msgData[22] = lng & 0xFF;

         dbl = (msgType2->kmecgl / 0.05) ;
         lng = lrint(dbl);
         msgData[23] = lng & 0xFF;

         return 24;
    }

    return 18;

}

struct laasMessageType2 *
laasMessageType2_init()
{
    struct laasMessageType2 *mt2 = 0;
    mt2 = (struct laasMessageType2 *) malloc(sizeof(struct laasMessageType2));
    memset(mt2,'\0',sizeof(struct laasMessageType2));
    return mt2;
}

struct laasMessageType2 *
laasMessageType2_delete(struct laasMessageType2 *mt2)
{
    free(mt2);
    mt2 = 0;
    return mt2;
}

void
laasMessageType2_clear(struct laasMessageType2 *mt2)
{
    memset(mt2,'\0',sizeof(struct laasMessageType2));
}

void laasMessageType2_print(struct laasMessageType2 *msg, unsigned char msgData[])
{
    double integer  = 0;
    double fraction = 0;
    long   lng  = 0;

    printf("Laas Message Type2 ------------------\n");

    printf("Message Segment: [");
    print_buffer(msgData,24);
    printf("]\n");

    printf("                 RawMessage              get isValid\n");

/***** Print GSRR *******/
    printf("gsrr:                  0x%02X       %10d ",(msgData[0] & 0x03), 
                                          getLmt2Gsrr(msg));
    if(!isValidLmt2Gsrr(msg))
        printf("Invalid");
    printf("\n");

/***** Print GSAD *******/
    printf("gsad:                  0x%02X                ",((msgData[0]& 0x0C)>>2));
     switch(getLmt2Gsad(msg))
     {
          case 0: printf("A\n");
                  break;
          case 1: printf("B\n");
                  break;
          case 2: printf("C\n");
                  break;
          default: printf("Spare\n");
                  break;
     }

/***** Print SPARE1 *****/
    printf("spare1:                0x%02X       %10d\n",((msgData[0] & 0x10) >> 4),
                                                                getLmt2Spare1(msg));

/***** Print GCID *******/
    printf("gcid:                  0x%02X       %10d\n",((msgData[0] & 0xE0) >> 5), 
                                                               getLmt2Gcid(msg));

/***** Print LVM ********/
    printf("lmv:                 0x%04X       %10.02f ",(((msgData[2] & 0x07) << 8) | msgData[1]),
                                                             getLmt2Lmv(msg));
    if(!isValidLmt2Lmv(msg))
        printf("Invalid");
    printf("\n");

/***** Print SPARE2 *****/
    printf("spare2:                0x%02X       %10d\n",(msgData[2] & 0xF8),
                                                               getLmt2Spare2(msg));

/***** Print SVIG *******/
    printf("svig:                  0x%02X       %10.7f ",msgData[3],
                                                               getLmt2Svig(msg));
    if(!isValidLmt2Svig(msg))
        printf("Invalid");
    printf("\n");

/***** Print RI *********/
    printf("ri:                    0x%02X       %10d ",msgData[4], 
                                                             getLmt2Ri(msg));
    if(!isValidLmt2Ri(msg))
        printf("Invalid");
    printf("\n");

/***** Print SH *********/
    printf("sh:                    0x%02X       %10d ",msgData[5], 
                                                              getLmt2Sh(msg));
    if(!isValidLmt2Sh(msg))
        printf("Invalid");
    printf("\n");

/***** Print RU *********/
    printf("ru:                    0x%02X       %10d\n",msgData[6], 
                                                              getLmt2Ru(msg));

/***** Print LAT ********/
    printf("lat:             0x%08X       %10.4f Deg ",(msgData[10]<<24 | msgData[9]<<16 | msgData[8]<<8 | msgData[7]),
                                                   getLmt2Lat(msg));
    if(!isValidLmt2Lat(msg))
        printf("Invalid\n");
    else
    {
          printf("\n");
          fraction = modf(getLmt2Lat(msg),&integer);
          lng = lrint(integer);
          printf("                              %ld Deg ",lng);
          fraction *= 60;
          fraction = modf(fraction,&integer);
          lng = lrint(integer);
          printf("%ld Min ",lng);
          fraction *= 60;
          lng = lrint(fraction);
          printf("%ld Sec\n",lng);

          fraction = getLmt2Lat(msg) * 3600;
          lng = lrint(fraction);
          printf("                              %ld Seconds\n",lng);
    }
/***** Print LON ********/
    printf("lon:             0x%08X       %10.4f Deg ",(msgData[14]<<24 | msgData[13]<<16 | msgData[12]<<8 | msgData[11]),
                                                    getLmt2Lon(msg));
    if(!isValidLmt2Lon(msg))
        printf("Invalid\n");
    else
    {
          printf("\n");
          fraction = modf(getLmt2Lon(msg),&integer);
          lng = lrint(integer);
          printf("                              %ld Deg ",lng);
          fraction *= 60;
          fraction = modf(fraction,&integer);
          lng = lrint(integer);
          printf("%ld Min ",lng);
          fraction *= 60;
          lng = lrint(fraction);
          printf("%ld Sec\n",lng);

          fraction = getLmt2Lon(msg) * 3600;
          lng = lrint(fraction);
          printf("                              %ld Seconds\n",lng);
     }

/***** Print RPH ********/
    printf("rph:               0x%06X       %10.2f ",(msgData[17]<<16 | msgData[16]<<8 | msgData[15]),
                                                         getLmt2Rph(msg));
    if(!isValidLmt2Rph(msg))
        printf("Invalid");
    printf("\n");

    if(msg->adb)
    {
/***** Print RSDS *******/
        printf("rsds:                  0x%02X       %10d ",msgData[18],
                                                              getLmt2Rsds(msg));
        if(!isValidLmt2Rsds(msg))
            printf("Invalid");
        printf("\n");

/***** Print MUD ********/
        printf("mud:                   0x%02X       %10d ",msgData[19],
                                                              getLmt2Mud(msg));
        if(!isValidLmt2Mud(msg))
            printf("Invalid");
        printf("\n");

/***** Print KMEPG ******/
        printf("kmepg:                 0x%02X       %10.02f ",msgData[20],
                                                                 getLmt2Kmepg(msg));
        if(!isValidLmt2Kmepg(msg))
            printf("Invalid");
        printf("\n");

/***** Print KMECG ******/
        printf("kmecg:                 0x%02X       %10.02f ",msgData[21],
                                                                 getLmt2Kmecg(msg));
        if(!isValidLmt2Kmecg(msg))
            printf("Invalid");
        printf("\n");

/***** Print KMEPGL *****/
        printf("kmepgl:                0x%02X       %10.02f ",msgData[22],
                                                                 getLmt2Kmepgl(msg));
        if(!isValidLmt2Kmepgl(msg))
            printf("Invalid");
        printf("\n");

/***** Print KMECGL *****/
        printf("kmecgl:                0x%02X       %10.02f ",msgData[23],
                                                                 getLmt2Kmecgl(msg));
        if(!isValidLmt2Kmecgl(msg))
            printf("Invalid");
        printf("\n");
    }
}

void laasMessageType2_print_csv(struct laasMessageType2 *msg, unsigned char msgData[])
{
/***** Print GSRR *******/
    printf("%d,", getLmt2Gsrr(msg));

/***** Print GSAD *******/
    printf("%d,",((msgData[0]& 0x0C)>>2));

/***** Print SPARE1 *****/
    printf("%d,", getLmt2Spare1(msg));

/***** Print GCID *******/
    printf("%d,", getLmt2Gcid(msg));

/***** Print LVM ********/
    printf("%0.25f,", getLmt2Lmv(msg));

/***** Print SPARE2 *****/
    printf("%d,", getLmt2Spare2(msg));

/***** Print SVIG *******/
    printf("%0.25f,", getLmt2Svig(msg));

/***** Print RI *********/
    printf("%d,", getLmt2Ri(msg));

/***** Print SH *********/
    printf("%d,", getLmt2Sh(msg));

/***** Print RU *********/
    printf("%d,", getLmt2Ru(msg));

/***** Print LAT ********/
    printf("%0.25f,", getLmt2Lat(msg));

/***** Print LON ********/
    printf("%0.25f,", getLmt2Lon(msg));

/***** Print RPH ********/
    printf("%0.25f,", getLmt2Rph(msg));

/***** Print RSDS *******/
    printf("%d,", getLmt2Rsds(msg));

/***** Print MUD ********/
    printf("%d,", getLmt2Mud(msg));

/***** Print KMEPG ******/
    printf("%0.25f,", getLmt2Kmepg(msg));

/***** Print KMECG ******/
    printf("%0.25f,", getLmt2Kmecg(msg));

/***** Print KMEPGL *****/
    printf("%0.25f,", getLmt2Kmepgl(msg));

/***** Print KMECGL *****/
    printf("%0.25f,", getLmt2Kmecgl(msg));

}

void laasMessageType2_set(struct laasMessageType2 *msg,
                          unsigned int gsrr,
                          unsigned int gsad,
                          unsigned int spare1,
                          unsigned int gcid,
                          int lmv,
                          unsigned int spare2,
                          unsigned int svig,
                          unsigned int ri,
                          unsigned int sh,
                          unsigned int ru,
                          int lat,
                          int lon,
                          int rph,
                          unsigned int rsds,
                          unsigned int mud,
                          unsigned int kmepg,
                          unsigned int kmecg,
                          unsigned int kmepgl,
                          unsigned int kmecgl)
{
    msg->gsrr = gsrr;
    msg->gsad = gsad;
    msg->spare1 = spare1;
    msg->gcid = gcid;
    msg->lmv = lmv;
    msg->spare2 = spare2;
    msg->svig = svig;
    msg->ri = ri;
    msg->sh = sh;
    msg->ru = ru;
    msg->lat = lat;
    msg->lon = lon;
    msg->rph = rph;
    msg->rsds = rsds;
    msg->mud = mud;
    msg->kmepg = kmepg;
    msg->kmecg = kmecg;
    msg->kmepgl = kmepgl;
    msg->kmecgl = kmecgl;
}

void laasMessageType2_raw_set(struct laasMessageType2 *msg,
                      unsigned char *rawMessage)
{
     msg->gsrr = rawMessage[0];
     msg->gsad = rawMessage[1];
     msg->spare1 = rawMessage[2];
     msg->gcid = rawMessage[3];
     msg->lmv = utils_string_raw_to_int(&rawMessage[4]);
     msg->spare2 = rawMessage[8];
     msg->svig = rawMessage[9];
     msg->ri = rawMessage[10];
     msg->sh = rawMessage[11];
     msg->ru = rawMessage[12];
     msg->lat = utils_string_raw_to_int(&rawMessage[13]);
     msg->lon = utils_string_raw_to_int(&rawMessage[17]);
     msg->rph = utils_string_raw_to_int(&rawMessage[21]);
     msg->rsds = rawMessage[25];
     msg->mud = rawMessage[26];
     msg->kmepg = rawMessage[27];
     msg->kmecg = rawMessage[28];
     msg->kmepgl = rawMessage[29];
     msg->kmecgl = rawMessage[30];
}

/********************************************************************/
unsigned int
getLmt2Adb(struct laasMessageType2 *msg)
{
     return msg->adb;
}

void
setLmt2Adb(struct laasMessageType2 *msg, unsigned int val)
{
     msg->adb = val;
}

/********************************************************************/
unsigned int
getLmt2Gsrr(struct laasMessageType2 *msg)
{
     return msg->gsrr;
}

void
setLmt2Gsrr(struct laasMessageType2 *msg, unsigned int val)
{
     msg->gsrr = val;
     isValidLmt2Gsrr(msg);
}

int
isValidLmt2Gsrr(struct laasMessageType2 *msg)
{
     /* Range 2 - 4   */
     if(msg->gsrr < 2 || msg->gsrr > 4)
     {
         msg->valid |= MASK_GSRR;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_GSRR;
         return true;
     }
}

/********************************************************************/
unsigned int
getLmt2Gsad(struct laasMessageType2 *msg)
{
     return msg->gsad;
}

void
setLmt2Gsad(struct laasMessageType2 *msg, unsigned int val)
{
     msg->gsad = val;
     isValidLmt2Gsad(msg);
}

int
isValidLmt2Gsad(struct laasMessageType2 *msg)
{
/* Range 0 - 3   */
     if(msg->gsad > 3)
     {
         msg->valid |= MASK_GSAD;     /* false */
         return false;
     }
     else
     {
         msg->valid &= ~MASK_GSAD;    /* true  */
         return true;
     }
}   


/********************************************************************/
unsigned int
getLmt2Spare1(struct laasMessageType2 *msg)
{
     return (unsigned int) msg->spare1;
}

void
setLmt2Spare1(struct laasMessageType2 *msg, unsigned int val)
{
     msg->spare1 = (unsigned int) val;
}

/********************************************************************/
unsigned int
getLmt2Gcid(struct laasMessageType2 *msg)
{
     return msg->gcid;
}

void
setLmt2Gcid(struct laasMessageType2 *msg, unsigned int val)
{
     msg->gcid = val;
     isValidLmt2Gcid(msg);
}

int
isValidLmt2Gcid(struct laasMessageType2 *msg)
{
/* Range 0 - 7   */
     if(msg->gcid > 7)
     {
         msg->valid |= MASK_GCID;     /* false */
         return false;
     }
     else
     {
         msg->valid &= ~MASK_GCID;    /* true  */
         return true;
     }
}   

/********************************************************************/
double
getLmt2Lmv(struct laasMessageType2 *msg)
{
     return msg->lmv;
}

void
setLmt2Lmv(struct laasMessageType2 *msg, double val)
{
     msg->lmv = val;
     isValidLmt2Lmv(msg);
}

int
isValidLmt2Lmv(struct laasMessageType2 *msg)
{
     /* Range:   -180 - 180 degrees, Resolution: 0.25 */
     if(msg->lmv < -180 || msg->lmv > 180)
     {
         msg->valid |= MASK_LMV;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_LMV;
         return true;
     }
}

/********************************************************************/
unsigned int
getLmt2Spare2(struct laasMessageType2 *msg)
{
     return (unsigned int) msg->spare2;
}

void
setLmt2Spare2(struct laasMessageType2 *msg, unsigned int val)
{
     msg->spare2 = (unsigned int) val;
}

/********************************************************************/
double
getLmt2Svig(struct laasMessageType2 *msg)
{
     return msg->svig;
}

void
setLmt2Svig(struct laasMessageType2 *msg, double val)
{
     msg->svig = val;
     isValidLmt2Svig(msg);
}

int
isValidLmt2Svig(struct laasMessageType2 *msg)
{
     /* RawRange 0 - 255 */
     /* Range    0 - 25.5x10^-6 m/m, Resolution: 0.1 * 10e-6 */
     if(msg->svig < 0 || msg->svig > 25.5e-6)
     {
         msg->valid |= MASK_SVIG;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_SVIG;
         return true;
     }
}

/********************************************************************/
int
getLmt2Ri(struct laasMessageType2 *msg)
{
     return msg->ri;
}

void
setLmt2Ri(struct laasMessageType2 *msg, int val)
{
     msg->ri = val;
     isValidLmt2Ri(msg);
}

int
isValidLmt2Ri(struct laasMessageType2 *msg)
{
/* Range 16 - 781  Resolution: 3 */
     if(msg->ri < 16 || msg->ri > 781)
     {
         msg->valid |= MASK_RI;     /* false */
         return false;
     }
     else
     {
         msg->valid &= ~MASK_RI;    /* true  */
         return true;
     }
}

/********************************************************************/
int
getLmt2Sh(struct laasMessageType2 *msg)
{
     return msg->sh;
}

void
setLmt2Sh(struct laasMessageType2 *msg, int val)
{
     msg->sh = val;
     isValidLmt2Sh(msg);
}

int
isValidLmt2Sh(struct laasMessageType2 *msg)
{
/* Range 0 - 25500 m Resolution: 100 */
     if(msg->sh > 25500)
     {
         msg->valid |= MASK_SH;     /* false */
         return false;
     }
     else
     {
         msg->valid &= ~MASK_SH;    /* true  */
         return true;
     }
}

/********************************************************************/
unsigned int
getLmt2Ru(struct laasMessageType2 *msg)
{
     return (unsigned int) msg->ru;
}

void
setLmt2Ru(struct laasMessageType2 *msg, unsigned int val)
{
     msg->ru = (unsigned int) val;
}

/********************************************************************/
double
getLmt2Lat(struct laasMessageType2 *msg)
{
     return msg->lat;
}

void
setLmt2Lat(struct laasMessageType2 *msg, double val)
{
     msg->lat = val;
     isValidLmt2Lat(msg);
}

int
isValidLmt2Lat(struct laasMessageType2 *msg)
{
/* Range -90 - 90 arcsec Resolution: 0.0005 */
     if(msg->lat < -90 || msg->lat > 90)
     {
         msg->valid |= MASK_LAT;     /* false */
         return false;
     }
     else
     {
         msg->valid &= ~MASK_LAT;    /* true  */
         return true;
     }
}

/********************************************************************/
double
getLmt2Lon(struct laasMessageType2 *msg)
{
     return msg->lon;
}

void
setLmt2Lon(struct laasMessageType2 *msg, double val)
{
     msg->lon = val;
     isValidLmt2Lon(msg);
}

int
isValidLmt2Lon(struct laasMessageType2 *msg)
{
/* Range -180 - 180 arcsec Resolution: 0.0005 */
     if(msg->lon < -180 || msg->lon > 180)
     {
         msg->valid |= MASK_LON;     /* false */
         return false;
     }
     else
     {
         msg->valid &= ~MASK_LON;    /* true  */
         return true;
     }
}

/********************************************************************/
double
getLmt2Rph(struct laasMessageType2 *msg)
{
     return msg->rph;
}

void
setLmt2Rph(struct laasMessageType2 *msg, double val)
{
     msg->rph = val;
     isValidLmt2Rph(msg);
}

int
isValidLmt2Rph(struct laasMessageType2 *msg)
{
/* Range -83886.07 - 83886.07 m Resolution: 0.01 */
     if(msg->rph < -83886.07 || msg->rph > 83886.07)
     {
         msg->valid |= MASK_RPH;     /* false */
         return false;
     }
     else
     {
         msg->valid &= ~MASK_RPH;    /* true  */
         return true;
     }
}

/********************************************************************/
unsigned int
getLmt2Rsds(struct laasMessageType2 *msg)
{
     return msg->rsds;
}

void
setLmt2Rsds(struct laasMessageType2 *msg, unsigned int val)
{
     msg->rsds = val;
     isValidLmt2Rsds(msg);
}

int
isValidLmt2Rsds(struct laasMessageType2 *msg)
{
     /* RawRange 0 - 48 */
     /* Range    0 - 48, Resolution: 1 */
     if(msg->rsds > 48)
     {
         msg->valid |= MASK_RSDS;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_RSDS;
         return true;
     }
}

/********************************************************************/
unsigned int
getLmt2Mud(struct laasMessageType2 *msg)
{
     return msg->mud;
}

void
setLmt2Mud(struct laasMessageType2 *msg, unsigned int val)
{
     msg->mud = val;
     isValidLmt2Mud(msg);
}

int
isValidLmt2Mud(struct laasMessageType2 *msg)
{
/* Range 2 - 510 km Resolution: 2 */
     if(msg->mud < 2 || msg->mud > 510)
     {
         msg->valid |= MASK_MUD;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_MUD;
         return true;
     }
}

/********************************************************************/
double
getLmt2Kmepg(struct laasMessageType2 *msg)
{
     return msg->kmepg;
}

void
setLmt2Kmepg(struct laasMessageType2 *msg, double val)
{
     msg->kmepg = val;
     isValidLmt2Kmepg(msg);
}

int
isValidLmt2Kmepg(struct laasMessageType2 *msg)
{
     /* RawRange 0 - 255 */
     /* Range    0 - 12.75, Resolution: 0.05 */
     if(msg->kmepg < 0 || msg->kmepg > 12.75)
     {
         msg->valid |= MASK_KMEPG;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_KMEPG;
         return true;
     }
}

/********************************************************************/
double
getLmt2Kmecg(struct laasMessageType2 *msg)
{
     return msg->kmecg;
}

void
setLmt2Kmecg(struct laasMessageType2 *msg, double val)
{
     msg->kmecg = val;
     isValidLmt2Kmecg(msg);
}

int
isValidLmt2Kmecg(struct laasMessageType2 *msg)
{
     /* RawRange 0 - 255 */
     /* Range    0 - 12.75, Resolution: 0.05 */
     if(msg->kmecg < 0 || msg->kmecg > 12.75)
     {
         msg->valid |= MASK_KMECG;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_KMECG;
         return true;
     }
}

/********************************************************************/
double
getLmt2Kmepgl(struct laasMessageType2 *msg)
{
     return msg->kmepgl;
}

void
setLmt2Kmepgl(struct laasMessageType2 *msg, double val)
{
     msg->kmepgl = val;
     isValidLmt2Kmepgl(msg);
}

int
isValidLmt2Kmepgl(struct laasMessageType2 *msg)
{
     /* RawRange 0 - 255 */
     /* Range    0 - 12.75, Resolution: 0.05 */
     if(msg->kmepgl < 0 || msg->kmepgl > 12.75)
     {
         msg->valid |= MASK_KMEPGL;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_KMEPGL;
         return true;
     }
}

/********************************************************************/
double
getLmt2Kmecgl(struct laasMessageType2 *msg)
{
     return msg->kmecgl;
}

void
setLmt2Kmecgl(struct laasMessageType2 *msg, double val)
{
     msg->kmecgl = val;
     isValidLmt2Kmecgl(msg);
}

int
isValidLmt2Kmecgl(struct laasMessageType2 *msg)
{
     /* RawRange 0 - 255 */
     /* Range    0 - 12.75, Resolution: 0.05 */
     if(msg->kmecgl < 0 || msg->kmecgl > 12.75)
     {
         msg->valid |= MASK_KMECGL;
         return false;
     }
     else
     {
         msg->valid &= ~MASK_KMECGL;
         return true;
     }
}
