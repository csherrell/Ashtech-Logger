#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../mathematical_constants.h"
#include "../laas.h"
#include "../gps.h"
#include "../satellite_position.h"
#include "../ashtech/ashtech.h"
#include "pseudorange_calculations.h"

#define SNR_Min  0
#define PRsc_Max 100 /* meters */
#define N_Limit  10
#define Phase_Change_Max 0.1  /* 10% phase change */

const double nominal_carrier_frequency =  1575420000.0;

double PRsc_Limit = 100;

void Calc_PR_s(struct str_prsc_filter *mca_filter)
{
     double Phase_diff = 0;
     const bool SNR_Valid = true;  /* Future Use */
     double N = 0.0;
     /* Calculate PRs */
     if(SNR_Valid)     
     {
          if((mca_filter->prs != 0) && (mca_filter->snr != 0)) /* <- Doesn't seem like this goes here */
          {
               /* Update Filter number of samples */
               mca_filter->N = (mca_filter->N < 200.0) ? mca_filter->N + 1.0 : 200.0;
               N = mca_filter->N;
               Phase_diff = mca_filter->phase - mca_filter->phase_previous;
               if(fabs(Phase_diff / mca_filter->phase_previous) <= 0.1)
               {
                    mca_filter->prs_previous = mca_filter->prs;
                    mca_filter->prs = (
                                 (mca_filter->raw_range / (double) N) +
                                    ((mca_filter->prs + (Phase_diff/nominal_carrier_frequency)) * 
                                      ((N - 1) / (double) N))
                              );
               } else {
                    mca_filter->prs = mca_filter->raw_range;
                    mca_filter->N = 1; 
               }
          } else {
               mca_filter->prs = mca_filter->raw_range;
               mca_filter->N = 1; 
          }
     } else {
          /* Zero out all the mca values */
          memset(mca_filter, 0x00, sizeof(struct str_prsc_filter));
     }
     mca_filter->phase_previous = mca_filter->phase;
}

void Calc_PR_sc(struct str_ashtech_snv *snv,
                struct str_satellite_position *satloc,
                struct str_location *antloc,
                struct str_prsc_filter *mca_filter)
{
     double range = 0;
     double PRsFinal = 0;
     double delta_t = 0;
     bool sector_mask = false;     /* Future Use */
     const bool SNR_Valid = true;  /* Future Use */

     if(mca_filter->snr != 0 && SNR_Valid == true)
     {
          /* Check to see if the Azimuth/Elevation is in a masked out region */
          if(satloc->elevation >= 5.0 && sector_mask == false)
          {
               /* Calculate the range from the antenna location to the satellites */
               /* Different from the centriod */
               range = get_satellite_range(satloc,antloc);
               delta_t = (((satloc->time_received.tm_gps.rcvtime/1000.0) - range/C) - snv->toc);
               PRsFinal = C * (mca_filter->prs + snv->af0 + (snv->af1 * delta_t) + (snv->af2 * pow(delta_t,2)) - snv->tgd + (satloc->delta_tr));
               mca_filter->prsc = range - PRsFinal;    /* meters */

               if(fabs(mca_filter->prsc) > PRsc_Limit) 
               {
                    mca_filter->prsc = 0;
               }
          } else { mca_filter->prsc = 0; }
     } else { mca_filter->prsc = 0; }
}

int
rs_count(double d_array[])
{
     int i = 0;
     int count = 0;
     for(i=0;i<NUMBER_OF_GPS_SATELLITES;i++)
     {
          count = (d_array[i] != 0) ? count + 1 : count;
     }
     return count;
}

int
Select_RS(double PRsc_1[], int PRsc_1_i, double PRsc_2[], int PRsc_2_i, int Active)
{
     int num_PRsc_1 = rs_count(PRsc_1);
     int num_PRsc_2 = rs_count(PRsc_2);
     if(num_PRsc_1 == num_PRsc_2)
     {
          return Active;
     } else {
          if(num_PRsc_1 > num_PRsc_2) 
          {
              return PRsc_1_i;
          } else {
              return PRsc_2_i;
          }
     }
}

int total_flags(int RS_Config) 
{
     switch(RS_Config)
     {
          case 0x01:
          case 0x02:
          case 0x04:
          case 0x08:
               return 1; break;
          case 0x03:
          case 0x05:
          case 0x06:
          case 0x09:
          case 0x0A:
          case 0x0C:
               return 2; break;
          case 0x07:
          case 0x0B:
          case 0x0D:
          case 0x0E:
               return 3; break;
          case 0x0F:
               return 4; break;
          default:
               return 0; break;
     }
}

int find_max(int Array[], int length)
{
     int i = 0;
     int max_value = 0;
     int max_index = 0;
     for(i = 0; i<length; i++)
     {
          if(Array[i] > max_value)
          {
               max_value = Array[i];
               max_index = i;
          }
     }
     return max_index;
}

void Calc_PR_sca(double PRscArray[MAX_NUMBER_OF_REFERENCE_RECEIVERS][NUMBER_OF_GPS_SATELLITES],
                    int Active[MAX_NUMBER_OF_REFERENCE_STATIONS])
{
     static unsigned int RS_Config = 0x0F;
     int Excluded_PRN = 0;
     #define MAX_NUMBER_OF_REFERENCE_RECEIVER_FLAGS 6
     #define NUMBER_OF_REFERENCE_RECEIVER_CASES 5

     const int PR_sc1 = 2;
     const int PR_sc2 = 3;
     const int PR_sc3 = 4;
     const int PR_sc4 = 5;

     bool reference_receiver_flags[NUMBER_OF_GPS_SATELLITES][MAX_NUMBER_OF_REFERENCE_RECEIVER_FLAGS];
     bool PR_LM[NUMBER_OF_GPS_SATELLITES];
     int i = 0;
     bool NC_Array[NUMBER_OF_GPS_SATELLITES][NUMBER_OF_REFERENCE_RECEIVER_CASES];
     int  NC[NUMBER_OF_REFERENCE_RECEIVER_CASES];
     int  RS_Case_Index = 0;
     int  Max_Nc_Index = 0;
     int  Max_Nc = 0;
     int  RS_15_Nc = 0;
     int  Final_Index = 0;

     double Sum_PRsc1 = 0;
     double Sum_PRsc2 = 0;
     double Sum_PRsc3 = 0;
     double Sum_PRsc4 = 0;

     int count = 0;
     int sum   = 0;
     struct str_status
     {
          bool Available;
          bool noWarning;
          int Num_RS;
          unsigned int RS_Config;
          int N_CE;
          double clock_error[4];
          double avg_clock_error;
     } status;

     int rr_idx = 0;
     int prn_idx = 0;

     /*printf("Calc_PR_sca()\n"); */
     /* printf("Active: %d, %d, %d, %d\n", Active[0], Active[1], Active[2], Active[3]); */
/*
     for(rr_idx = 0; rr_idx < 8; rr_idx++)
     {
          for(prn_idx = 0; prn_idx < NUMBER_OF_GPS_SATELLITES; prn_idx++)
          {
               if (PRscArray[rr_idx][prn_idx] != 0)
               {
                    printf("PRSC2: RR: %d, PRN: %d\n", rr_idx, prn_idx); 
               }
          }
     }
*/

     memset(&status, 0x00, sizeof(struct str_status));

     if(RS_Config != 0x0F && /* All Reference Stations are enabled   */
        RS_Config != 0x0E && /* Only Reference Station 1 is disabled */
        RS_Config != 0x0D && /* Only Reference Station 2 is disabled */
        RS_Config != 0x0B && /* Only Reference Station 3 is disabled */
        RS_Config != 0x07)   /* Only Reference Station 4 is disabled */
     {
          /* Case 6 - Not case 7, 11, 13, 14, or 15 */
          /* We do not have three reference stations availiable */
          status.Available = false;
          status.N_CE = 0;
          status.RS_Config = RS_Config;
          status.Num_RS = total_flags(RS_Config);
          return;
     } else {
          memset(reference_receiver_flags, false, sizeof(bool) * NUMBER_OF_GPS_SATELLITES * MAX_NUMBER_OF_REFERENCE_RECEIVER_FLAGS);
          memset(PR_LM, true, sizeof(bool) * NUMBER_OF_GPS_SATELLITES);
          memset(NC_Array, false, sizeof(bool) * NUMBER_OF_GPS_SATELLITES * NUMBER_OF_REFERENCE_RECEIVER_CASES);
          memset(NC, 0x00, sizeof(int) * NUMBER_OF_REFERENCE_RECEIVER_CASES);
          switch(RS_Config)
          {
               case 0x07: RS_Case_Index = 0;
                          break;
               case 0x0B: RS_Case_Index = 1;
                          break;
               case 0x0D: RS_Case_Index = 2;
                          break;
               case 0x0E: RS_Case_Index = 3;
                          break;
               default:   RS_Case_Index = 4;
                          break;
          }

          for(i=0;i<NUMBER_OF_GPS_SATELLITES;i++)
          {
               if(PR_LM[i] != false) { reference_receiver_flags[i][0] = true; }
               if(Excluded_PRN != i+1) { reference_receiver_flags[i][1] = true; }
               /* printf("PRscArray: %f\n", PRscArray[Active[0]][i]); */
               if(PRscArray[Active[0]][i] != 0) { reference_receiver_flags[i][PR_sc1] = true; }
               if(PRscArray[Active[1]][i] != 0) { reference_receiver_flags[i][PR_sc2] = true; }
               if(PRscArray[Active[2]][i] != 0) { reference_receiver_flags[i][PR_sc3] = true; }
               if(PRscArray[Active[3]][i] != 0) { reference_receiver_flags[i][PR_sc4] = true; }
               /* printf("FLAGS: %d, %d, %d, %d, %d, %d\n", reference_receiver_flags[i][0], reference_receiver_flags[i][1],reference_receiver_flags[i][2],reference_receiver_flags[i][3],reference_receiver_flags[i][4],reference_receiver_flags[i][5]); */
               NC_Array[i][0] = reference_receiver_flags[i][0] &&
                    reference_receiver_flags[i][1] &&
                    reference_receiver_flags[i][PR_sc1] &&
                    reference_receiver_flags[i][PR_sc2] &&
                    reference_receiver_flags[i][PR_sc3];
               NC_Array[i][1] = reference_receiver_flags[i][0] &&
                    reference_receiver_flags[i][1] &&
                    reference_receiver_flags[i][PR_sc1] &&
                    reference_receiver_flags[i][PR_sc2] &&
                    reference_receiver_flags[i][PR_sc4];
               NC_Array[i][2] = reference_receiver_flags[i][0] &&
                    reference_receiver_flags[i][1] &&
                    reference_receiver_flags[i][PR_sc1] &&
                    reference_receiver_flags[i][PR_sc3] &&
                    reference_receiver_flags[i][PR_sc4];
               NC_Array[i][3] = reference_receiver_flags[i][0] &&
                    reference_receiver_flags[i][1] &&
                    reference_receiver_flags[i][PR_sc2] &&
                    reference_receiver_flags[i][PR_sc3] &&
                    reference_receiver_flags[i][PR_sc4];
               NC_Array[i][4] = reference_receiver_flags[i][0] &&
                    reference_receiver_flags[i][1] &&
                    reference_receiver_flags[i][PR_sc1] &&
                    reference_receiver_flags[i][PR_sc2] &&
                    reference_receiver_flags[i][PR_sc3] &&
                    reference_receiver_flags[i][PR_sc4];
               NC[0] += NC_Array[i][0];
               NC[1] += NC_Array[i][1];
               NC[2] += NC_Array[i][2];
               NC[3] += NC_Array[i][3];
               NC[4] += NC_Array[i][4];
               /* printf("NC_Array[%d][4]: %d\n", i, NC_Array[i][4]); */
          }
     }


     if(RS_Config != 0x0F)
     {
          status.Num_RS = 3;
          status.N_CE = NC[RS_Case_Index];
          status.RS_Config = RS_Config;
     } else {
          Max_Nc_Index = find_max(NC, NUMBER_OF_REFERENCE_RECEIVER_CASES);
          Max_Nc = NC[Max_Nc_Index];
          RS_15_Nc = NC[NUMBER_OF_REFERENCE_RECEIVER_CASES-1];
          if(((Max_Nc - RS_15_Nc) > 1) || (RS_15_Nc < 4))
          {
               status.Num_RS = 3;
               status.RS_Config = RS_Config;
               status.N_CE = Max_Nc;
               Final_Index = Max_Nc_Index;
          } else {
               status.Num_RS = 4;
               Final_Index = 4;
               status.N_CE = RS_15_Nc;
          }
     }


     if(status.N_CE >= 4)
     {
          status.Available = true;
     } else {
          status.Available = false;
          return;
     }

     /* printf("NC: %d, %d, %d, %d, %d\n", NC[0], NC[1], NC[2], NC[3], NC[4]); */
     /* printf("N_CE: %d\n", status.N_CE); */
     /* Calculate Clock Errors */

     if((RS_Config & 0x08) > 0)
     {
          for(i=0;i<NUMBER_OF_GPS_SATELLITES;i++)
          {
               if(NC_Array[i][Final_Index] == true)
               {
                    Sum_PRsc1 += PRscArray[Active[0]][i];
               }
               status.clock_error[0] = Sum_PRsc1 / (double) status.N_CE;
          }
     } else {
          status.clock_error[0] = 0;
     }
     /* printf("Clock Error 0: %f\n", status.clock_error[0]); */

     if((RS_Config & 0x04) > 0)
     {
          for(i=0;i<NUMBER_OF_GPS_SATELLITES;i++)
          {
               if(NC_Array[i][Final_Index] == true)
               {
                    Sum_PRsc2 += PRscArray[Active[1]][i];
               }
               status.clock_error[1] = Sum_PRsc2 / (double) status.N_CE;
          }
     } else {
          status.clock_error[1] = 0;
     }

     if((RS_Config & 0x02) > 0)
     {
          for(i=0;i<NUMBER_OF_GPS_SATELLITES;i++)
          {
               if(NC_Array[i][Final_Index] == true)
               {
                    Sum_PRsc3 += PRscArray[Active[2]][i];
               }
               status.clock_error[2] = Sum_PRsc3 / (double) status.N_CE;
          }
     } else {
          status.clock_error[2] = 0;
     }

     if((RS_Config & 0x01) > 0)
     {
          for(i=0;i<NUMBER_OF_GPS_SATELLITES;i++)
          {
               if(NC_Array[i][Final_Index] == true)
               {
                    Sum_PRsc4 += PRscArray[Active[3]][i];
               }
               status.clock_error[3] = Sum_PRsc4 / (double) status.N_CE;
          }
     } else {
          status.clock_error[3] = 0;
     }

     count = 0;
     sum   = 0;
     for(i=0;i<4;i++)
     {
          if(status.clock_error[i] != 0)
          {
               count++;
               sum += status.clock_error[i];
          }
     }
     status.avg_clock_error = (double) sum / (double) count;

     for(i=0;i<NUMBER_OF_GPS_SATELLITES;i++)
     {
          /* printf("PRsca: PRN: %02d ->", i); */
          if(PRscArray[Active[0]][i] != 0)
          {
               PRscArray[0][i] = PRscArray[Active[0]][i] - status.clock_error[0];
               /* printf("1: %f ", PRscArray[Active[0]][i]); */
          } else {
               PRscArray[0][i] = 0;
          }
          if(PRscArray[Active[1]][i] != 0)
          {
               PRscArray[1][i] = PRscArray[Active[1]][i] - status.clock_error[1];
               /* printf("2: %f ", PRscArray[Active[1]][i]); */
          } else {
               PRscArray[1][i] = 0;
          }
          if(PRscArray[Active[2]][i] != 0)
          {
               PRscArray[2][i] = PRscArray[Active[2]][i] - status.clock_error[2];
               /* printf("3: %f ", PRscArray[Active[2]][i]); */
          } else {
               PRscArray[2][i] = 0;
          }
          if(PRscArray[Active[3]][i] != 0)
          {
               PRscArray[3][i] = PRscArray[Active[3]][i] - status.clock_error[3];
               /* printf("4: %f ", PRscArray[Active[3]][i]); */
          } else {
               PRscArray[3][i] = 0;
          }
          /* printf("\n"); */
     }

     if(status.N_CE == 4 || status.Num_RS == 3)
     {
          status.noWarning = false;
     } else {
          status.noWarning = true;
     }
}

void test()
{
     struct str_ashtech_snv snv;
     struct str_location antenna;
     struct str_satellite_position satellite;
     struct str_prsc_filter mca_pr;
     double PRscArray[MAX_NUMBER_OF_REFERENCE_RECEIVERS][NUMBER_OF_GPS_SATELLITES];
     int Active[MAX_NUMBER_OF_REFERENCE_STATIONS];
     Calc_PR_sc(&snv, &satellite, &antenna, &mca_pr);
     Active[0] = Select_RS(PRscArray[0], 0, PRscArray[1], 1, Active[0]);
}
