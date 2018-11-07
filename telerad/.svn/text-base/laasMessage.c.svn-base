#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef LAAS_H
#include "../laas.h"
#endif

#ifndef LAAS_MESSAGE_H
#include "laasMessage.h"
#endif

#ifndef LAAS_MESSAGE_TYPE_1_H
#include "laasMessageType1.h"
#endif

#ifndef LAAS_MESSAGE_TYPE_2_H
#include "laasMessageType2.h"
#endif

#ifndef LAAS_MESSAGE_TYPE_4_H
#include "laasMessageType4.h"
#endif

#ifndef UTILS_STRING_H
#include "utils_string.h"
#endif

#ifndef CRC_32_BIT_H
#include "crc_32bit.h"
#endif



/************************************************************
   2       0xFF00

   1       Message ID 
           Transmitter Input
           0xA3 Transmitter Contol
           0xAC Transmitter Reset
           0x5C Tx CAT-I Message
           Transmitter Output
           0x53 Transmitter Status
           0x96 Command Acknowledgment
           Receiver Input
           0xA3 Receiver Contol
           0xAC Receiver Reset
           0xA6 Application Data FEC
           Receiver Output
           0x5C Rx CAT-I Message
           0x53 Receiver Status
           0x96 Command Acknowledgment

   1       Number of bytes in the application data section

   0 - 255 Each byte can be any 8-bit value

   2       16 bit additive checksum (LSB First)
************************************************************/

void
setlaasMessageMBI(struct laasMessage *laasMsg, unsigned int mbi)
{
    laasMsg->mbi = mbi;
}

unsigned int
getlaasMessageMBI(struct laasMessage *laasMsg)
{
    return laasMsg->mbi;
}

int
isValidlaasMessageMBI(struct laasMessage *laasMsg)
{
    /* Range  0xAA - Normal    */
    /* Range  0xFF - Test      */
    /* Range  All Else Invalid */
    if(laasMsg->mbi == 0xAA || laasMsg->mbi == 0xFF)
        return true;
    else
        return false;
}

void
setlaasMessageGbasId(struct laasMessage *laasMsg, char gbas_id[])
{
    laasMsg->gbas_id[0] = gbas_id[0];
    laasMsg->gbas_id[1] = gbas_id[1];
    laasMsg->gbas_id[2] = gbas_id[2];
    laasMsg->gbas_id[3] = gbas_id[3];
}

char *
getlaasMessageGbasId(struct laasMessage *laasMsg)
{
    return laasMsg->gbas_id;
}

void
setlaasMessageMT(struct laasMessage *laasMsg, unsigned int mt)
{
    laasMsg->mt = mt;
}

unsigned int
getlaasMessageMT(struct laasMessage *laasMsg)
{
    return laasMsg->mt;
}

void
setlaasMessageLength(struct laasMessage *laasMsg, unsigned int length)
{
    laasMsg->length = length;
}

unsigned int
getlaasMessageLength(struct laasMessage *laasMsg)
{
    return laasMsg->length;
}

/****************************************************************/
struct laasMessage *
laasMessage_init()
{
    struct laasMessage *lm;
    lm = (struct laasMessage *) malloc(sizeof(struct laasMessage));
    if(!lm)
    {
        printf("Error: (1) Memory Not allocated\n");
        exit(0);
    }
    memset(lm,'\0',sizeof(struct laasMessage));
    lm->valid = true;
    return lm;
}

struct laasMessage *
laasMessage_delete(struct laasMessage *lm)
{
    if(lm)
        free(lm);
    lm = 0;
    return lm;
}

void
laasMessage_clear(struct laasMessage *lm)
{
    memset(lm,'\0',sizeof(struct laasMessage));
    lm->valid = true;
}

/****************************************************************/


void
laasMessage_print(struct laasMessage *laasMsg, unsigned char rawMsg[])
{
    if(!laasMsg->valid)
    {
        printf("Error: CRC Invalid\n");
        return;
    }

    printf("LAAS MESSAGE:  =================================\n");
    if(laasMsg->mbi == 0xAA)
        printf("Status:                     0xAA     [Normal]\n");
    else if(laasMsg->mbi == 0xFF)
        printf("Status:                     0xFF     [Test]\n");
    else
        printf("Error Undefined Message (AA)Normal (FF)Test\n");

    printf("GBAS ID:                    [%s]\n",laasMsg->gbas_id);

    printf("LAAS Message Type:          %02X\n",laasMsg->mt);
    printf("Message Length:             0x%02X     [%d]\n",laasMsg->length,laasMsg->length);

    switch(laasMsg->mt)
    {
        case 0x01:  laasMessageType1_print(&(*laasMsg).mt1,&rawMsg[6]);
                    break;
        case 0x02:  laasMessageType2_print(&(*laasMsg).mt2,&rawMsg[6]);
                    break;
        case 0x04:  laasMessageType4_print(&(*laasMsg).mt4,&rawMsg[6]);
                    break;
        default:    printf("Error: Invalid Message Type\n");
    }

}

void
laasMessage_print_csv(struct laasMessage *laasMsg, FILE *csv)
{
     if(!laasMsg->valid)
     {
          printf("Error: CRC Invalid\n");
          return;
     }

     if(csv)
     {
          fprintf(csv, "%X, %s, %d, %d, ",
                    laasMsg->mbi,
                    laasMsg->gbas_id,
                    laasMsg->mt,
                    laasMsg->length);

          switch(laasMsg->mt)
          {
               case 0x01:  laasMessageType1_print_csv(&(*laasMsg).mt1,csv);
                           break;
               case 0x02:  laasMessageType2_print_csv(&(*laasMsg).mt2,NULL); /* FIX the NULL */
                           break;
               case 0x04:  laasMessageType4_print_csv(&(*laasMsg).mt4,NULL); /* FIX the NULL */
                           break;
               default:    printf("Error: Invalid Message Type\n");
          }
          fprintf(csv, "\n");
          fflush(csv);
     } else {
          printf("FILE NOT OPEN\n");
     }
}

int
getLaasMessageAsByteArray(struct laasMessage *laasMsg, unsigned char message[])
{
     unsigned long crc = 0;
     int size = 6;

     unsigned int gbas_id = 0;

     message[0] = laasMsg->mbi;

/**************************************************************************************/


    gbas_id = ia_5_to_bin[(unsigned int)laasMsg->gbas_id[0]] << 18;
    gbas_id |= ia_5_to_bin[(unsigned int)laasMsg->gbas_id[1]] << 12;
    gbas_id |= ia_5_to_bin[(unsigned int)laasMsg->gbas_id[2]] << 6;
    gbas_id |= ia_5_to_bin[(unsigned int)laasMsg->gbas_id[3]];

    message[1] = gbas_id;
    message[2] = gbas_id >> 8;
    message[3] = gbas_id >> 16;

/*

    printf("MSG: %02X%02X%02X\n",message[1], message[2], message[3]);
    test =  ia_5_to_bin[laasMsg->gbas_id[0]];
    test |= ia_5_to_bin[laasMsg->gbas_id[1]] << 6;
    test |= ia_5_to_bin[laasMsg->gbas_id[2]] << 12;
    test |= ia_5_to_bin[laasMsg->gbas_id[3]] << 18;
    
    for(i=3;i>=0;i--)
    {
        laasMsg->gbas_id[i] = ia_5[(char_string & 0x3F)];
        char_string = (char_string>>6);
    }

      ia_5_to_bin

*/

/**************************************************************************************/
     message[4] = laasMsg->mt;
     
     switch(laasMsg->mt)
     {
          case 1: size += getLmt1AsByteArray(&laasMsg->mt1, &message[6]);
                  break;
          case 2: size += getLmt2AsByteArray(&laasMsg->mt2, &message[6]);
                  break;
          case 4: size += getLmt4AsByteArray(&laasMsg->mt4, &message[6]);
                  break;
     }

     printf("Message Size: %d\n",size);

     message[5] = size + 4;
     laasMsg->length = size + 4;
     
     crc = generate_crc_32bit(message,size);
     laasMsg->crc = crc;
     message[size++] = crc;
     message[size++] = crc >> 8;
     message[size++] = crc >> 16;
     message[size++] = crc >> 24;

     printf("Return Message Size: %d\n",size);

     return size;
}


void
laasMessage_process(struct laasMessage *laasMsg, unsigned char rawMsg[])
{
    unsigned int char_string = 0;
    int i = 0;

    if(!calculate_crc_32bit(&rawMsg[0],rawMsg[5]))
    {
        laasMsg->valid = false;
        return;
    }

    laasMsg->valid = true;

    laasMsg->mbi        = rawMsg[0];

    memset(laasMsg->gbas_id, '\0', 5);
    char_string = (rawMsg[3]<<16) |  (rawMsg[2]<<8) |  (rawMsg[1]);
    for(i=3;i>=0;i--)
    {
        laasMsg->gbas_id[i] = ia_5[(char_string & 0x3F)];
        char_string = (char_string>>6);
    }

    laasMsg->mt         = rawMsg[4];
    laasMsg->length     = rawMsg[5];
 
    switch(rawMsg[4])
    {
        case 0x01:  laasMessageType1_process(&(*laasMsg).mt1, &rawMsg[6]);
                    break;
        case 0x02:  laasMessageType2_process(&(*laasMsg).mt2, &rawMsg[6],laasMsg->length);
                    break;
        case 0x03:  /* Valid LAAS Message Type, but we do not use it */
                    break;
        case 0x04:  
/* Type 4 Messages can have multiple FAS block so the LAAS Message Length
   is the only way to determine how many FAS are in the message.
   This should come from DataSet Lenght, but the old base station does not seem to
   broadcast it           */
                    laasMessageType4_process(&(*laasMsg).mt4, &rawMsg[6],laasMsg->length);
                    break;
        case 0x05:  /* Valid LAAS Message Type, but we do not use it */
                    break;
        default:    printf("Error: Invalid CAT-1 Message Type\n");
                    /* Means message is sending out invalid messages */
                    if(calculate_crc_32bit(&rawMsg[0],laasMsg->length))
                        printf("CRC Valid\n");
                    break;
    }

    /* printf("laasMessage: Message Type: %d\n", laasMsg->mt); */

}
