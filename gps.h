#ifndef GPS_H
#define GPS_H 1

#include "time_struct.h"
#include "ashtech/ashtech.h"

#define NUMBER_OF_GPS_SATELLITES 32

#define EPHEMERIS_ARCHIVE_SIZE 26

#define MEDE 3500
#define PRr_NOMINAL 0.075

/** \brief Satellite Position. */
struct str_satellite_position
{
     /* Values calculated by satellite_position() */

     /** \brief Satellite Pseudo Random Number (PRN) */
     unsigned int svprn;

     /** \brief GPS Receive Time in Milliseconds */
     struct str_time time_received;

     /* ECEF X, Y, Z */
     /** \brief ECEF X Coordinate of the Satellite */
     double x;  
     /** \brief ECEF Y Coordinate of the Satellite */
     double y;
     /** \brief ECEF Z Coordinate of the Satellite */
     double z; 

     /* Values calculated by azimuth_elevation() */
     /** \brief Azimuth Position of Satellite to Measurement Reference Point in degrees */
     double azimuth;
     /** \brief Elevation Position of Satellite to Measurement Reference Point in degrees */
     double elevation;   

     /** \brief Range from Satellite to the Measurement Reference Point */
     double range;  

     /** \brief Delta Tr */
     double delta_tr;  

     /** \brief P Value */
     double p_value;  

     /** \brief Source Availability Duration*/
     double sad;  
};

/* FILE *csv; */ /* CSV file for Satellite X, Y, Z, Range, Azimuth, and Elevation */ 
/* almanac_ephemeris_position - Almanac / Ephemeris / Position */
struct str_satellite_almanac_ephemeris_position
{
     struct str_ashtech_sal          sal;
     struct str_ashtech_snv          snv;
     struct str_satellite_position xyzae;
     /* Ephemeris has been validated and is in a two minute hold */
     struct str_ashtech_snv     snv_hold;
     int                        previous_aode;
};

#endif

