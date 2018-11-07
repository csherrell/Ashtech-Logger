#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "satellite_position.h"
#include "mathematical_constants.h"
#include "laas.h"
#include "gps.h"

#define F (-2*sqrt(MU)/pow(C,2.0))

void debug_print_snv(unsigned int rcvtime, struct str_ashtech_snv *snv)
{
     printf("rcvtime  = %d\n", rcvtime);
     printf("prn      = %d\n", snv->prnnum);
     printf("wn       = %d\n", snv->wn);
     printf("tow      = %d\n", snv->tow);
     printf("tgd      = %24.24f\n", snv->tgd);
     printf("aodc     = %d\n", snv->aodc);
     printf("toc      = %d\n", snv->toc);
     printf("af2      = %24.24f\n", snv->af2);
     printf("af1      = %24.24f\n", snv->af1);
     printf("af0      = %24.24f\n", snv->af0);
     printf("aode     = %d\n", snv->aode);
     printf("deltan   = %24.24f\n", snv->deltan);
     printf("m0       = %24.24f\n", snv->m0);
     printf("e        = %24.24f\n", snv->e);
     printf("roota    = %24.24f\n", snv->roota);
     printf("toe      = %d\n", snv->toe);
     printf("cic      = %24.24f\n", snv->cic);
     printf("crc      = %24.24f\n", snv->crc);
     printf("cis      = %24.24f\n", snv->cis);
     printf("crs      = %24.24f\n", snv->crs);
     printf("cuc      = %24.24f\n", snv->cuc);
     printf("cus      = %24.24f\n", snv->cus);
     printf("omega0   = %24.24f\n", snv->omega0);
     printf("omega    = %24.24f\n", snv->omega);
     printf("i0       = %24.24f\n", snv->i0);
     printf("omegadot = %24.24f\n", snv->omegadot);
     printf("idot     = %24.24f\n", snv->idot);
     printf("accuracy = %d\n", snv->accuracy);
     printf("health   = %d\n", snv->health);
     printf("fit      = %d\n", snv->fit);
     printf("res      = %d\n", snv->res);
}

void debug_print_sal(unsigned int rcvtime, struct str_ashtech_sal *sal)
{
     printf("rcvtime  = %d\n", rcvtime);
     printf("prn      = %d\n", sal->prn);
     printf("health   = %d\n", sal->health);
     printf("e        = %f\n", sal->e);
     printf("toa      = %d\n", sal->toa);
     printf("i0       = %f\n", sal->i0);
     printf("omegadot = %f\n", sal->omegadot);
     printf("roota    = %f\n", sal->roota);
     printf("omega0   = %f\n", sal->omega0);
     printf("omega    = %f\n", sal->omega);
     printf("m0       = %f\n", sal->m0);
     printf("af0      = %f\n", sal->af0);
     printf("af1      = %f\n", sal->af1);
     printf("wna      = %d\n", sal->wna);
     printf("wn       = %d\n", sal->wn);
     printf("tow      = %d\n", sal->tow);
}


double
get_satellite_distance(struct str_satellite_position *position1, struct str_satellite_position *position2)
{
     double delta_x = (position1->x - position2->x);
     double delta_y = (position1->y - position2->y);
     double delta_z = (position1->z - position2->z);

/*
     printf("\nSNV Position: X:%f, Y:%f, Z:%f\n", position1->x, position1->y, position1->z);
     printf("SAL Position: X:%f, Y:%f, Z:%f\n\n", position2->x, position2->y, position2->z);
*/

     return (double) sqrt((pow(delta_x,2.0) + pow(delta_y,2.0) + pow(delta_z,2.0)));
}

double
get_satellite_range(struct str_satellite_position *position1, struct str_location *position2)
{
     double delta_x = (position1->x - position2->x);
     double delta_y = (position1->y - position2->y);
     double delta_z = (position1->z - position2->z);

/*
     printf("Sat Position: X:%f, Y:%f, Z:%f\n", position1->x, position1->y, position1->z);
     printf("Ant Position: X:%f, Y:%f, Z:%f\n", position2->x, position2->y, position2->z);
*/

     return (double) sqrt((pow(delta_x,2.0) + pow(delta_y,2.0) + pow(delta_z,2.0)));
}

void
satellite_position(
          void *ashtech_variable,
          enum  ashtech_type  t,
          struct str_location *location,
          double PRr,
          struct str_time *master_time,
          struct str_satellite_position *xyzae) /* Satellite Position Return Value */
{
     struct str_ashtech_snv *snv = NULL;
     struct str_ashtech_sal *sal = NULL;
     double rcvtime = master_time->tm_gps.rcvtime / 1000.0;

     double transmit_time = 0;
     double Range_err = 0;
     double Range_old = 0;

     int j = 0;
     int i = 0;
     int prn = 0;

     double deltan   = 0;
     double m0       = 0;
     double e        = 0;
     double roota    = 0;
     double toe      = 0;
     double cic      = 0;
     double crc      = 0;
     double cis      = 0;
     double crs      = 0;
     double cuc      = 0;
     double cus      = 0;
     double omega0   = 0;
     double omega    = 0;
     double i0       = 0;
     double omegadot = 0;
     double idot     = 0;

     double Mk = 0;
     double Mkerror = 0;
     double Ek = 0;
     double vk = 0;
     double phik = 0;

     double delta_ik = 0;
     double delta_rk = 0;
     double delta_uk = 0;

     double ik = 0;
     double rk = 0;
     double uk = 0;

     double xk = 0;
     double yk = 0;
     double zk = 0;
     double xk1 = 0;
     double yk1 = 0;
     double xk2 = 0;
     double yk2 = 0;

     double omega_k = 0;

     double omega_cor = 0;
     double Range = 0;

     double time_test = 0;

     if(t == SNV)
     {
          snv = (struct str_ashtech_snv *) ashtech_variable;
          prn = snv->prnnum;
          deltan   = PI * snv->deltan;
          m0       = PI * snv->m0;
          e        =      snv->e;
          roota    =      snv->roota;
          toe      =      snv->toe;
          cic      =      snv->cic;
          crc      =      snv->crc;
          cis      =      snv->cis;
          crs      =      snv->crs;
          cuc      =      snv->cuc;
          cus      =      snv->cus;
          omega0   = PI * snv->omega0;
          omega    = PI * snv->omega;
          i0       = PI * snv->i0;
          omegadot = PI * snv->omegadot;
          idot     = PI * snv->idot;
          /* debug_print_snv(rcvtime, snv); */
     } else if (t == SAL) {
          sal = (struct str_ashtech_sal *) ashtech_variable;
          prn = sal->prn;
          m0       = PI * sal->m0;
          e        =      sal->e;
          roota    =      sal->roota;
          toe      =      sal->toa;
          omega0   = PI * sal->omega0;
          omega    = PI * sal->omega;
          i0       = PI * sal->i0;
          omegadot = PI * sal->omegadot;
          /* debug_print_sal(rcvtime, sal); */
     } else {
          printf("ERROR: Invalid type information\n");
     }

     /*transmit_time = (((double)rcvtime) / 1000.0) - PRr; */
     transmit_time = rcvtime - PRr;

     time_test = transmit_time - toe;
     if(time_test > 302400)
     {
          transmit_time = transmit_time - 604800; 
     } else if(time_test < (-302400)) {
          transmit_time = transmit_time + 604800; 
     }

     Range_err = 1;
     Range_old = PRr * C;

     j = 1;
     while (j <= 10)
     {

          Mk = m0 + (pow((MU / pow(roota,6.0)),0.5) + deltan) * (transmit_time - toe);
          Ek = Mk;
          Mkerror = Mk - Ek + sin(Ek) * e;
          i = 0; 
          while(fabs(Mkerror) > 1e-14)
          {
               Ek = Mk + sin(Ek) * e;
               Mkerror = Mk - Ek + sin(Ek) * e;
               ++i;
               if(i >= 20)
               {
                    break;
               }
          }

          vk = atan2(((sqrt(1-pow(e,2.0))*sin(Ek))),((cos(Ek)-e)));

          /* calculate phik */
          phik = vk+omega;

          /* Calculate delta uk, ik, and rk */
          delta_uk = cus*sin(2*phik)+cuc*cos(2*phik);
          delta_rk = crs*sin(2*phik)+crc*cos(2*phik);
          delta_ik = cis*sin(2*phik)+cic*cos(2*phik);

          /* Calculate uk, ik, and rk */
          uk = phik + delta_uk;
          rk = (pow(roota,2.0))*(1-e*cos(Ek))+ delta_rk;
          ik = i0 + idot*(transmit_time-toe) + delta_ik;

          /* Calculate xk'(xk1) and yk'(yk1) */
          xk1 = rk*cos(uk);
          yk1 = rk*sin(uk);

          /* Calculate omega_k */
          omega_k = omega0+(omegadot-OMEGADOTE)*(transmit_time-toe)-OMEGADOTE*toe;

          /* calculate xk2, yk2, Final zk */
          xk2 = xk1*cos(omega_k)-yk1*cos(ik)*sin(omega_k);
          yk2 = xk1*sin(omega_k)+yk1*cos(ik)*cos(omega_k);
          zk = yk1*sin(ik);

          /* Adjust xk and yk for Earth's Rotation and Calculate the Range */
          omega_cor = ((double)-1*OMEGADOTE*Range_old) / (double) C;
          xk = xk2*cos(omega_cor)-yk2*sin(omega_cor);
          yk = xk2*sin(omega_cor)+yk2*cos(omega_cor);
          Range = sqrt(pow((xk-location->x),2.0) + pow((yk-location->y),2.0) + pow((zk-location->z),2.0));

          /* Calculate Range Error and Store the current Range for next iteration. */
          /* If the Range Error is < 1e-16 end */
          Range_err = fabs((Range / (double) C) - (Range_old / (double) C));  
          if(Range_err >= 1e-16)
          {
               /* printf("In If: %24.24f\n", Range_err); */
               Range_old = Range;
               /* Modify the estimated transmit time with the new Range Calculation */
               transmit_time = (rcvtime - (Range / C));
               ++j;
          } else {
               /* printf("In else: Break, Range_err: %24.24f\n", Range_err); */
               break;
          }
     }
     xyzae->x = xk;
     xyzae->y = yk;
     xyzae->z = zk;
     xyzae->range = Range / (double) C;
     xyzae->svprn = prn;
     xyzae->delta_tr = (F*e*roota*sin(Ek));
     memcpy(&xyzae->time_received, master_time, sizeof(struct str_time));

/*
     printf("Delta_tr: %24.24f\n", xyzae->delta_tr);
     if (t == SNV) {
          printf("Sat Pos: %f, %d, %.9f, %.9f, %.9f, %.9f, %d\n",rcvtime * 1000, prn +1, xyzae->x, xyzae->y, xyzae->z, xyzae->range, snv->aodc); 
     } else {
          printf("Sat Pos: %f, %d, %.9f, %.9f, %.9f, %.9f\n"    ,rcvtime * 1000, prn +1, xyzae->x, xyzae->y, xyzae->z, xyzae->range);
     }
*/
}

