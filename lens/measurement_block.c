#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../gps.h"
#include "measurement_block.h"

#define NUMBER_OF_SIGMA_BINS 15

void verify_pr_corr(double PRsca1, double PRsca2, double PRsca3, double PRsca4,
                   double K_B_PR, double Sigma,
                   double PRcorr_prev, double PRcorr_threshold, 
                   struct str_measurement_block *measurement_block)
{
     /* Determine the number of reference stations with valid Pseudorange corrections */
     /* Output Variables */
     double B1 = 0;
     double B2 = 0;
     double B3 = 0;
     double B4 = 0;
     int number_of_rs = 0;
     double RRC = 0;
     double PRcorr_out = 0;
     bool PRcorr_Mod = false;
     bool B_Fail = false;
     bool Failed_PRC = false;
     bool Failed_RRC = false;

     int count = 0;
     double B_Value_Limit = 0;
     double PRC_Avg = 0;
     double PRsca1_new = 0;
     double PRsca2_new = 0;
     double PRsca3_new = 0;
     double PRsca4_new = 0;
     bool B1_Valid = false;
     bool B2_Valid = false;
     bool B3_Valid = false;
     bool B4_Valid = false;
     int RS1 = 0;
     int RS2 = 0;
     int RS3 = 0;
     int RS4 = 0;


     if(PRsca1 != 0)
     {
          count++;
     }
     if(PRsca2 != 0)
     {
          count++;
     }
     if(PRsca3 != 0)
     {
          count++;
     }
     if(PRsca4 != 0)
     {
          count++;
     }

     number_of_rs = count;

     /* printf("Number of RS (1): %d\n",number_of_rs); */

     if(number_of_rs < 3)
     {
          /* Must have 3 Reference Stations, Leave output values as initial conditions */
          return;
     } else {
          /* Calculate the B-Value Limit */
          B_Value_Limit = (K_B_PR * Sigma)/(sqrt(number_of_rs-1));
          /* Calculate Average PRC */
          PRC_Avg = (PRsca1 + PRsca2 + PRsca3 + PRsca4)/number_of_rs;

          /* printf("PRsca1: B_Value_Limit: %f, K_B_PR: %f, Sigma: %f\n",B_Value_Limit, K_B_PR, Sigma); */
          /* printf("PRsca1: PRC_Avg: %f\n",PRC_Avg); */
          /* Calculate B-Values */
          /* B1 Calculation */
          if(PRsca1 != 0)
          {
               B1 = fabs(PRC_Avg - (PRsca2 + PRsca3 + PRsca4)/(number_of_rs-1));
               /* printf("B1: %f\n",B1); */
               if(B1 > B_Value_Limit)
               {
                    B1 = 0;
                    PRsca1_new = 0 ;
                    B1_Valid = false;
                    RS1 = 0;
               } else {
                    PRsca1_new = PRsca1;
                    B1_Valid = true;
                    RS1=1;
               }
          } else {
               B1 = 0;
               B1_Valid = true; /* This is set true for future logic */
               RS1 = 0;
          }

          /* B2 Calculation */
          if(PRsca2 != 0)
          {
               B2 = fabs(PRC_Avg - (PRsca1 + PRsca3 + PRsca4)/(number_of_rs-1));
               if(B2 > B_Value_Limit)
               {
                    B2 = 0;
                    PRsca2_new = 0 ;
                    B2_Valid = false;
                    RS2 = 0;
               } else {
                    PRsca2_new = PRsca2;
                    B2_Valid = true;
                    RS2=1;
               }
          } else {
               B2 = 0;
               B2_Valid = true;/* This is set true for future logic */
               RS2 = 0;
          }

          /* B3 Calculation */
          if(PRsca3 != 0)
          {
               B3 = fabs(PRC_Avg - (PRsca1 + PRsca2 + PRsca4)/(number_of_rs-1));
               if(B3 > B_Value_Limit)
               {
                    B3 = 0;
                    PRsca3_new = 0 ;
                    B3_Valid = false;
                    RS3 = 0;
               } else {
                    PRsca3_new = PRsca3;
                    B3_Valid = true;
                    RS3=1;
               }
          } else {
               B3 = 0;
               B3_Valid = true;/* This is set true for future logic */
               RS3 = 0;
          }

          /* B4 Calculation */
          if(PRsca4 != 0)
          {
               B4 = fabs(PRC_Avg - (PRsca1 + PRsca2 + PRsca3)/(number_of_rs-1));
               if(B4 > B_Value_Limit)
               {
                    B4 = 0;
                    PRsca4_new = 0 ;
                    B4_Valid = false;
                    RS4 = 0;
               } else {
                    PRsca4_new = PRsca4;
                    B4_Valid = true;
                    RS4=1;
               }
          } else {
               B4 = 0;
               B4_Valid = true;/* This is set true for future logic */
               RS4 = 0;
          }
          /* Calculate PRcorr_out & the Number of Reference Stations (overwrite previous value) */
          number_of_rs = RS1 + RS2 + RS3 + RS4;
          /* printf("Number of RS (2): %d\n",number_of_rs); */
          if(number_of_rs < 3)
          {
               B_Fail = true;
          } else {
               B_Fail = false;
          }
          /* Check for any B-Value Failurers (Make Sure there are at least 3 B-Values) */
          /* printf("B1_Valid: %d, B2_Valid: %d, B3_Valid: %d, B4_Valid: %d\n",B1_Valid, B2_Valid, B3_Valid, B4_Valid); */
          if((B1_Valid && B2_Valid && B3_Valid && B4_Valid) == true)
          {
               /* This is the case where there are no B-Value Failures */
               PRcorr_out = PRC_Avg;
          } else {
               /* This is the case where there is at least one B-Value Failure */
               PRcorr_out = (PRsca1_new + PRsca2_new + PRsca3_new + PRsca4_new)/(number_of_rs);
          }
          /* Zero out PRC and RRC if B-Value test failed */
          if(B_Fail == true)
          {
               PRcorr_out = 0;
               RRC = 0;
          } else {
               /* printf("B_Fail: %d\n",B_Fail); */
               /* printf("PRcorr_out: %f\n", PRcorr_out); */
               /* PRC Test */
               PRcorr_prev = PRcorr_out; /* Done for testing */
               if((fabs(PRcorr_out - PRcorr_prev)) > PRcorr_threshold)
               {
                    Failed_PRC = true;
                    PRcorr_out = 0;
               } else {
                    Failed_PRC = false;
               }
               /* RRC Test */
               RRC = (PRcorr_out - PRcorr_prev)/0.5;
               if((fabs(RRC)) > 3.4) /* What it 3.4???? */
               {
                    Failed_RRC = true;
                    PRcorr_out = 0;
               } else {
                    Failed_RRC = false;
               }
          }
          /* printf("Failed_PRC: %d\n", Failed_PRC); */
          /* printf("Failed_RRC: %d\n", Failed_RRC); */
          /* Check to see if the PRC has been modified */
          if(((PRcorr_out != PRC_Avg) || Failed_RRC == true || Failed_PRC == true || B_Fail == true))
          {
               PRcorr_Mod = true;
          } else {
               PRcorr_Mod = false;
          }
     }
     /* printf("PRcorr_out (Final): %f\n", PRcorr_out); */
     measurement_block->prc = PRcorr_out;
     measurement_block->rrc = RRC;
     measurement_block->number_of_rs = number_of_rs;
     measurement_block->B1  = B1;
     measurement_block->B2  = B2;
     measurement_block->B3  = B3;
     measurement_block->B4  = B4;
}

double find_sigma(double elevation)
{
     /* Set initial elevation bin value to 0 */
     int elevation_bin = 0;
     int i = 0;

     float sigma_table[NUMBER_OF_SIGMA_BINS][2] =
     {
          {6, 0.08},
          {7, 0.078},
          {8, 0.076},
          {10, 0.072},
          {15, 0.07},
          {20, 0.067},
          {25, 0.063},
          {30, 0.06},
          {35, 0.055},
          {40, 0.05},
          {45, 0.045},
          {50, 0.042},
          {60, 0.04},
          {70, 0.035},
          {90, 0.03}
     };

     for(i = 0; i < NUMBER_OF_SIGMA_BINS; i++)
     {
          if(elevation < 5)
          {
               break;
          }
          if((elevation < sigma_table[i][0]) && (elevation > elevation_bin))
          {
               break;
          } else {
               elevation_bin = sigma_table[i][0];
          }
     }
     return sigma_table[i][1];
}

/*
     for(prn_idx = 0; prn_idx < NUMBER_OF_GPS_SATELLITES; prn_idx++)
     {
          if(prsc_is_set[prn_idx])
          {
               printf("PRsc: %d, %d, ", master_time_current->gps.rcvtime, prn_idx + 1);
               for(rr_idx = 0; rr_idx < limits->NUMBER_OF_REFERENCE_STATIONS*2 - 1; rr_idx++)
               {
                         printf("%f, ", prsc_filter[prn_idx][rr_idx].prsc);
               }
               printf("%f\n", prsc_filter[prn_idx][rr_idx].prsc);
          }
          prsc_is_set[prn_idx] = false;
     }
*/

void build_measurement_block(double PRsca[MAX_NUMBER_OF_GG12S][NUMBER_OF_GPS_SATELLITES],
                             struct str_satellite_almanac_ephemeris_position all_satellites_aep_information[])
{
     int count = 0;
     int i = 0;
     double sigma = 0;
     double PRcorr_prev = 0;
     double K_B_PR = 5.5;
     double PRcorr_threshold = 100;
     struct str_satellite_position *xyz = NULL;
     struct str_ashtech_snv *snv = NULL;
     unsigned int rcvtime = 0;

     struct str_measurement_block measurement_block[NUMBER_OF_GPS_SATELLITES];

     struct str_status
     {
          int number_prc;
          double tow;
          bool available;
     } status;

     memset(measurement_block, 0x00, sizeof(struct str_measurement_block) * NUMBER_OF_GPS_SATELLITES);

     for(i = 0; i<NUMBER_OF_GPS_SATELLITES; i++)
     {
          xyz = &all_satellites_aep_information[i].xyzae;
          snv = &all_satellites_aep_information[i].snv;

          sigma = find_sigma(xyz->elevation);

          /* printf("BMB: PRsca: %0.24f, %0.24f, %0.24f, %0.24f\n", PRsca[0][i], PRsca[1][i], PRsca[2][i], PRsca[3][i]); */
          /* printf("Sigma: %f\n", sigma); */
          verify_pr_corr(PRsca[0][i], PRsca[1][i], PRsca[2][i], PRsca[3][i],
                         K_B_PR, sigma, PRcorr_prev, PRcorr_threshold, &measurement_block[i]);

          if(measurement_block[i].prc != 0)
          {
               count += 1;
               measurement_block[i].prn = i+1;
               measurement_block[i].iod = snv->aode;
               measurement_block[i].sigma = sigma;
               printf("PRN: %d, PRC: %0.24f, RRC: %f, SIGMA: %f, PRSCA: %f, %f, %f, %f, B-Values: %f, %f, %f, %f\n",
                         measurement_block[i].prn,
                         measurement_block[i].prc, measurement_block[i].rrc, measurement_block[i].sigma,
                         PRsca[0][i], PRsca[1][i], PRsca[2][i], PRsca[3][i], 
                         measurement_block[i].B1, measurement_block[i].B2, 
                         measurement_block[i].B3, measurement_block[i].B4);
          }
     }
     status.number_prc = count;
     status.tow = rcvtime/1000;
     if(count < 4)
     {
          status.available = false;
     } else {
          status.available = true;
     }
}
