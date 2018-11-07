#include "../laas.h"
#include "../gps.h"

struct str_measurement_block
{
     int prn;
     int iod;
     double prc;
     double rrc;
     double sigma;
     double B1;
     double B2;
     double B3;
     double B4;
     int number_of_rs;
};

void verify_pr_corr(double PRsca1, double PRsca2, double PRsca3, double PRsca4,
                   double K_B_PR, double Sigma,
                   double PRcorr_prev, double PRcorr_threshold, 
                   struct str_measurement_block *measurement_block);

/*
void verify_pr_corr(double PRsca1, double PRsca2, double PRsca3, double PRsca4,
                    double K_B_PR, double Sigma, double PRcorr_prev, double PRcorr_threshold,
                    struct str_measurement_block *measurement_block);
*/
                   
double find_sigma(double Elevation);

void build_measurement_block(double prsc_array_full[MAX_NUMBER_OF_GG12S][NUMBER_OF_GPS_SATELLITES],
          struct str_satellite_almanac_ephemeris_position all_satellites_aep_information[]);
