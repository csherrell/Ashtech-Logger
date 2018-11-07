#define _ISOC9X_SOURCE  1
#define _ISOC99_SOURCE  1
#include  <math.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef LAAS_H
#include "../laas.h"
#endif

#ifndef LAAS_MESSAGE_TYPE_4_H
#include "laasMessageType4.h"
#endif

#include "utils_string.h"
#include "crc_32bit.h"

/*
#define FAS_BLOCK_SIZE 38
*/
#define FAS_BLOCK_SIZE 38 
#define laasMessageType4Size 39 


#define MASK_DSL     0x00000001
#define MASK_OT      0x00000002
#define MASK_SSP     0x00000004
#define MASK_AID     0x00000008
#define MASK_RN      0x00000010
#define MASK_RL      0x00000020
#define MASK_APD     0x00000040
#define MASK_RI      0x00000080
#define MASK_RPDS    0x00000100
#define MASK_RPID    0x00000200
#define MASK_LAT     0x00000400
#define MASK_LON     0x00000800
#define MASK_HEIGHT  0x00001000
#define MASK_D_LAT   0x00002000
#define MASK_D_LON   0x00004000
#define MASK_ATCH    0x00008000
#define MASK_ATUS    0x00010000
#define MASK_GPA     0x00020000
#define MASK_CWAT    0x00040000
#define MASK_DLO     0x00080000
#define MASK_FAS_CRC 0x00100000
#define MASK_FAS_VAL 0x00200000
#define MASK_FAS_LAL 0x00400000


int option = 0;

struct laasMessageType4 *
laasMessageType4_init()
{
    struct laasMessageType4 *mt4 = 0;
    mt4 = (struct laasMessageType4 *) malloc(sizeof(struct laasMessageType4));
    memset(mt4,'\0',sizeof(struct laasMessageType4));
    return mt4;
}

struct laasMessageType4 *
laasMessageType4_delete(struct laasMessageType4 *mt4)
{
    free(mt4);
    mt4 = 0;
    return mt4;
}

void
laasMessageType4_clear(struct laasMessageType4 *mt4)
{
    memset(mt4,'\0',sizeof(struct laasMessageType4));
}

void
setLmt4Validity(struct laasMessageType4 *msg, unsigned int index)
{
     isValidLaasMessageType4Dsl(msg, index);
     isValidLaasMessageType4Ot(msg, index);
     isValidLaasMessageType4Ssp(msg, index);
     isValidLaasMessageType4Rn(msg, index);
     isValidLaasMessageType4Apd(msg, index);
     isValidLaasMessageType4Rpds(msg, index);
     isValidLaasMessageType4Lat(msg, index);
     isValidLaasMessageType4Lon(msg, index);
     isValidLaasMessageType4Height(msg, index);
     isValidLaasMessageType4D_lat(msg, index);
     isValidLaasMessageType4D_lon(msg, index);
     isValidLaasMessageType4Atch(msg, index);
     isValidLaasMessageType4Gpa(msg, index);
     isValidLaasMessageType4Cwat(msg, index);
     isValidLaasMessageType4Dlo(msg, index);
     isValidLaasMessageType4Fas_val(msg, index);
     isValidLaasMessageType4Fas_lal(msg, index);
}


int
getLmt4AsByteArray(struct laasMessageType4 *msgType4, unsigned char msgData[])
{
     int             i = 0;
     double        dbl = 0;
     long          lng = 0;
     double resolution = 0;

     for(i=0;i<msgType4->fas_count;i++)
     {
          msgData[0] = msgType4->fas[i].dsl;

          msgData[1]  = (msgType4->fas[i].ot & 0x0F);
          msgData[1] |= ((msgType4->fas[i].ssp & 0x0F) << 4);

/* May need to mask with 0xFF */
/*
          id = msgType4->fas[i].aid;
          id = msgType4->fas[i].aid >>  8;
          id = msgType4->fas[i].aid >> 16;
          id = msgType4->fas[i].aid >> 24;
*/

          msgData[2] = ia_5_to_bin[(unsigned int)msgType4->fas[i].aid[3]];
          msgData[3] = ia_5_to_bin[(unsigned int)msgType4->fas[i].aid[2]];
          msgData[4] = ia_5_to_bin[(unsigned int)msgType4->fas[i].aid[1]];
          msgData[5] = ia_5_to_bin[(unsigned int)msgType4->fas[i].aid[0]];

          msgData[6]  = (msgType4->fas[i].rn & 0x3F);
/*
          msgData[6] |= ((msgType4->fas[i].rl & 0x03) << 6);
*/
          switch(msgType4->fas[i].rl)
          {
               case 'R': msgData[6] |= 1<<6; break;
               case 'C': msgData[6] |= 2<<6; break;
               case 'L': msgData[6] |= 3<<6; break;
               default:  msgData[6] |= 0<<6; break;
          }

          msgData[7]  = (msgType4->fas[i].apd & 0x07);

          msgData[7] |= (ia_5_to_bin[(unsigned int)msgType4->fas[i].ri] << 3);

          msgData[8] = msgType4->fas[i].rpds;

          msgData[9]  = ia_5_to_bin[(unsigned int)msgType4->fas[i].rpid[3]];
          msgData[10] = ia_5_to_bin[(unsigned int)msgType4->fas[i].rpid[2]];
          msgData[11] = ia_5_to_bin[(unsigned int)msgType4->fas[i].rpid[1]];
          msgData[12] = ia_5_to_bin[(unsigned int)msgType4->fas[i].rpid[0]];

          dbl = (((msgType4->fas[i].lat * 3600) / 0.0005));
          lng = lrint(dbl);
          msgData[13] = lng;
          msgData[14] = lng >> 8;
          msgData[15] = lng >> 16;
          msgData[16] = lng >> 24;

          dbl = (((msgType4->fas[i].lon * 3600) / 0.0005));
          lng = lrint(dbl);
          msgData[17] = lng;
          msgData[18] = lng >> 8;
          msgData[19] = lng >> 16;
          msgData[20] = lng >> 24;

          dbl = ((msgType4->fas[i].height + 512) / 0.1);
          lng = lrint(dbl);
          msgData[21] = lng;
          msgData[22] = lng >> 8;

          dbl = (((msgType4->fas[i].d_lat * 3600) / 0.0005));
          lng = lrint(dbl);
          msgData[23] = lng;
          msgData[24] = lng >> 8;
          msgData[25] = lng >> 16;

          dbl = (((msgType4->fas[i].d_lon * 3600) / 0.0005));
          lng = lrint(dbl);
          msgData[26] = lng;
          msgData[27] = lng >> 8;
          msgData[28] = lng >> 16;

          if(msgType4->fas[i].atus == 0)
               resolution = 0.1;      /* 0 =  Feet   */
          else
               resolution = 0.05;     /* 1 =  Meters */
          dbl = ((msgType4->fas[i].atch / resolution));
          lng = lrint(dbl);
          msgData[29] = lng;
          msgData[30]  = ((lng >> 8) & 0x7F);
          msgData[30] |= ((msgType4->fas[i].atus & 0x01)<<7);

          dbl = (msgType4->fas[i].gpa / 0.01);
          lng = lrint(dbl);
          msgData[31] = lng;
          msgData[32] = lng >> 8;

          dbl = (double) ((msgType4->fas[i].cwat - 80) / 0.25);
          lng = lrint(dbl);
          msgData[33] = lng;

          dbl = (msgType4->fas[i].dlo / 8);
          lng = lrint(dbl);
          msgData[34] = lng;

          
          msgType4->fas[i].fas_crc = generate_crc_32bit(&msgData[1], FAS_BLOCK_SIZE - 4);
          msgData[35] = msgType4->fas[i].fas_crc;
          msgData[36] = msgType4->fas[i].fas_crc >> 8;
          msgData[37] = msgType4->fas[i].fas_crc >> 16;
          msgData[38] = msgType4->fas[i].fas_crc >> 24;

/*
          if(option)
          {
*/
               dbl = (msgType4->fas[i].fas_val / 0.1);
               lng = lrint(dbl);
               msgData[39] = lng;

               dbl = (msgType4->fas[i].fas_lal / 0.2);
               lng = lrint(dbl);
               msgData[40] = lng;
/*          }  */

          msgData = &msgData[laasMessageType4Size+2];

     }
     return (msgType4->fas_count * (laasMessageType4Size + 2));
} /* End getLmt4AsByteArray */


void
laasMessageType4_print2(struct laasMessageType4 *msg, unsigned char msgData[])
{
     int i = 0;

     printf("FAS Count: %d\n",msg->fas_count);

     for(i=0;i<msg->fas_count;i++)
     {
          print_buffer(&msgData[1],20);
          printf("\n");
          print_buffer(&msgData[21],18);
          printf("\n\n");
          msgData = &msgData[laasMessageType4Size+2];

          printf("FAS Data Set Length = %d  ",getLaasMessageType4Dsl(msg,i));
          printf("VAL =  %0.1fm   ",getLaasMessageType4Fas_val(msg,i));
          printf("LAL =  %0.1fm\n",getLaasMessageType4Fas_lal(msg,i));
          printf("RPDS =  %d  ",getLaasMessageType4Rpds(msg,i));
          printf("Operation type =  %d  ",getLaasMessageType4Ot(msg,i));
          printf("Airport = %s  ",getLaasMessageType4Aid(msg,i));
          printf("RW number = %d ",getLaasMessageType4Rn(msg,i));
          
          if('\0' == getLaasMessageType4Rl(msg,i))
               printf("RW letter = b\n");
          else
               printf("RW letter = %c\n",getLaasMessageType4Rl(msg,i));
          printf("LTP Lat =   %0.8f  ",getLaasMessageType4Lat(msg,i));
          printf("LTP Lon =  %0.8f  ",getLaasMessageType4Lon(msg,i));
          printf("LDP Height = %0.2f\n",getLaasMessageType4Height(msg,i));
          printf("Ref Path ID = %s  ",getLaasMessageType4Rpid(msg,i));
          printf("App Perf = %d  ",getLaasMessageType4Atus(msg,i));
          printf("Route Ind = %c\n",getLaasMessageType4Ri(msg,i));
          printf("FPAP Lat = %0.8f  ",getLaasMessageType4Lat(msg,i) + getLaasMessageType4D_lat(msg,i));
          printf("FPAP Lon = %0.8f  ",getLaasMessageType4Lon(msg,i) + getLaasMessageType4D_lon(msg,i));
          printf("TCH = %0.2f",getLaasMessageType4Atch(msg,i));
          if(getLaasMessageType4Atus(msg,i) == 0)
               printf("f  ");
          else
               printf("m  ");
          printf("GPA = %0.2f\n",getLaasMessageType4Gpa(msg,i));
          printf("Delta FPAP Lat = %0.8f  ",getLaasMessageType4D_lat(msg,i));
          printf("Delta FPAP Lon = %0.8f\n",getLaasMessageType4D_lon(msg,i));
          printf("CW@threshold = %0.6f   ",getLaasMessageType4Cwat(msg,i));
          printf("Delta Length Offset = %d\n\n",getLaasMessageType4Dlo(msg,i));

     }
}


void laasMessageType4_print(struct laasMessageType4 *msg, unsigned char msgData[])
{
    int i = 0;

    printf("Laas Message Type4 =============================\n");

    printf("                              RawMessage                     get isValid\n");

    printf("FAS Count: %d\n",msg->fas_count);

    for(i=0;i<msg->fas_count;i++)
    {

        printf("FAS Block --------------------------------------\n");
        printf("Message Segment: [");
        print_buffer(&msgData[0],9);
        printf("]\n");

/***** Print Data Set Length ****************/
    printf("Data Set Length:                    0x%02X         %16d ",
                    (msgData[0]),
                     getLaasMessageType4Dsl(msg,i));

    if(!isValidLaasMessageType4Dsl(msg,i))
        printf("Invalid");
    printf("\n");
/********************************************************************/

/***** Print Operation Type *****************************************/
        printf("Operation Type:                     0x%02X         %16d ",(msgData[1] & 0x0F),
                                                                   getLaasMessageType4Ot(msg,i));
        if(!isValidLaasMessageType4Ot(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print SBAS Service Provider **********************************/
        printf("SBAS Service Provider:              0x%02X         %16d ",(msgData[1] & 0xF0),
                                                                   getLaasMessageType4Ssp(msg,i));
        if(!isValidLaasMessageType4Ssp(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Airport Id *********************************************/
        printf("Airport Id:                   0x%08X   %s\n",((msgData[5]<<24) | (msgData[4]<<16) |
                                                                   (msgData[3]<<8)  | (msgData[2])),
                                                                   getLaasMessageType4Aid(msg,i));
/***** Print Runway Number ******************************************/
        printf("Runway Number:                      0x%02X         %16d ",(msgData[6] & 0x3F),
                                                                   getLaasMessageType4Rn(msg,i));
        if(!isValidLaasMessageType4Rn(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Runway Letter ******************************************/
        printf("Runway Letter:                      0x%02X         ",(msgData[6] & 0xC0));
        (getLaasMessageType4Rl(msg,i) == (char) NULL) ?
        printf("''\n")   :
        printf("'%c'\n",getLaasMessageType4Rl(msg,i)) ;
/***** Print Approach Performance Desig *****************************/
        printf("Approach Per Des                    0x%02X         %16d ",(msgData[7] & 0x07),
                                                                   getLaasMessageType4Apd(msg,i));
        if(!isValidLaasMessageType4Apd(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Route Indicator ****************************************/
        printf("Route Indicator:                    0x%02X         '%c'\n",((msgData[7] & 0xF8)>>3),                                                                  getLaasMessageType4Ri(msg,i));
/***** Print Reference Path Data Select *****************************/
        printf("Ref Path Selector:                  0x%02X         %d ",msgData[8],
                                                                   getLaasMessageType4Rpds(msg,i));
        if(!isValidLaasMessageType4Rpds(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Reference Path ID **************************************/
        printf("\nMessage Segment: [");
        print_buffer(&msgData[8],4);
        printf("]\n");

        printf("Reference Path ID:            0x%08X   %s\n",((msgData[12]<<24) | (msgData[11]<<16) |
                                                                    (msgData[10]<<8) | (msgData[9])),
                                                                   getLaasMessageType4Rpid(msg,i));
/***** Print LTP/FTP Latitude ***************************************/

        printf("\nMessage Segment: [");
        print_buffer(&msgData[12],4);
        printf("]\n");

        printf("LTP/FTP Latitude:             0x%08X   %16.10f ",((msgData[16]<<24) | (msgData[15]<<16) |
                                                                     (msgData[14]<<8) | (msgData[13])),
                                                                     getLaasMessageType4Lat(msg,i));
        if(!isValidLaasMessageType4Lat(msg,i))
            printf("Invalid");
        printf("\n");

/***** Print LTP/FTP Longitude **************************************/

        printf("\nMessage Segment: [");
        print_buffer(&msgData[16],4);
        printf("]\n");
        printf("LTP/FTP Longitude:            0x%08X   %16.10f ",((msgData[20]<<24) | (msgData[19]<<16) |
                                                      (msgData[18]<<8) | (msgData[17])),
                                                                     getLaasMessageType4Lon(msg,i));
        if(!isValidLaasMessageType4Lon(msg,i))
            printf("Invalid");
        printf("\n");

/***** Print LTP/FTP Height *****************************************/
        printf("\nMessage Segment: [");
        print_buffer(&msgData[20],8);
        printf("]\n");

        printf("LTP/FTP Height:                   0x%04X       %16.1f ",((msgData[22]<<8) | msgData[21]),
                                                                     getLaasMessageType4Height(msg,i));
        if(!isValidLaasMessageType4Height(msg,i))
            printf("Invalid");
        printf("\n");

/***** Print Delta FPAP Latitude ************************************/
        printf("Delta FPAP Latitude:            0x%06X   %16.10f ",((msgData[25]<<16) | (msgData[24]<<8) | msgData[23]),
                                                                     getLaasMessageType4D_lat(msg,i));
        if(!isValidLaasMessageType4D_lat(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Delta FPAP Longitude ***********************************/
        printf("Delta FPAP Longitude:           0x%06X   %16.10f ",((msgData[28]<<16) | (msgData[27]<<8) | msgData[26]),
                                                                     getLaasMessageType4D_lon(msg,i));
        if(!isValidLaasMessageType4D_lon(msg,i))
            printf("Invalid");
        printf("\n");

/***** Print Approach TCH *******************************************/
        printf("\nMessage Segment: [");
        print_buffer(&msgData[28],10);
        printf("]\n");
        printf("Approach TCH:                     0x%04X       %16.2f ",(((msgData[30] & 0x7f)<<8) | msgData[29]),
                                                                     getLaasMessageType4Atch(msg,i));
        if(!isValidLaasMessageType4Atch(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Approach TCH Units Selector ****************************/
        printf("TCH Units Selector:                 0x%02X         %16d\n",((msgData[30] & 0x80)>>7),
                                                                     getLaasMessageType4Atus(msg,i));
/***** Print Glide Path Angle ***************************************/
        printf("Glide Path Angle:                 0x%04X       %16.2f ",((msgData[32]<<8) | (msgData[31])),
                                                                     getLaasMessageType4Gpa(msg,i));
        if(!isValidLaasMessageType4Gpa(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Course Width at Threshold ******************************/
        printf("Course Width at Threshold:          0x%02X         %16.2f ",msgData[33],
                                                                     getLaasMessageType4Cwat(msg,i));
        if(!isValidLaasMessageType4Cwat(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Delta Length Offset ************************************/
        printf("Delta Length Offset:                0x%02X         %16d ",msgData[34],
                                                                     getLaasMessageType4Dlo(msg,i));
        if(!isValidLaasMessageType4Dlo(msg,i))
            printf("Invalid");
        printf("\n");
/***** Print Final Approach Segment CRC *****************************/
        printf("FAS CRC:                      0x%08X   %16d\n",((msgData[38]<<24) | (msgData[37]<<16) |
                                                      (msgData[36]<<8) | (msgData[35])),
                                                                     getLaasMessageType4Fas_crc(msg,i));
/***** Print Vertical Alert Limit ***********************************/
    printf("FAS Vertical Alert Limit:                          %16.1f ",
                                                                     getLaasMessageType4Fas_val(msg,i));
    if(!isValidLaasMessageType4Fas_val(msg,i))
        printf("Invalid");
    printf("\n");
/***** Print  Lateral Alert Limit ***********************************/
    printf("FAS Lateral Alert Limit:                           %16.1f ",
                                                                     getLaasMessageType4Fas_lal(msg,i));
    if(!isValidLaasMessageType4Fas_lal(msg,i))
        printf("Invalid");
    printf("\n");

        msgData = &msgData[laasMessageType4Size];
    }
}

void
laasMessageType4_print_csv(struct laasMessageType4 *msg, unsigned char msgData[])
{
     int i = 0;

     for(i=0;i<msg->fas_count;i++)
     {
          printf("%d,",getLaasMessageType4Dsl(msg,i));
          printf("%d,",getLaasMessageType4Ot(msg,i));
          printf("%d,",getLaasMessageType4Ssp(msg,i));
          printf("\"%s\",",getLaasMessageType4Aid(msg,i));
          printf("%d,",getLaasMessageType4Rn(msg,i));
          
          if('\0' == getLaasMessageType4Rl(msg,i))
               printf("\'b\',");
          else
               printf("\'%c\',",getLaasMessageType4Rl(msg,i));

          printf("%d,",getLaasMessageType4Apd(msg,i));
          printf("\'%c\',",getLaasMessageType4Ri(msg,i));
          printf("%d,",getLaasMessageType4Rpds(msg,i));
          printf("\"%s\",",getLaasMessageType4Rpid(msg,i));
          printf("%0.25f,",getLaasMessageType4Lat(msg,i));
          printf("%0.25f,",getLaasMessageType4Lon(msg,i));
          printf("%0.25f,",getLaasMessageType4Height(msg,i));
          printf("%0.25f,",getLaasMessageType4D_lat(msg,i));
          printf("%0.25f,",getLaasMessageType4D_lon(msg,i));
          printf("%0.25f,",getLaasMessageType4Atch(msg,i));
          printf("%d,",getLaasMessageType4Atus(msg,i));
          printf("%0.25f,",getLaasMessageType4Gpa(msg,i));
          printf("%0.25f,",getLaasMessageType4Cwat(msg,i));
          printf("%d,",getLaasMessageType4Dlo(msg,i));
          printf("0x%08X,",getLaasMessageType4Fas_crc(msg,i));
          printf("%0.25f,",getLaasMessageType4Fas_val(msg,i));
          printf("%0.25f,",getLaasMessageType4Fas_lal(msg,i));
     }
}

/********************************************************************/
int
getLaasMessageType4FAS_Count(struct laasMessageType4 *msg)
{
     return msg->fas_count;
}

void
setLaasMessageType4FAS_Count(struct laasMessageType4 *msg, int val)
{
     msg->fas_count = val;
}


/********************************************************************/
unsigned int
getLaasMessageType4Dsl(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].dsl;
}

void
setLaasMessageType4Dsl(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].dsl = val;
     isValidLaasMessageType4Dsl(msg,index);
}

int
isValidLaasMessageType4Dsl(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range     2 - 212 Resolution: 1 */
     /* Raw Range 2 - 212  */

     if(msg->fas[index].dsl < 2 || msg->fas[index].dsl > 212)
     {
         msg->fas[index].valid |= MASK_DSL;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_DSL;
         return true;
     }
}

/********************************************************************/
unsigned int
getLaasMessageType4Ot(struct laasMessageType4 *msg, unsigned int index)
{
     return (unsigned int) msg->fas[index].ot;
}

void
setLaasMessageType4Ot(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].ot = (unsigned int) val;
     isValidLaasMessageType4Ot(msg,index);
}

int
isValidLaasMessageType4Ot(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range     0 - 15 Resolution: 1 */
     /* Raw Range 0 - 15  */
     if(msg->fas[index].ot > 15)
     {
         msg->fas[index].valid |= MASK_OT;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_OT;
         return true;
     }
}

/********************************************************************/
unsigned int
getLaasMessageType4Ssp(struct laasMessageType4 *msg, unsigned int index)
{
     return (unsigned int) msg->fas[index].ssp;
}

void
setLaasMessageType4Ssp(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].ssp = (unsigned int) val;
     isValidLaasMessageType4Ssp(msg,index);
}

int
isValidLaasMessageType4Ssp(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range     0 - 15 Resolution: 1 */
     /* Raw Range 0 - 15  */
     if(msg->fas[index].ssp > 15)
     {
         msg->fas[index].valid |= MASK_SSP;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_SSP;
         return true;
     }
}

/********************************************************************/
char * 
getLaasMessageType4Aid(struct laasMessageType4 *msg, unsigned int index)
{
     return (char *) &msg->fas[index].aid;
}

void
setLaasMessageType4Aid(struct laasMessageType4 *msg, unsigned int index, char val[])
{
     msg->fas[index].aid[0] = val[0];
     msg->fas[index].aid[1] = val[1];
     msg->fas[index].aid[2] = val[2];
     msg->fas[index].aid[3] = val[3];
}

/********************************************************************/
unsigned int
getLaasMessageType4Rn(struct laasMessageType4 *msg, unsigned int index)
{
     return (unsigned int) msg->fas[index].rn;
}

void
setLaasMessageType4Rn(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].rn = (unsigned int) val;
     isValidLaasMessageType4Rn(msg,index);
}

int
isValidLaasMessageType4Rn(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range     0 - 36 Resolution: 1 */
     /* Raw Range 0 - 36  */
     if(msg->fas[index].rn > 36)
     {
         msg->fas[index].valid |= MASK_RN;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_RN;
         return true;
     }
}

/********************************************************************/
char
getLaasMessageType4Rl(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].rl;
}

void
setLaasMessageType4Rl(struct laasMessageType4 *msg, unsigned int index, char val)
{
     msg->fas[index].rl = val;
}

/********************************************************************/
unsigned int
getLaasMessageType4Apd(struct laasMessageType4 *msg, unsigned int index)
{
     return (unsigned int) msg->fas[index].apd;
}

void
setLaasMessageType4Apd(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].apd = (unsigned int) val;
     isValidLaasMessageType4Apd(msg,index);
}

int
isValidLaasMessageType4Apd(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range     0 - 7 Resolution: 1 */
     /* Raw Range 0 - 7  */
     if(msg->fas[index].apd > 7)
     {
         msg->fas[index].valid |= MASK_APD;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_APD;
         return true;
     }
}

/********************************************************************/
char
getLaasMessageType4Ri(struct laasMessageType4 *msg, unsigned int index)
{
     return (char) msg->fas[index].ri;
}

void
setLaasMessageType4Ri(struct laasMessageType4 *msg, unsigned int index, char val)
{
     msg->fas[index].ri = (char) val;
}

/********************************************************************/
unsigned int
getLaasMessageType4Rpds(struct laasMessageType4 *msg, unsigned int index)
{
     return (unsigned int) msg->fas[index].rpds;
}

void
setLaasMessageType4Rpds(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].rpds = (unsigned int) val;
     isValidLaasMessageType4Rpds(msg,index);
}

int
isValidLaasMessageType4Rpds(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range     0 - 48 Resolution: 1 */
     /* Raw Range 0 - 48  */
     if(msg->fas[index].rpds > 48)
     {
         msg->fas[index].valid |= MASK_RPDS;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_RPDS;
         return true;
     }
}

/********************************************************************/
char *
getLaasMessageType4Rpid(struct laasMessageType4 *msg, unsigned int index)
{
     return (char *) &msg->fas[index].rpid;
}

void
setLaasMessageType4Rpid(struct laasMessageType4 *msg, unsigned int index, char val[])
{
     msg->fas[index].rpid[0] = val[0];
     msg->fas[index].rpid[1] = val[1];
     msg->fas[index].rpid[2] = val[2];
     msg->fas[index].rpid[3] = val[3];
}

/********************************************************************/
double
getLaasMessageType4Lat(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].lat;
}

void
setLaasMessageType4Lat(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].lat = val;
     isValidLaasMessageType4Lat(msg,index);
}

int
isValidLaasMessageType4Lat(struct laasMessageType4 *msg, unsigned int index)
{
/* Range -648000000 - 648000000 arcsec Resolution: 0.0005 */
     /* Range     0 - 48 Resolution: 1 */
     /* Raw Range 0 - 48  */
     if(msg->fas[index].lat < -90.0 || msg->fas[index].lat > 90.0)
     {
         msg->fas[index].valid |= MASK_LAT;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_LAT;
         return true;
     }
}

/********************************************************************/
double
getLaasMessageType4Lon(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].lon;
}

void
setLaasMessageType4Lon(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].lon = val;
     isValidLaasMessageType4Lon(msg,index);
}

int
isValidLaasMessageType4Lon(struct laasMessageType4 *msg, unsigned int index)
{
/* Range -1296000000 - 1296000000 arcsec Resolution: 0.0005 */
     if(msg->fas[index].lon < -180.0 || msg->fas[index].lon > 180.0)
     {
         msg->fas[index].valid |= MASK_LON;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_LON;
         return true;
     }
}

/********************************************************************/
double
getLaasMessageType4Height(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].height;
}

void
setLaasMessageType4Height(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].height = val;
     isValidLaasMessageType4Height(msg,index);
}

int
isValidLaasMessageType4Height(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range -5120 - 60415 m Resolution: 0.1 */
     if(msg->fas[index].height < -512.0 || msg->fas[index].height > 6041.5)
     {
         msg->fas[index].valid |= MASK_HEIGHT;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_HEIGHT;
         return true;
     }
}

/********************************************************************/
double
getLaasMessageType4D_lat(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].d_lat;
}

void
setLaasMessageType4D_lat(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].d_lat = val;
     isValidLaasMessageType4D_lat(msg,index);
}

int
isValidLaasMessageType4D_lat(struct laasMessageType4 *msg, unsigned int index)
{
/* Range -7200000 - 7200000 arcsec Resolution: 0.0005 */
     if(msg->fas[index].d_lat < -1.0 || msg->fas[index].d_lat > 1.0)
     {
         msg->fas[index].valid |= MASK_LAT;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_LAT;
         return true;
     }
}

/********************************************************************/
double
getLaasMessageType4D_lon(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].d_lon;
}

void
setLaasMessageType4D_lon(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].d_lon = val;
     isValidLaasMessageType4D_lon(msg,index);
}

int
isValidLaasMessageType4D_lon(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range -7200000 - 7200000 arcsec Resolution: 0.0005 */
     if(msg->fas[index].d_lon < -1.0 || msg->fas[index].d_lon > 1.0)
     {
         msg->fas[index].valid |= MASK_LON;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_LON;
         return true;
     }
}

/********************************************************************/
double
getLaasMessageType4Atch(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].atch;
}

void
setLaasMessageType4Atch(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].atch = val;
     isValidLaasMessageType4Atch(msg,index);
}

int
isValidLaasMessageType4Atch(struct laasMessageType4 *msg, unsigned int index)
{
     double dbl = 0;
     long   lng = 0;

     if(msg->fas[index].atus == 0)
          dbl = msg->fas[index].atch / 0.1;
     else 
          dbl = msg->fas[index].atch / 0.05;

     lng = lrint(dbl);
     
     /* Range 0 - 32767 arcsec Resolution: 0.1 */
     if(lng < 0 || lng > 32767)
     {
          msg->fas[index].valid |= MASK_ATCH;
          return false;
     }
     else
     {
          msg->fas[index].valid &= ~MASK_ATCH;
          return true;
     }
}

/********************************************************************/
unsigned int
getLaasMessageType4Atus(struct laasMessageType4 *msg, unsigned int index)
{
     return (unsigned int) msg->fas[index].atus;
}

void
setLaasMessageType4Atus(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].atus = (unsigned int) val;
}

/********************************************************************/
double
getLaasMessageType4Gpa(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].gpa;
}

void
setLaasMessageType4Gpa(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].gpa = val;
     isValidLaasMessageType4Gpa(msg,index);
}

int
isValidLaasMessageType4Gpa(struct laasMessageType4 *msg, unsigned int index)
{
/* Range 0 - 9000 degrees Resolution: 0.01 */
     if(msg->fas[index].gpa < 0.0 || msg->fas[index].gpa > 90.0)
     {
         msg->fas[index].valid |= MASK_GPA;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_GPA;
         return true;
     }
}

/********************************************************************/
double
getLaasMessageType4Cwat(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].cwat;
}

void
setLaasMessageType4Cwat(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].cwat = val;
     isValidLaasMessageType4Cwat(msg,index);
}

int
isValidLaasMessageType4Cwat(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range 320 - 575 m Resolution: 0.25 */
     if(msg->fas[index].cwat < 80.00 || msg->fas[index].cwat > 143.75)
     {
         msg->fas[index].valid |= MASK_CWAT;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_CWAT;
         return true;
     }
}

/********************************************************************/
unsigned int
getLaasMessageType4Dlo(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].dlo;
}

void
setLaasMessageType4Dlo(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].dlo = val;
     isValidLaasMessageType4Dlo(msg,index);
}

int
isValidLaasMessageType4Dlo(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range 0 - 254 m Resolution: 8 */
     if(msg->fas[index].dlo > 2032)
     {
         msg->fas[index].valid |= MASK_DLO;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_DLO;
         return true;
     }
}

/********************************************************************/
unsigned int
getLaasMessageType4Fas_crc(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].fas_crc;
}

void
setLaasMessageType4Fas_crc(struct laasMessageType4 *msg, unsigned int index, unsigned int val)
{
     msg->fas[index].fas_crc = (unsigned int) val;
}

/********************************************************************/
double
getLaasMessageType4Fas_val(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].fas_val;
}

void
setLaasMessageType4Fas_val(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].fas_val = val;
     isValidLaasMessageType4Fas_val(msg,index);
}

int
isValidLaasMessageType4Fas_val(struct laasMessageType4 *msg, unsigned int index)
{
     /* Range 0 - 25.4 m Resolution: 0.1 */
     if(msg->fas[index].fas_val < 0.0 || msg->fas[index].fas_val > 25.4)
     {
         msg->fas[index].valid |= MASK_FAS_VAL;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_FAS_VAL;
         return true;
     }
}

/********************************************************************/
double
getLaasMessageType4Fas_lal(struct laasMessageType4 *msg, unsigned int index)
{
     return msg->fas[index].fas_lal;
}

void
setLaasMessageType4Fas_lal(struct laasMessageType4 *msg, unsigned int index, double val)
{
     msg->fas[index].fas_lal = val;
     isValidLaasMessageType4Fas_lal(msg,index);
}

int
isValidLaasMessageType4Fas_lal(struct laasMessageType4 *msg, unsigned int index)
{
/* Range 0 - 50.8 m Resolution: 0.2 */
     if(msg->fas[index].fas_lal < 0.0 || msg->fas[index].fas_lal > 50.8)
     {
         msg->fas[index].valid |= MASK_FAS_LAL;
         return false;
     }
     else
     {
         msg->fas[index].valid &= ~MASK_FAS_LAL;
         return true;
     }
}

void
laasMessageType4_process(struct laasMessageType4 *msgType4, unsigned char msgData[], int length)
{
    int i = 0;

    char c_temp = 0x00;

    int temp = 0;
    unsigned int utemp = 0;

    double resolution = 0;

    double count = 0;

/* 
     The old base station code does not transmit FAS Vertical Alert Limit
     or FAS Lateral Alert Limit.  Once this code becomes our full time system
     then this patch should be totally removed from the code
*/

    laasMessageType4_clear(msgType4);

    option = (length - 10);
    if((option%41) == 0)
    {
          modf(option/41,&count);
          msgType4->fas_count = (int) count;
          option = 2;
    }

    if((option%39) == 0)
    {
          modf(option/39,&count);
          msgType4->fas_count = (int) count;
          option = 0;
    }

    for(i=0;i<msgType4->fas_count;i++)
    {
        if(calculate_crc_32bit(&msgData[1], FAS_BLOCK_SIZE) == true)
        {       
            msgType4->fas[i].dsl = msgData[0];

            msgType4->fas[i].ot  = (msgData[1] & 0x0F);       /* 4 bits only (8) */
            msgType4->fas[i].ssp = ((msgData[1]>>4) & 0x0F);  /* 4 bits only (8) */

            msgType4->fas[i].aid[0] = ia_5[msgData[5]];
            msgType4->fas[i].aid[1] = ia_5[msgData[4]];
            msgType4->fas[i].aid[2] = ia_5[msgData[3]];
            msgType4->fas[i].aid[3] = ia_5[msgData[2]];

            msgType4->fas[i].rn = (msgData[6] & 0x3F);
            switch((msgData[6]>>6) & 0x03)
            {
               case 1:  msgType4->fas[i].rl = 'R'; break;
               case 2:  msgType4->fas[i].rl = 'C'; break;
               case 3:  msgType4->fas[i].rl = 'L'; break;
               default: msgType4->fas[i].rl = '\0';   break;
            }

            msgType4->fas[i].apd = (msgData[7] & 0x07);

            msgType4->fas[i].ri = ia_5[((msgData[7]>>3) & 0x1F)];

            msgType4->fas[i].rpds = msgData[8];

            msgType4->fas[i].rpid[0] = ia_5[msgData[12]];
            msgType4->fas[i].rpid[1] = ia_5[msgData[11]];
            msgType4->fas[i].rpid[2] = ia_5[msgData[10]];
            msgType4->fas[i].rpid[3] = ia_5[msgData[9]];

            temp = (int)((msgData[16]<<24) | (msgData[15]<<16) | (msgData[14]<<8) | (msgData[13]));
            msgType4->fas[i].lat = (double) ((temp * 0.0005) / 3600);

            temp = (int)((msgData[20]<<24) | (msgData[19]<<16) | (msgData[18]<<8) | (msgData[17]));
            msgType4->fas[i].lon = (double) ((temp * 0.0005) / 3600);

            temp = (int)((msgData[22]<<8) | msgData[21]);
            msgType4->fas[i].height = ((temp * 0.1) - 512);

            c_temp = (msgData[25]);
            temp = (int)((c_temp<<16) | (msgData[24]<<8) | msgData[23]);
            msgType4->fas[i].d_lat = (double)((temp * 0.0005) / 3600);

            c_temp = (msgData[28]);
            temp = (int)((c_temp<<16) | (msgData[27]<<8) | msgData[26]);
            msgType4->fas[i].d_lon = (double)((temp * 0.0005) / 3600);

            msgType4->fas[i].atus = ((msgData[30]>>7) & 0x01);
            if(msgType4->fas[i].atus == 0)
                resolution = 0.1;      /* 0 =  Feet   */
            else
                resolution = 0.05;     /* 1 =  Meters */

            utemp = (unsigned int)(((msgData[30] & 0x7F)<<8) | msgData[29]);
            msgType4->fas[i].atch = (double) (utemp * resolution);

            utemp = (unsigned int) ((msgData[32]<<8) | (msgData[31]));
            msgType4->fas[i].gpa  = (double) (utemp * 0.01);

            msgType4->fas[i].cwat = (double)((msgData[33] * 0.25) + 80);
            msgType4->fas[i].dlo  = msgData[34] * 8;

            msgType4->fas[i].fas_crc = (unsigned int)((msgData[38]<<24) | (msgData[37]<<16) | (msgData[36]<<8) | (msgData[35]));
            if(option)
            {
               msgType4->fas[i].fas_val = (double) (msgData[39] * 0.1);
               msgType4->fas[i].fas_lal = (double) (msgData[40] * 0.2);
            }

            msgData = &msgData[laasMessageType4Size+option];
            setLmt4Validity(msgType4,i);

        } else
            printf("ERROR: LAAS Message 4 FAS CRC Invalid\n");

/*
        length -= (laasMessageType4Size + option);
        if(length < (laasMessageType4Size + option))
            done = true;
*/

    }
/*    msgType4->fas_count = i;  */
}
