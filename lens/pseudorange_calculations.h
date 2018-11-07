#include "../ashtech/ashtech.h"
#include "../gps.h"
#include "../laas.h"

struct str_prsc_filter
{
     unsigned int goodbad; /* From MCA */
     unsigned int snr;           /* From MCA ireg*/
     double raw_range;     /* From MCA */
     double phase;

     double N;
     double prs;
     double prsc;

     double phase_previous;
     double prs_previous;
     unsigned int snr_previous;           /* From MCA ireg*/
};

int Select_RS(double PRsc_1[], int PRsc_1_i, double PRsc_2[], int PRsc_2_i, int Active);

void Calc_PR_s(struct str_prsc_filter *prsc_filter);

void Calc_PR_sc(struct str_ashtech_snv *snv,
                struct str_satellite_position *satloc,
                struct str_location *antloc,
                struct str_prsc_filter *prsc_filter);

void Calc_PR_sca(double PRscArray[MAX_NUMBER_OF_REFERENCE_RECEIVERS][NUMBER_OF_GPS_SATELLITES],
                    int Active[MAX_NUMBER_OF_REFERENCE_STATIONS]);
