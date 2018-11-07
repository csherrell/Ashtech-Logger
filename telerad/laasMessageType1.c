#define _ISOC9X_SOURCE  1
#define _ISOC99_SOURCE  1
#include  <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef LAAS_H
#include "../laas.h"
#endif

#ifndef UTILS_STRING_H
#include "utils_string.h"
#endif

#ifndef LAAS_MESSAGE_TYPE_1_H
#include "laasMessageType1.h"
#endif


#define MASK_MZC   0x01
#define MASK_AMF   0x02
#define MASK_NOM   0x04
#define MASK_MT    0x08
#define MASK_ED    0x10
#define MASK_ECRC  0x20
#define MASK_SAD   0x40

#define MASK_RS_ID 0x00000001
#define MASK_IOD   0x00000002
#define MASK_PRC   0x00000004
#define MASK_RRC   0x00000008
#define MASK_SPG   0x00000010
#define MASK_B1    0x00000020
#define MASK_B2    0x00000040
#define MASK_B3    0x00000080
#define MASK_B4    0x00000100

/* From limits.h */
    /* Unsigned Char Range 0 - 255         */
    /* Unsigned Int  Range 0 - 4294967295U */
    /* Int Range           0 - 2147483647  */


/***************************************************************************/

/* ModifiedZCount Methods ------------------------------------------*/
double
getLmt1ModifiedZCount(struct laasMessageType1 *msg)
{
    return (msg->mzc);
}

void
setLmt1ModifiedZCount(struct laasMessageType1 *msgType1, double param)
{
    msgType1->mzc = param;
    isValidLmt1ModifiedZCount(msgType1);
}

int
isValidLmt1ModifiedZCount(struct laasMessageType1 *msgType1)
{
    /* Range 0 - 1199.9 Sec */
    if(msgType1->mzc < 0 || msgType1->mzc > 1199.9)
    {
        msgType1->valid |= MASK_MZC;
        return false;
    }
    else
    {
        msgType1->valid &= ~MASK_MZC;
        return true;
    }
}

/* AdditionalMessageFlag Methods -----------------------------------*/
unsigned int
getLmt1AdditionalMessageFlag(struct laasMessageType1 *msg)
{
    return (unsigned int) msg->amf;
}


void
setLmt1AdditionalMessageFlag(struct laasMessageType1 *msgType1, unsigned int param)
{
    msgType1->amf = (unsigned int) param;
    isValidLmt1AdditionalMessageFlag(msgType1);
}

int
isValidLmt1AdditionalMessageFlag(struct laasMessageType1 *msgType1)
{
    /* Unsigned Char Range 0 - 255         */
    /* Range 0 - 3 */
    if(msgType1->amf > 3)
    {
        msgType1->valid |= MASK_AMF;
        return false;
    }
    else 
    {
        msgType1->valid &= ~MASK_AMF;
        return true;
    }
}

/* NumberOfMeasurements Methods ------------------------------------*/
unsigned int
getLmt1NumberOfMeasurements(struct laasMessageType1 *msg)
{
    return (unsigned int) msg->nom;
}

void
setLmt1NumberOfMeasurements(struct laasMessageType1 *msgType1, unsigned int param)
{
    msgType1->nom = (unsigned int) param;
    isValidLmt1NumberOfMeasurements(msgType1);
}

int
isValidLmt1NumberOfMeasurements(struct laasMessageType1 *msgType1)
{
    /* Unsigned Char Range 0 - 255         */
    /* Range 0 - 18  Really is 0 - 12 for the gg12*/
    if(msgType1->nom > 12)
    {
        msgType1->valid |= MASK_NOM;
        return false;
    }
    else 
    {
        msgType1->valid &= ~MASK_NOM;
        return true;
    }
}

/* MeasurementType Methods -----------------------------------------*/
unsigned int
getLmt1MeasurementType(struct laasMessageType1 *msg)
{
    return msg->mt;
}

void
setLmt1MeasurementType(struct laasMessageType1 *msgType1, unsigned int param)
{
    msgType1->mt = (unsigned int) param;
    isValidLmt1MeasurementType(msgType1);
}

int
isValidLmt1MeasurementType(struct laasMessageType1 *msgType1)
{
    /* Unsigned Char Range 0 - 255         */
    /* Range 0 - 7 */
    if(msgType1->mt > 7)
    {
        msgType1->valid |= MASK_MT;
        return false;
    }
    else 
    {
        msgType1->valid &= ~MASK_MT;
        return true;
    }
}

/* EphemerisDecorrelation Methods ----------------------------------*/
double
getLmt1EphemerisDecorrelation(struct laasMessageType1 *msg)
{
    return (double)(msg->ed);
}

void
setLmt1EphemerisDecorrelation(struct laasMessageType1 *msgType1, double param)
{
    msgType1->ed = param;
    isValidLmt1EphemerisDecorrelation(msgType1);
}

int
isValidLmt1EphemerisDecorrelation(struct laasMessageType1 *msgType1)
{
    /* Range 0 - 1.275x10^-3 */
    if(msgType1->ed < 0 || msgType1->ed > 1.275e-3)
    {
        msgType1->valid |= MASK_ED;
        return false;
    }
    else
    {
        msgType1->valid &= ~MASK_ED;
        return true;
    }
}


/* EphemerisCRC Methods --------------------------------------------*/
unsigned int
getLmt1EphemerisCRC(struct laasMessageType1 *msg)
{
    return (unsigned int) msg->ecrc;
}

void
setLmt1EphemerisCRC(struct laasMessageType1 *msgType1, unsigned int param)
{
    msgType1->ecrc = (unsigned int) param;
}

/* SourceAvailabilityDuration Methods ------------------------------*/
unsigned int
getLmt1SourceAvailabilityDuration(struct laasMessageType1 *msg)
{
    return (unsigned int) (msg->sad);
}

void
setLmt1SourceAvailabilityDuration(struct laasMessageType1 *msgType1, unsigned int param)
{
    msgType1->sad = param;
    isValidLmt1SourceAvailabilityDuration(msgType1);
}

int
isValidLmt1SourceAvailabilityDuration(struct laasMessageType1 *msgType1)
{
    /* Unsigned Int  Range 0 - 4294967295U */
    /* Range     0 - 2540 */
    /* DO-246B 2.4.2.1 Note 4 */
    if(msgType1->sad == 2550)
    {
        msgType1->valid |= MASK_SAD;
        return false;
    }
    else 
    {
        msgType1->valid &= ~MASK_SAD;
        return true;
    }
}


/* RangingSourceID Methods -----------------------------------------*/
unsigned int
getLmt1RangingSourceID(struct laasMessageType1 *msg, int index)
{
    return (unsigned int) msg->mb[index].rs_id;
}

void
setLmt1RangingSourceID(struct laasMessageType1 *msg, int index, unsigned int param)
{
    msg->mb[index].rs_id = (unsigned int) param;
    isValidLmt1RangingSourceID(msg,index);
}

int
isValidLmt1RangingSourceID(struct laasMessageType1 *msg, int index)
{
    /* Unsigned Char Range 0 - 255         */
    /* Range 1 - 255 */
    if(msg->mb[index].rs_id == 0)
    {
        msg->mb[index].valid |= MASK_RS_ID;
        return false;
    }
    else 
    {
        msg->mb[index].valid &= ~MASK_RS_ID;
        return true;
    }
}

/* IssueOfData Methods ---------------------------------------------*/
unsigned int
getLmt1IssueOfData(struct laasMessageType1 *msg, int index)
{
    return (unsigned int) msg->mb[index].iod;
}

void
setLmt1IssueOfData(struct laasMessageType1 *msg, int index, unsigned int param)
{
    msg->mb[index].iod = (unsigned int) param;
}

/* PseudorangeCorrection Methods -----------------------------------*/
double
getLmt1PseudorangeCorrection(struct laasMessageType1 *msg, int index)
{
    return (double)(msg->mb[index].prc);
}

void
setLmt1PseudorangeCorrection(struct laasMessageType1 *msg, int index, double param)
{ 
    msg->mb[index].prc = (param);
    isValidLmt1PseudorangeCorrection(msg, index);
}

int
isValidLmt1PseudorangeCorrection(struct laasMessageType1 *msg, int index)
{
    /* Range -327.67 - +327.67 */
    if(msg->mb[index].prc < -327.67 || msg->mb[index].prc > 327.67)
    {
        msg->mb[index].valid |= MASK_PRC;
        return false;
    }
    else
    {
        msg->mb[index].valid &= ~MASK_PRC;
        return true;
    }
}


/* RangeRateCorrection Methods -------------------------------------*/
double
getLmt1RangeRateCorrection(struct laasMessageType1 *msg, int index)
{
    return (double)msg->mb[index].rrc;
}

void
setLmt1RangeRateCorrection(struct laasMessageType1 *msg, int index, double param)
{
    msg->mb[index].rrc = param;
    isValidLmt1RangeRateCorrection(msg, index);
}

int
isValidLmt1RangeRateCorrection(struct laasMessageType1 *msg, int index)
{
    /* Range -32.767 - +32.767 */
    if(msg->mb[index].rrc < -32.767 || msg->mb[index].rrc > 32.767)
    {
        msg->mb[index].valid |= MASK_RRC;
        return false;
    }
    else
    {
        msg->mb[index].valid &= ~MASK_RRC;
        return true;
    }
}

/* SigmaPrGnd Methods ----------------------------------------------*/
double
getLmt1SigmaPrGnd(struct laasMessageType1 *msg, int index)
{
    return (double)msg->mb[index].spg;
}

void
setLmt1SigmaPrGnd(struct laasMessageType1 *msg, int index, double param)
{
    msg->mb[index].spg = param;
    isValidLmt1SigmaPrGnd(msg,index);
}

int
isValidLmt1SigmaPrGnd(struct laasMessageType1 *msg, int index)
{
    /* Range 0 - 5.08 */
    if(msg->mb[index].spg < 0 || msg->mb[index].spg > 5.08)
    {
        msg->mb[index].valid |= MASK_SPG;
        return false;
    }
    else
    {
        msg->mb[index].valid &= ~MASK_SPG;
        return true;
    }
}


/* B1 Methods ------------------------------------------------------*/
double
getLmt1B1(struct laasMessageType1 *msg, int index)
{
    return (double) (msg->mb[index].B1);
}

void
setLmt1B1(struct laasMessageType1 *msg, int index, double param)
{
    msg->mb[index].B1 = param;
    isValidLmt1B1(msg,index);
}

int
isValidLmt1B1(struct laasMessageType1 *msg, int index)
{
    /* Range -6.35 - +6.35 */
    if(msg->mb[index].B1 < -6.35 || msg->mb[index].B1 > 6.35)
    {
        msg->mb[index].valid |= MASK_B1;
        return false;
    }
    else
    {
        msg->mb[index].valid &= ~MASK_B1;
        return true;
    }
}

/* B2 Methods ------------------------------------------------------*/
double
getLmt1B2(struct laasMessageType1 *msg, int index)
{
    return (double)(msg->mb[index].B2);
}

void
setLmt1B2(struct laasMessageType1 *msg, int index, double param)
{
    msg->mb[index].B2 = param;
    isValidLmt1B2(msg,index);
}

int
isValidLmt1B2(struct laasMessageType1 *msg, int index)
{
    /* Range -6.35 - +6.35 */
    if(msg->mb[index].B2 < -6.35 || msg->mb[index].B2 > 6.35)
    {
        msg->mb[index].valid |= MASK_B2;
        return false;
    }
    else
    {
        msg->mb[index].valid &= ~MASK_B2;
        return true;
    }
}

/* B3 Methods ------------------------------------------------------*/
double
getLmt1B3(struct laasMessageType1 *msg, int index)
{
    return (double)(msg->mb[index].B3);
}

void
setLmt1B3(struct laasMessageType1 *msg, int index, double param)
{
    msg->mb[index].B3 = param;
    isValidLmt1B3(msg,index);
}

int
isValidLmt1B3(struct laasMessageType1 *msg, int index)
{
    /* Range -6.35 - +6.35 */
    if(msg->mb[index].B3 < -6.35 || msg->mb[index].B3 > 6.35)
    {
        msg->mb[index].valid |= MASK_B3;
        return false;
    }
    else
    {
        msg->mb[index].valid &= ~MASK_B3;
        return true;
    }
}

/* B4 Methods ------------------------------------------------------*/
double
getLmt1B4(struct laasMessageType1 *msg, int index)
{
    return (double)(msg->mb[index].B4);
}

void
setLmt1B4(struct laasMessageType1 *msg, int index, double param)
{
    msg->mb[index].B4 = param;
    isValidLmt1B4(msg,index);
}

int
isValidLmt1B4(struct laasMessageType1 *msg, int index)
{
    /* Range -6.35 - +6.35 */
    if(msg->mb[index].B4 < -6.35 || msg->mb[index].B4 > 6.35)
    {
        msg->mb[index].valid |= MASK_B4;
        return false;
    }
    else
    {
        msg->mb[index].valid &= ~MASK_B4;
        return true;
    }
}


struct laasMessageType1 *
laasMessageType1_init()
{
    struct laasMessageType1 *mt1 = 0;
    mt1 = (struct laasMessageType1 *) malloc(sizeof(struct laasMessageType1));
    memset(mt1,'\0',sizeof(struct laasMessageType1));
    return mt1;
}

struct laasMessageType1 *
laasMessageType1_delete(struct laasMessageType1 *mt1)
{
    free(mt1);
    mt1 = 0;
    return mt1;
}

void
laasMessageType1_clear(struct laasMessageType1 *mt1)
{
    memset(mt1,'\0',sizeof(struct laasMessageType1));
}

void
laasMessageType1_print(struct laasMessageType1 *msgType1, unsigned char msgData[])
{
    unsigned char *start_ptr = 0x00;
    int dataBlock = 0;
 
    start_ptr = msgData;

    printf("LAAS Message Type1:  ==========================\n");
    printf("                            RawMessage              get   isValid\n");

    printf("Message Segment: [");
    print_buffer(msgData,7);
    printf("]\n");

/***** Print ModifiedZCount *****************************************/
    printf("ModifiedZCount:                 0x%04X %16.2f  ",
                              (((msgData[1] & 0x3F) << 8) | msgData[0]),
                              getLmt1ModifiedZCount(msgType1));
    if(!isValidLmt1ModifiedZCount(msgType1))
        printf("Invalid"); 
    printf("\n"); 

/***** Print AdditionalMessageFlag **********************************/
    printf("AdditionalMessageFlag:            0x%02X %16d  ",
                              ((msgData[1] & 0xC0) >> 6),
                              getLmt1AdditionalMessageFlag(msgType1));
    if(!isValidLmt1AdditionalMessageFlag(msgType1))
        printf("Invalid"); 
    printf("\n"); 

/***** Print NumberOfMeasurements ***********************************/
    printf("NumberOfMeasurements:             0x%02X %16d  ",
                              (msgData[2] & 0x1F),
                              getLmt1NumberOfMeasurements(msgType1));
    if(!isValidLmt1NumberOfMeasurements(msgType1))
        printf("Invalid"); 
    printf("\n"); 

/***** Print MeasurementType ****************************************/
    printf("MeasurementType:                  0x%02X %16d  ",
                              ((msgData[2] & 0xE0)>>5),
                              getLmt1MeasurementType(msgType1));
    switch(getLmt1MeasurementType(msgType1))
    {
        case 0:
            printf("  C/A L1"); 
            break;
        case 1:
            printf("  C/A L2"); 
            break;
        case 2:
            printf(" P(Y) L1"); 
            break;
        case 3:
            printf(" P(Y) L2"); 
            break;
        case 4: case 5: case 6: case 7:
            printf("Reserved"); 
            break;
        default :
            printf("Invalid"); 
            break;
    }
    printf("\n"); 

/***** Print EphemerisDecorrelation *********************************/
    printf("EphemerisDecorrelation:           0x%02X %16.7f   ",
                              msgData[3],
                              getLmt1EphemerisDecorrelation(msgType1));
    if(!isValidLmt1EphemerisDecorrelation(msgType1))
        printf("Invalid"); 
    printf("\n"); 
/***** Print EphemerisCRC *******************************************/
    printf("EphemerisCRC:  %d               0x%04X %16d\n",
                              getLmt1RangingSourceID(msgType1,0),
                              ((msgData[5]<<8) | msgData[4]),
                              getLmt1EphemerisCRC(msgType1));

/***** Print SourceAvailabilityDuration *****************************/
    printf("SourceAvailabilityDuration:       0x%02X %16d   ",
                              msgData[6],
                              getLmt1SourceAvailabilityDuration(msgType1));
    if(!isValidLmt1SourceAvailabilityDuration(msgType1))
        printf("Not Provided"); 
    printf("\n"); 

    msgData = &msgData[7];
    for (dataBlock=0; dataBlock < msgType1->nom; dataBlock++) 
    {
        printf("-------------------------------------------\n");
        printf("Message Segment: [");
        print_buffer(msgData,11);
        printf("]\n");
/***** Print RangingSourceID ****************************************/
        printf("RangingSourceID:                  0x%02X %16d   ",
                              msgData[0],
                              getLmt1RangingSourceID(msgType1,dataBlock));
        if(!isValidLmt1RangingSourceID(msgType1,dataBlock))
            printf("Invalid"); 
        printf("\n"); 

/***** Print IssueOfData ********************************************/
        printf("IssueOfData:                      0x%02X %16d\n",
                              msgData[1],
                              getLmt1IssueOfData(msgType1,dataBlock));

/***** Print PseudorangeCorrection **********************************/
        printf("PseudorangeCorrection:          0x%04X %16.3f   ",
                              (msgData[3]<<8) | msgData[2],
                              getLmt1PseudorangeCorrection(msgType1,dataBlock));
        if(!isValidLmt1PseudorangeCorrection(msgType1,dataBlock))
            printf("Invalid"); 
        printf("\n"); 

/***** Print RangeRateCorrection ************************************/
        printf("RangeRateCorrection:            0x%04X %16.4f   ",
                              (msgData[5]<<8) | msgData[4],
                              getLmt1RangeRateCorrection(msgType1,dataBlock));
        if(!isValidLmt1RangeRateCorrection(msgType1,dataBlock))
            printf("Invalid"); 
        printf("\n"); 

/***** Print SigmaPrGnd *********************************************/
        printf("SigmaPrGnd:                       0x%02X %16.3f   ",
                              msgData[6],
                              getLmt1SigmaPrGnd(msgType1,dataBlock));
        if(!isValidLmt1SigmaPrGnd(msgType1,dataBlock))
            printf("Invalid"); 
        printf("\n"); 

/***** Print B1 *****************************************************/
        printf("B1:                               0x%02X %16.3f  ",
                              msgData[7],
                              getLmt1B1(msgType1,dataBlock));
        if(!isValidLmt1B1(msgType1,dataBlock))
            (msgType1->mb[dataBlock].B1 >= 0) ? printf("Not Used") : printf("Not Used");
        printf("\n"); 

/***** Print B2 *****************************************************/
        printf("B2:                               0x%02X %16.3f  ",
                              msgData[8],
                              getLmt1B2(msgType1,dataBlock));
        if(!isValidLmt1B2(msgType1,dataBlock))
            (msgType1->mb[dataBlock].B2 >= 0) ? printf("Not Used") : printf("Not Used");
        printf("\n"); 

/***** Print B3 *****************************************************/
        printf("B3:                               0x%02X %16.3f  ",
                              msgData[9],
                              getLmt1B3(msgType1,dataBlock));
        if(!isValidLmt1B3(msgType1,dataBlock))
            (msgType1->mb[dataBlock].B3 >= 0) ? printf("Not Used") : printf("Not Used");
        printf("\n"); 

/***** Print B4 *****************************************************/
        printf("B4:                               0x%02X %16.3f  ",
                              msgData[10],
                              getLmt1B4(msgType1,dataBlock));
        if(!isValidLmt1B4(msgType1,dataBlock))
            (msgType1->mb[dataBlock].B4 >= 0) ? printf("Not Used") : printf("Not Used");
        printf("\n"); 

        msgData = &msgData[11];
    }

    msgData = start_ptr;
}

void
laasMessageType1_print_csv(struct laasMessageType1 *msgType1, FILE *csv)
{
    int dataBlock = 0;
 
    if(csv)
    {
         /***** Print ModifiedZCount *****************************************/
         fprintf(csv, "%0.2f,", getLmt1ModifiedZCount(msgType1));

         /***** Print AdditionalMessageFlag **********************************/
         fprintf(csv, "%d,", getLmt1AdditionalMessageFlag(msgType1));

         /***** Print NumberOfMeasurements ***********************************/
         fprintf(csv, "%d,", getLmt1NumberOfMeasurements(msgType1));

         /***** Print MeasurementType ****************************************/
         fprintf(csv, "%d,", getLmt1MeasurementType(msgType1));

         /***** Print EphemerisDecorrelation *********************************/
         fprintf(csv, "%0.8f,", getLmt1EphemerisDecorrelation(msgType1));
         /***** Print EphemerisCRC *******************************************/
         fprintf(csv, "0x%04X,", getLmt1EphemerisCRC(msgType1));

         /***** Print SourceAvailabilityDuration *****************************/
         fprintf(csv,"%d,", getLmt1SourceAvailabilityDuration(msgType1));

         for (dataBlock=0; dataBlock<msgType1->nom; dataBlock++) 
         {
              /***** Print RangingSourceID ****************************************/
              fprintf(csv, "%d,", getLmt1RangingSourceID(msgType1,dataBlock));

              /***** Print IssueOfData ********************************************/
              fprintf(csv, "%d,", getLmt1IssueOfData(msgType1,dataBlock));

              /***** Print PseudorangeCorrection **********************************/
              fprintf(csv, "%0.3f,", getLmt1PseudorangeCorrection(msgType1,dataBlock));

              /***** Print RangeRateCorrection ************************************/
              fprintf(csv, "%0.4f,", getLmt1RangeRateCorrection(msgType1,dataBlock));

              /***** Print SigmaPrGnd *********************************************/
              fprintf(csv, "%0.3f,", getLmt1SigmaPrGnd(msgType1,dataBlock));

              /***** Print B1 *****************************************************/
              fprintf(csv, "%0.3f,", getLmt1B1(msgType1,dataBlock));

              /***** Print B2 *****************************************************/
              fprintf(csv, "%0.3f,", getLmt1B2(msgType1,dataBlock));

              /***** Print B3 *****************************************************/
              fprintf(csv, "%0.3f,", getLmt1B3(msgType1,dataBlock));

              /***** Print B4 *****************************************************/
              fprintf(csv, "%0.3f,", getLmt1B4(msgType1,dataBlock));
         }
         fflush(csv);
    }
}

/* setType1DataBlock -----------------------------------------------*/
void
laasMessageType1_process(struct laasMessageType1 *msgType1, unsigned char msgData[])
{
    char ch_temp = 0;
    int dataBlock = 0;

    laasMessageType1_clear(msgType1);

/* setModifiedZCount */
    msgType1->mzc =  (0.1 * (((msgData[1] & 0x3F) << 8) | msgData[0]));

/* setAdditionalMessageFlag */
    msgType1->amf = ((msgData[1] & 0xC0) >> 6);

/* setNumberOfMeasurements  */
    msgType1->nom = (msgData[2] & 0x1F);

/* setMeasurementType */
    msgType1->mt = ((msgData[2] & 0xE0)>>5);

/* setEphemerisDecorrelation */
    msgType1->ed = (5e-6 * msgData[3]);

/* setEphemerisCRC */
    msgType1->ecrc = ((msgData[5]<<8) | msgData[4]);

/* setSourceAvailabilityDuration */
    msgType1->sad = (10 * msgData[6]);

/* cycle through and add each measurement */
    msgData = &msgData[7];
    for (dataBlock=0; dataBlock<msgType1->nom; dataBlock++) 
    {

/* Ranging Source ID */
        msgType1->mb[dataBlock].rs_id = msgData[0];

/* Issue of Data */
        msgType1->mb[dataBlock].iod   = msgData[1];

/* Pseudorange Correction */
        ch_temp = msgData[3]; 
        msgType1->mb[dataBlock].prc = (((ch_temp << 8) | msgData[2]) * 0.01);

/* Range Rate Correction */
        ch_temp = msgData[5]; 
        msgType1->mb[dataBlock].rrc = (((ch_temp <<8) | msgData[4]) * 0.001);

/* Sigma pr_grn */
        msgType1->mb[dataBlock].spg = msgData[6] * 0.02;

/* B1 */
        ch_temp = msgData[7]; 
        msgType1->mb[dataBlock].B1 = ch_temp * 0.05;

/* B2 */
        ch_temp = msgData[8]; 
        msgType1->mb[dataBlock].B2 = ch_temp * 0.05;

/* B3 */
        ch_temp = msgData[9]; 
        msgType1->mb[dataBlock].B3 = ch_temp * 0.05;

/* B4 */
        ch_temp = msgData[10]; 
        msgType1->mb[dataBlock].B4 = ch_temp * 0.05;
 
        msgData = &msgData[11];
    }
    setLmt1Validity(msgType1);
}


int
getLmt1AsByteArray(struct laasMessageType1 *mt1, unsigned char msgData[])
{
    int dataBlock = 0;
    int size = 7;
    double dbl = 0;
    long   lng = 0;

    size += (11 * getLmt1NumberOfMeasurements(mt1));

    /* First 7 bytes are constant size */
    /* plus 11 byes for each Measurement */
    

/* You have to to do the divide first with double then cast to unsigned int
   or else the value will be less by 1                                       */
    dbl = (mt1->mzc / 0.1);
    lng = lrint(dbl);
    msgData[0] = (lng & 0x000000FF);
    msgData[1] = ((lng >> 8) & 0x0000003F);

    msgData[1] |= mt1->amf << 6;

    msgData[2] = mt1->nom;
    msgData[2] |= (mt1->mt << 5) ;
    
    dbl = (mt1->ed / 5e-6);
    lng = lrint(dbl);
    msgData[3] = lng & 0x000000FF;

    msgData[4] = mt1->ecrc & 0x000000FF;
    msgData[5] = ((mt1->ecrc >> 8) & 0x000000FF);

    msgData[6] = (mt1->sad / 10);
    
    msgData = &msgData[7];
    for (dataBlock=0; dataBlock<mt1->nom; dataBlock++) 
    {
/* Ranging Source ID */
        msgData[0] = mt1->mb[dataBlock].rs_id;

/* Issue of Data */
        msgData[1] = mt1->mb[dataBlock].iod;

/* Pseudorange Correction */
        dbl = (mt1->mb[dataBlock].prc / 0.01);
        lng = lrint(dbl);
        msgData[2] = lng & 0x000000FF;
        msgData[3] = ((lng >> 8) & 0x000000FF);

/* Range Rate Correction */
        dbl = (mt1->mb[dataBlock].rrc / 0.001);
        lng = lrint(dbl);
        msgData[4] = lng & 0x000000FF;
        msgData[5] = ((lng >> 8) & 0x000000FF);

/* Sigma pr_grn */
        dbl = (mt1->mb[dataBlock].spg / 0.02);
        lng = lrint(dbl);
        msgData[6] = (lng & 0x000000FF);

/* B1 */
        dbl = (mt1->mb[dataBlock].B1 / 0.05);
        lng = lrint(dbl);
        msgData[7] = (lng & 0x000000FF);

/* B2 */
        dbl = (mt1->mb[dataBlock].B2 / 0.05);
        lng = lrint(dbl);
        msgData[8] = (lng & 0x000000FF);

/* B3 */
        dbl = (mt1->mb[dataBlock].B3 / 0.05);
        lng = lrint(dbl);
        msgData[9] = (lng & 0x000000FF);

/* B4 */
        dbl = (mt1->mb[dataBlock].B4 / 0.05);
        lng = lrint(dbl);
        msgData[10] = (lng & 0x000000FF);
 
        msgData = &msgData[11];
    }
    return size;
}

void
setLmt1Validity(struct laasMessageType1 *msg)
{
    int counter = 0;

    isValidLmt1ModifiedZCount(msg);
    isValidLmt1AdditionalMessageFlag(msg);
    isValidLmt1NumberOfMeasurements(msg);
    isValidLmt1MeasurementType(msg);
    isValidLmt1EphemerisDecorrelation(msg);
    isValidLmt1SourceAvailabilityDuration(msg);

    for(counter = 0; counter < msg->nom ; counter++)
    {    
        isValidLmt1RangingSourceID(msg, counter);
        isValidLmt1PseudorangeCorrection(msg, counter);
        isValidLmt1RangeRateCorrection(msg, counter);
        isValidLmt1SigmaPrGnd(msg, counter);
        isValidLmt1B1(msg, counter);
        isValidLmt1B2(msg, counter);
        isValidLmt1B3(msg, counter);
        isValidLmt1B4(msg, counter);
    }
}
