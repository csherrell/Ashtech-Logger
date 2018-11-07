#ifndef ASHTECH_H
#define ASHTECH_H 1

#include <stdbool.h>
#include <gdbm.h>
#include <time.h>
#include "../time_struct.h"
/*
#include "../gps.h"
*/

#define MCA_PACKET_SIZE      48   /* 37 + 11 */
#define MIS_PACKET_SIZE      53   /* 42 + 11 */
#define MPC_PACKET_SIZE     106   /* 95 + 11 */
#define PBN_PACKET_SIZE      67   /* 56 + 11 */
#define SAL_PACKET_SIZE      81   /* 70 + 11 */
#define SNV_PACKET_SIZE     143   /* 132 + 11 */
#define XYZ_MIN_PACKET_SIZE  53   /* Minimum Packet Size */
#define XYZ_MAX_PACKET_SIZE 427   /* Maximum Packet Size */

#define MCA_IDENTIFIER 0x004D4341    /* "MCA" in HEX */
#define MIS_IDENTIFIER 0x004D4953
#define MPC_IDENTIFIER 0x004D5043
#define PBN_IDENTIFIER 0x0050424E
#define SAL_IDENTIFIER 0x0053414C
#define SNV_IDENTIFIER 0x00534E56
#define XYZ_IDENTIFIER 0x0058595A

#define NEW_LINE_LINE_FEED      2    /* 0x0D0A */
#define PASH_HEADER     "$PASHR,"
#define PASH_HEADER_SIZE        7    /* strlen(PASH_HEADER) */
#define PASH_TYPE_SIZE          4    /* XXX, */
#define PASH_HEADER_TOTAL      11    /* PASH_HEADER_SIZE + PASH_TYPE_SIZE */

#define DURATION 36000 /* ((30 * 60 * 1000) / 50) - 30 min with 50 Millisecond resolution*/

#define MAX_ASHTECH_CHANNELS 12   /* Max of GG12 GG12W and ZXTREME */


enum ashtech_type {MCA, MIS, MPC, PBN, SAL, SNV, XYZ};


/*
 * ASHTECH MCA RAW PSEUDORANGE DATA (TYPE 3)
 * see pg. 86 of GG12 manual for more info
*/
struct str_ashtech_mca_data
{
     bool new_message;
     bool log_message;
     double  full_phase;  /* 8  Full carrier phase (measured in cycles) */
     double  raw_range;   /* 8  Raw range (in seconds) to the satellite
                             receive time - transmitted time = raw range */

     int           polarity;    /* 1  This number is either zero or five:
                                   0 - the satellite has just been locked
                                   5 - meaning the first frame of the
                                   message has been found  */
     int     doppler;     /* 4  doppler measurement (10-4 Hz) */

     unsigned int  smoothing;   /* 4  Smoothing Data */
     unsigned int  sequence;    /* 2  Sequence ID (units of 50 ms, mod 30 min) */
     unsigned int  left;        /* 1  Number of remaining MCA msg to be sent */
     unsigned int  svprn;       /* 1  Satellite PRN number (1 - 56) */
     unsigned int  elev;        /* 1  Elevation angle (degrees) */
     unsigned int  azim;        /* 1  Azimuth angle (2 deg increments) */
     unsigned int  chnind;      /* 1  Channel (1 - 12) assigned to the sat */
     unsigned int  warning;     /* 1  See Table 4.39  */
     unsigned int  goodbad;     /* 1  Indicates the quality of measurement */
     unsigned int  ireg;      /* 1  Signal-to-noise measurement (db Hz) */
     unsigned int  qa_phase;  /* 1  Not used; always zero */
     unsigned int  checksum;    /* 1  XOR checksum of above data */
};
/* Total Bytes: 37 */

struct str_ashtech_mca
{
     bool logging_parsed;
     FILE *csv;
     struct str_ashtech_mca_data channel[MAX_ASHTECH_CHANNELS];
};

/*
 * ASHTECH MISC. RAW DATA
 * see pg. 90 of GG12 manual for more info
 */
struct str_ashtech_mis
{
     bool logging_parsed;
     FILE *csv;
     bool new_message;

     unsigned int rcvtime;         /* 4 Time of GPS week (ms) */
     unsigned int rcvtimefrac;     /* 2 Fraction of time (microsec) */
     unsigned int numGpsSatsUsed;  /* 1 No. of GPS SVs in soln. */
     unsigned int numGloSatsUsed;  /* 1 No. of GLONASS SVs in soln. */
     unsigned int numGpsSatsTrkd;  /* 1 No. of GPS SVs tracked */
     unsigned int numGloSatsTrkd;  /* 1 No. of GLONASS SVs tracked */
     unsigned int positionMode;    /* 2 See pg. 90 */
     int          geoidalSep;      /* 2 Geoidal Seperation (100x) */
     unsigned int hdop;            /* 2 HDOP value (100x) */
     unsigned int vdop;            /* 2 VDOP value (100x) */
     unsigned int ModZcnt;         /* 2 Modified Z count */
     float        gloTimeShift;    /* 4 Offset between GLO and GPS */
     unsigned int gpsNavFlags;     /* 4 See pg. 91 */
     unsigned int gloNavFlags;     /* 4 See pg. 91 */
     unsigned int gpsExcSats;      /* 4 GPS exclusion bitmask */
     unsigned int gloExcSats;      /* 4 GLONASS exclusion bitmask */
     unsigned int checksum;        /* 2 Least sig. 16 bits of sum */
     /* Total Bytes: 42 */
};

struct str_ashtech_mpc
{
     bool logging_parsed;
     FILE *csv;

     struct str_mpc_data
     {
          bool new_message;

          unsigned int  sequence;    /* 2  Sequence ID (units of 50 ms, mod 30 min) */
          unsigned int  left;        /* 1  Number of remaining MCA msg to be sent */
          unsigned int  svprn;       /* 1  Satellite PRN number (1 - 56) */
          unsigned int  elev;        /* 1  Elevation angle (degrees) */
          unsigned int  azim;        /* 1  Azimuth angle (2 deg increments) */
          unsigned int  chnind;      /* 1  Channel (1 - 12) assigned to the sat */

/* C/A Code Data Block */
          unsigned int  ca_warning;     /* 1  See Table 4.39  */
          unsigned int  ca_goodbad;     /* 1  Indicates the quality of measurement */
          unsigned int  ca_reserved;    /* 1  Set to 5 for backward compatibility */
          unsigned int  ca_ireg;        /* 1  Signal-to-noise measurement (db Hz) */
          unsigned int  ca_spare;       /* 1  Not used; always zero */
          double        ca_full_phase;  /* 8  Full carrier phase (measured in cycles) */
          double        ca_raw_range;   /* 8  Raw range (in seconds) to the satellite
                                           receive time - transmitted time = raw range */
          int           ca_doppler;     /* 4  doppler measurement (10-4 Hz) */
          unsigned int  ca_smoothing;   /* 4  Smoothing Data */
          unsigned int  ca_checksum;    /* 1  XOR checksum of above data */

/* L1 Code Data Block */
          unsigned int  l1_warning;     /* 1  See Table 4.39  */
          unsigned int  l1_goodbad;     /* 1  Indicates the quality of measurement */
          unsigned int  l1_reserved;    /* 1  Set to 5 for backward compatibility */
          unsigned int  l1_ireg;        /* 1  Signal-to-noise measurement (db Hz) */
          unsigned int  l1_spare;       /* 1  Not used; always zero */
          double        l1_full_phase;  /* 8  Full carrier phase (measured in cycles) */
          double        l1_raw_range;   /* 8  Raw range (in seconds) to the satellite
                                           receive time - transmitted time = raw range */
          int           l1_doppler;     /* 4  doppler measurement (10-4 Hz) */
          unsigned int  l1_smoothing;   /* 4  Smoothing Data */
          unsigned int  l1_checksum;    /* 1  XOR checksum of above data */

/* L2 Code Data Block */
          unsigned int  l2_warning;     /* 1  See Table 4.39  */
          unsigned int  l2_goodbad;     /* 1  Indicates the quality of measurement */
          unsigned int  l2_reserved;    /* 1  Set to 5 for backward compatibility */
          unsigned int  l2_ireg;        /* 1  Signal-to-noise measurement (db Hz) */
          unsigned int  l2_spare;       /* 1  Not used; always zero */
          double        l2_full_phase;  /* 8  Full carrier phase (measured in cycles) */
          double        l2_raw_range;   /* 8  Raw range (in seconds) to the satellite
                                           receive time - transmitted time = raw range */
          int           l2_doppler;     /* 4  doppler measurement (10-4 Hz) */
          unsigned int  l2_smoothing;   /* 4  Smoothing Data */
          unsigned int  checksum;    /* 1  XOR checksum of above data */

     } channel[MAX_ASHTECH_CHANNELS];
     /* Total Bytes: 95 */
};


/*
 * ASHTECH PBN RAW POSITION DATA
 * see pg. 93 of GG12 manual for more info
 */
struct str_ashtech_pbn
{
     bool logging_parsed;
     FILE *csv;
     bool new_message;

     unsigned int  rcvtime;/* 4 time of week in which the signal (ms) */

     char    name[4];       /* 4 character ASCII user string */
     double  navx;         /* 8 Antenna position ECEF x coordinate (m) */
     double  navy;         /* 8 Antenna position ECEF y coordinate (m) */
     double  navz;         /* 8 Antenna position ECEF z coordinate (m) */
     float   navt;         /* 4 Receiver clock offset (error) (m) */
     float   navxdot;      /* 4 The antenna x velocity (m/s) */
     float   navydot;      /* 4 The antenna y velocity (m/s) */
     float   navzdot;      /* 4 The antenna z velocity (m/s) */
     float   navtdot;      /* 4 Receiver clock drift (m/s) */
     unsigned int  pdop;          /* 2 PDOP multiplied by 100. */
     unsigned int  checksum;      /* 2 The checksum is computed by breaking
                                     the structure into 27 ushorts, adding
                                     them together, and taking the least
                                     significant 16 bits of the result. */
     /* Total bytes: 56 */
};

/** \brief ASHTECH ALMANAC DATA

    See page 
    <a href="http://wiki/files/0/05/GG12_Rev_A.pdf#SAL: GPS Satellite Almanac Data">100</a>
    of the GG12 Reference Manual for more information.

*/
struct str_ashtech_sal
{
     bool logging_parsed;
     FILE *csv;
     bool new_message;
     bool is_set;

     struct str_time time_received;

     /* C Type                Bytes Description */
     int               prn; /**< \brief 2 Bytes - Satellite PRN number minus 1 (0 to 31) */
     int            health; /**< \brief 2 Bytes - Satellite health */
     float               e; /**< \brief 4 Bytes - Eccentricity */
     int               toa; /**< \brief 4 Bytes - Reference time for orbit (sec) */
     float              i0; /**< \brief 4 Bytes - Inclination angle (semicircles) */
     float        omegadot; /**< \brief 4 Bytes - Rate of right ascension (semicircles/sec) */
     double          roota; /**< \brief 8 Bytes - Sqr root of semi-major axis (met 1/2) */
     double         omega0; /**< \brief 8 Bytes - Longitude of ascending node (semicircles) */
     double          omega; /**< \brief 8 Bytes - Argument of perigee (semicircles) */
     double             m0; /**< \brief 8 Bytes - Mean anomaly at ref. time (semicircle) */
     float             af0; /**< \brief 4 Bytes - Clock correction (sec) */
     float             af1; /**< \brief 4 Bytes - Clock correction (sec/sec) */
     int               wna; /**< \brief 2 Bytes - Almanac week number */
     int                wn; /**< \brief 2 Bytes - GPS week number */
     int               tow; /**< \brief 4 Bytes - Seconds of GPS week */
     unsigned int checksum; /**< \brief 2 Bytes - The checksum is computed by breaking
                                                  the structure into 27 unsigned shorts, adding
                                                  them together, and taking the least
                                                  significant 16 bits of the result. */
     /*                       70 Bytes Total */
};

/** \brief ASHTECH EPHEMERIS DATA

    See page 103 of the GG12 Reference Manual for more information.

    http://wiki/
*/
struct str_ashtech_snv
{
     bool logging_parsed; /**< \brief Boolean flag is set if the LGF is going to log the data to a csv file */
     FILE *csv;           /**< \brief CSV file handle that the SNV data will be written to */
     bool is_set;         /**< \brief Boolean flag is set for a newly received SNV */

     /* double source_availability_duration;*/ /**< \brief Source Avaliability Duration is the time that the satellite will set */
     bool valid;                          /**< \brief Valid Flag defined in the LLRD for snv_update() */
/*
     unsigned int rcvtime;
*/
     struct str_time time_received;

     int          wn;       /**< \brief 2 Bytes - GPS week number */
     int          tow;      /**< \brief 4 Bytes - Seconds of GPS week */
     float        tgd;      /**< \brief 4 Bytes - Group delay (seconds) */
     int          aodc;     /**< \brief 4 Bytes - Clock data issue */
     int          toc;      /**< \brief 4 Bytes - Clock data reference time in seconds */
     float        af2;      /**< \brief 4 Bytes - Clock correction (sec/sec2) */
     float        af1;      /**< \brief 4 Bytes - Clock correction (sec/sec) */
     float        af0;      /**< \brief 4 Bytes - Clock correction (sec) */
     int          aode;     /**< \brief 4 Bytes - Orbit data issue */
     float        deltan;   /**< \brief 4 Bytes - Mean anomaly correction (semicirc/sec) */
     double       m0;       /**< \brief 8 Bytes - Mean anomaly at ref. time (semicirc) */
     double       e;        /**< \brief 8 Bytes - Eccentricity */
     double       roota;    /**< \brief 8 Bytes - Sqr root of semi-major axis (met 1/2) */
     int          toe;      /**< \brief 4 Bytes - Reference time for orbit (sec) */
     float        cic;      /**< \brief 4 Bytes - Harmonic correction term (radians) */
     float        crc;      /**< \brief 4 Bytes - Harmonic correction term (meters) */
     float        cis;      /**< \brief 4 Bytes - Harmonic correction term (radians) */
     float        crs;      /**< \brief 4 Bytes - Harmonic correction term (meters) */
     float        cuc;      /**< \brief 4 Bytes - Harmonic correction term (radians) */
     float        cus;      /**< \brief 4 Bytes - Harmonic correction term (radians) */
     double       omega0;   /**< \brief 8 Bytes - Longitude of ascending node (semicircles) */
     double       omega;    /**< \brief 8 Bytes - Argument of perigee (semicircles) */
     double       i0;       /**< \brief 8 Bytes - Inclination angle (semicircles) */
     float        omegadot; /**< \brief 4 Bytes - Rate of right ascension (semicircles/sec) */
     float        idot;     /**< \brief 4 Bytes - Rate of inclination (semicircles/sec) */
     int          accuracy; /**< \brief 2 Bytes - User range accuracy */
     int          health;   /**< \brief 2 Bytes - Satellite health */
     int          fit;      /**< \brief 2 Bytes - Curve fit interval */
     int          prnnum;   /**< \brief 1 Bytes - Satellite PRN number minus 1 (0 to 31) */
     int          res;      /**< \brief 1 Bytes - Reserved character */
     unsigned int checksum; /**< \brief 2 Bytes - The checksum is computed by breaking the   */
     /**<                        \brief         structure into 65 unsigned shorts, adding  */
     /**<                        \brief         together, and taking the least significant */
     /**<                        \brief         16 bits of the result                      */
     /** Total Bytes: 132 */
};

struct str_ashtech_xyz
{
     bool logging_parsed;
     FILE *csv;
     bool new_message;

     unsigned int rcvtime; 
     unsigned int numSats;
     unsigned int  checksum;    /* 1  XOR checksum of above data */

     struct str_xyz_data
     {
          unsigned int sv; /* 2  PRN of channel */
          double        x; /* 8  X coord of satellite (WGS-84) */
          double        y; /* 8  Y coord of satellite (WGS-84) */
          double        z; /* 8  Z coord of satellite (WGS-84) */
          double    range; 
     } channel[MAX_ASHTECH_CHANNELS];
};

void ashtech_parse_mca(unsigned char *x, struct str_ashtech_mca *mca, int index);
/* void ashtech_print_mca_transaction(struct str_ashtech_mca *mca, int i, unsigned int rcvtime); */
void ashtech_print_mca(struct str_ashtech_mca *mca, int i, unsigned int rcvtime);
void ashtech_print_mca_all(struct str_ashtech_mca *mca, unsigned int rcvtime);

void ashtech_parse_mis(unsigned char *x, struct str_ashtech_mis *mis);
void ashtech_print_mis(struct str_ashtech_mis *mis);

void ashtech_parse_mpc(unsigned char *x, struct str_ashtech_mpc *mpc, int index);
void ashtech_print_mpc(struct str_ashtech_mpc *mpc, int i, unsigned int rcvtime);
void ashtech_print_mpc_all(struct str_ashtech_mpc *mpc, unsigned int rcvtime);

void ashtech_parse_pbn(unsigned char *x, struct str_ashtech_pbn *pbn);
void ashtech_print_pbn(struct str_ashtech_pbn *pbn);

void ashtech_parse_sal(unsigned char *x, struct str_ashtech_sal *sal);
void ashtech_print_sal(struct str_ashtech_sal *sal, unsigned int rcvtime);
int  ashtech_store_sal(struct str_ashtech_sal *sal, GDBM_FILE dbf);
void ashtech_print_sal_stdio(struct str_ashtech_sal *sal, unsigned int rcvtime);

void ashtech_parse_snv(unsigned char *x, struct str_ashtech_snv *snv);
void ashtech_print_snv(struct str_ashtech_snv *snv);
int  ashtech_store_snv(struct str_ashtech_snv *snv, GDBM_FILE dbf);
void ashtech_zero_snv(struct str_ashtech_snv *snv);

unsigned int ashtech_xyz_size(unsigned char *x);
void ashtech_parse_xyz(unsigned char *x, struct str_ashtech_xyz *xyz);
void ashtech_print_xyz(struct str_ashtech_xyz *xyz);

int  ashtech_checksum_8(unsigned char *in, int size, FILE *console);
int  ashtech_checksum_16(unsigned char *in, int size, FILE *console);

#endif

