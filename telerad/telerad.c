/* GNU_SOURCE defines memmem in string.h */
#define _GNU_SOURCE
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef LAAS_H
#include "../laas.h"
#endif

#ifndef UTILS_STRING_H
#include "utils_string.h"
#endif

#ifndef CRC_32_BIT_H
#include "crc_32bit.h"
#endif

#ifndef LAAS_MESSAGE_TYPE_1_H
#include "laasMessageType1.h"
#endif

#ifndef LAAS_MESSAGE_H
#include "laasMessage.h"
#endif

#ifndef TELERAD_H
#include "telerad.h"
#endif

unsigned char
getTlmMS(struct telerad_laas_message *tlm)
{
     return tlm->ms;
}

void
setTlmMS(struct telerad_laas_message *tlm, unsigned char var)
{
     tlm->ms = var;
}

unsigned char
getTlmRSSI(struct telerad_laas_message *tlm)
{
     return tlm->rssi;
}

void
setTlmRSSI(struct telerad_laas_message *tlm, unsigned char var)
{
     tlm->rssi = var;
}

unsigned char
getTlmTDMA(struct telerad_laas_message *tlm)
{
     return tlm->tdma;
}
     
void
setTlmTDMA(struct telerad_laas_message *tlm, unsigned char var)
{
     tlm->tdma = var;
}

int
getTmAsByteArray(struct str_telerad_message *tm,
      unsigned char rawMsg[])
{

     int size = 0;

     rawMsg[0] = tm->ps[0];
     rawMsg[1] = tm->ps[1];
     rawMsg[2] = 0x5C;      /* 5C is a LAAS Message */

     setTlmMS(&tm->tlm, 0x00);     /* Message Status */
     setTlmRSSI(&tm->tlm, 0xFF);   /* Received Signal Strength Indicatior */

     size = getTlmAsByteArray(&tm->tlm,&rawMsg[4]);

     rawMsg[3] = size;
     tm->length = size; /* set the lenght */

     size += 4; /* 0-4 FF005C + Size */

     tm->checksum = telerad_generate_checksum(rawMsg,size);

     /* Two Byte Checksum */
     size += 2;

     return size;
}


int
getTlmAsByteArray(struct telerad_laas_message *tlm, unsigned char message[])
{
     int size = 3;
     message[0] = tlm->ms;
     message[1] = tlm->rssi;
     message[2] = tlm->tdma;

     size += getLaasMessageAsByteArray(&tlm->lm,&message[3]);

     return size;

}

struct telerad_laas_message *
telerad_laas_message_init()
{
    struct telerad_laas_message *tlm;
    tlm = (struct telerad_laas_message *) malloc(sizeof(struct telerad_laas_message));
    if(!tlm)
    {   
        printf("Error: (1) Memory Not allocated\n");
        exit(0);
    }
    memset(tlm,'\0',sizeof(struct telerad_laas_message));
    return tlm;
}

struct telerad_laas_message *
telerad_laas_message_delete(struct telerad_laas_message *tlm)
{
    if(tlm)
        free(tlm);
    tlm = 0;
    return tlm;
}

void
telerad_laas_message_clear(struct telerad_laas_message *tlm)
{
    laasMessage_clear(&tlm->lm);

    tlm->ms = 0;
    tlm->rssi = 0;
    tlm->tdma = 0;
}

struct str_telerad_message *
telerad_init(struct str_telerad_message *tm)
{
    tm = (struct str_telerad_message *) malloc(sizeof(struct str_telerad_message));
    if(!tm)
    {   printf("Error: (1) Memory Not allocated\n");
        exit(0);
    }
    memset(tm,'\0',sizeof(struct str_telerad_message));
    tm->checksum_valid = true;
    return tm;
}

void
telerad_clear(struct str_telerad_message *tm)
{
    memset(tm,'\0',sizeof(struct str_telerad_message));
    tm->checksum_valid = true;
}

void
telerad_cat_1_message_process(struct str_telerad_message *tm, unsigned char rawMsg[])
{
    unsigned char *rawMsgStart = 0;

    rawMsgStart = rawMsg;

    tm->tlm.ms = rawMsg[0];
    tm->tlm.rssi = rawMsg[1];
    tm->tlm.tdma = rawMsg[2];

    /*  Switching to Document D0-246B */
    rawMsg = &rawMsg[3];

    laasMessage_process(&tm->tlm.lm,rawMsg);
} 

void
telerad_status_process(struct str_telerad_message *tm, unsigned char rawMsg[])
{
    tm->trs.rf   = rawMsg[0];
    tm->trs.flb  = rawMsg[1];
    tm->trs.fhb  = rawMsg[2];
    tm->trs.dbr[0]  = rawMsg[5];
    tm->trs.dbr[1]  = rawMsg[6];
}

void
telerad_command_acknowledgement_process(struct str_telerad_message *tm, unsigned char rawMsg[])
{
    tm->trca.ca = rawMsg[0];
}


void
telerad_laas_message_print(struct str_telerad_message *tm, unsigned char rawMsg[])
{
    int  frame = 0;
    char time_slot_array[7] = {'A','B','C','D','E','F','G'};

/* Process Telerad Header Information */
    printf("Telerad LAAS Message:  =========================\n");
    printf("Message Segment: [");
    print_buffer(rawMsg,3);
    printf("]\n");
    printf("Unused:          %X\n",(tm->tlm.ms & CAT1_MS_UNUSED) >> 5);
    printf("SSID:            %X\n",(tm->tlm.ms & CAT1_MS_SSID) >> 2);
    printf("FEC Enabled:     %X\n",tm->tlm.ms & CAT1_MS_FEC);
    printf("FEC Applied:     %X\n",tm->tlm.ms & CAT1_MS_FEC_APPLIED);
    printf("Signal Strength: %02X\n",tm->tlm.rssi);
    frame = ((time_slot_array[(tm->tlm.tdma >> 4)]) < 8 ) ? 1:2;
    printf("TDMA Slot->Frame:%d, Slot:%c\n",frame,
                      (time_slot_array[(tm->tlm.tdma >> 4) % 8]));
    laasMessage_print(&tm->tlm.lm,&rawMsg[3]);
}

void
telerad_laas_message_print_csv(struct str_telerad_message *tm, unsigned char rawMsg[])
{
    int  frame = 0;
    char time_slot_array[7] = {'A','B','C','D','E','F','G'};

/* Process Telerad Header Information */
    printf("0x%X,",(tm->tlm.ms & CAT1_MS_UNUSED) >> 5);
    printf("0x%X,",(tm->tlm.ms & CAT1_MS_SSID) >> 2);
    printf("0x%X,",tm->tlm.ms & CAT1_MS_FEC);
    printf("0x%X,",tm->tlm.ms & CAT1_MS_FEC_APPLIED);
    printf("0x%02X,",tm->tlm.rssi);
    frame = ((time_slot_array[(tm->tlm.tdma >> 4)]) < 8 ) ? 1:2;
    printf("%d,\'%c\',",frame, (time_slot_array[(tm->tlm.tdma >> 4) % 8]));
    laasMessage_print_csv(&tm->tlm.lm, NULL); /* FIX NULL */
}

void
telerad_status_print(struct str_telerad_message *tm, unsigned char trs[])
{
    unsigned int frequency_KHz = 0;
    float        frequency_MHz = 0;
    printf("Telerad Status Message:  =======================\n");
    printf("unused:                             %02X\n", tm->trs.rf & RS_RECEIVER_FALURE_UNUSED);
    printf("receiver_bit_failure:               %X\n", tm->trs.rf & RS_RECEIVER_BIT_FAILURE);

    frequency_KHz = (unsigned int) (tm->trs.fhb << 8 | tm->trs.flb);
    frequency_KHz += 100000;
    frequency_MHz = (float) frequency_KHz / 1000;
    printf("receiver_frequency:                 %0.3fMhz\n", frequency_MHz);

    printf("receiver_failure:                   %X\n", tm->trs.dbr[0] & RS_RECEIVER_FAILURE);
    printf("receiver_not_ready:                 %X\n", tm->trs.dbr[0] & RS_RECEIVER_NOT_READY);
    printf("frequency_synthesizer_out_out_lock: %X\n", tm->trs.dbr[0] & RS_FREQUENCY_SYNTHESIZER_OUT_OF_LOCK);
    printf("spare1:                             %X\n", tm->trs.dbr[0] & RS_SPARE1);
    printf("VDC_supply_too_high:                %X\n", tm->trs.dbr[0] & RS_VDC_SUPPLY_TOO_HIGH);
    printf("VDC_supply_too_low:                 %X\n", tm->trs.dbr[0] & RS_VDC_SUPPLY_TOO_LOW);
    printf("GPS_1PPS_not_present:               %X\n", (tm->trs.dbr[0] & RS_GPS_1PPS_NOT_PRESENT) >> 1);
    printf("spare2:                             %X\n", tm->trs.dbr[0] & RS_SPARE2);
    printf("EEPROM_Error:                       %X\n", tm->trs.dbr[1] & RS_EEPROM_ERROR);
    printf("Microprocessor_Ram_Error:           %X\n", tm->trs.dbr[1] & RS_MICROPROCESSOR_RAM_ERROR);
    printf("Flash_Memory_Error:                 %X\n", tm->trs.dbr[1] & RS_FLASH_MEMORY_ERROR);
    printf("DSP_RAM_Error:                      %X\n", tm->trs.dbr[1] & RS_DSP_RAM_ERROR);
    printf("DSP_Error:                          %X\n", tm->trs.dbr[1] & RS_DSP_ERROR);
    printf("spare3:                             %X\n", tm->trs.dbr[1] & RS_SPARE3);
    printf("spare4:                             %X\n", tm->trs.dbr[1] & RS_SPARE4);
    printf("reserved1:                          %02X\n", tm->trs.dbr[1] & RS_RESERVED1);
}

void
telerad_command_acknowledgement_print(struct str_telerad_message *tm, unsigned char trca[])
{
    printf("Telerad Command Acknowledgement:  ==============\n");
    printf("Message Segment: [%02X]\n",trca[0]);
 
    printf("Unused:             %02X\n", tm->trca.ca & CA_UNUSED);
    printf("Command_Successful: %X\n", tm->trca.ca & CA_COMMAND_SUCCESSFUL);
}

void
telerad_print(struct str_telerad_message *tm, unsigned char rawMsg[], int length)
{
    if(!tm->checksum_valid)
    {
        printf("Error: Checksum Invalid\n");
        return;
    }

    printf("Telerad Header:  ===============================\n");
    printf("Message Segment: [");
    print_buffer(rawMsg,4);
    printf("...");
    print_buffer(&rawMsg[length-2],2);
    printf("]\n");
    printf("                 (Raw)   (tm)\n");
    printf("Packet Sync(1):  0x%02X    0x%02X\n",rawMsg[0], tm->ps[0]);
    printf("Packet Sync(2):  0x%02X    0x%02X\n",rawMsg[1], tm->ps[1]);
    printf("Message Id:      0x%02X    0x%02X ",rawMsg[2], tm->ms_id);
    switch(tm->ms_id)
    {
        case 0x5C:  printf("Telerad Receiver LAAS Message\n");
                    break;
        case 0x53:  printf("Telerad Receiver Status Message\n");
                    break;
        case 0x96:  printf("Telerad Receiver Command Acknowledgement Message\n");
                    break;
        default:    printf("Error: Invalid VDB Receiver Message Type\n");
    }
    printf("Length:          0x%02X      %02d Bytes\n",rawMsg[3], tm->length);
    printf("Checksum:        0x%04X  0x%04X\n",rawMsg[length-2]<<8|rawMsg[length-1],tm->checksum);

    switch(tm->ms_id)
    {
        case 0x5C:  telerad_laas_message_print(tm,&rawMsg[4]);
                    break;
        case 0x53:  telerad_status_print(tm,&rawMsg[4]);
                    break;
        case 0x96:  telerad_command_acknowledgement_print(tm,&rawMsg[4]);
                    break;
        default:    printf("Error: Invalid VDB Receiver Message Type\n");
    }
    printf("\n");
}

void
telerad_print_csv(struct str_telerad_message *tm, unsigned char rawMsg[], int length)
{
    if(!tm->checksum_valid)
    {
        printf("Error: Checksum Invalid\n");
        return;
    }

    printf("0x%02X",tm->ps[0]);
    printf("%02X,",tm->ps[1]);
    printf("0x%02X,",tm->ms_id);
    printf("%d,",tm->length);

    switch(tm->ms_id)
    {
        case 0x5C:  telerad_laas_message_print_csv(tm,&rawMsg[4]);
                    break;
        case 0x53:  telerad_status_print(tm,&rawMsg[4]);
                    break;
        case 0x96:  telerad_command_acknowledgement_print(tm,&rawMsg[4]);
                    break;
        default:    printf("Error: Invalid VDB Receiver Message Type\n");
    }
    printf("0x%04X\n",tm->checksum);
    printf("\n");
}

int
telerad_validate_checksum(unsigned char msg[],int size)
{
    unsigned int total = 0;
    int counter = 0;

    for(counter = 0; counter < size - 2; counter++)
        total += msg[counter];
    
    if(!((total ^ ((msg[counter+1] << 8) | msg[counter])) == 0))
        return false;
    else
        return true;
}

unsigned int
telerad_generate_checksum(unsigned char msg[],int size)
{
    unsigned int total = 0;
    int counter = 0;

    for(counter = 0; counter < size; counter++)
        total += msg[counter];

    msg[size]   = total;
    msg[size+1] = total >> 8;
    
    printf("%X, %X, %X\n",total, ((total & 0xFF) << 8),((total & 0xFF00) >> 8)); 

    return (((total & 0xFF) << 8) | ((total & 0xFF00) >> 8));
}

void telerad_sync_message(unsigned char buf[],int *size)
{
     bool synced = false;
     unsigned char *search_str_ptr = '\0';
     unsigned char search_str[] = {0xFF, 0x00};

     unsigned char *buffer_start = buf;
     int size_start = (*size);

/*     print_buffer(buf,(*size)); printf("\n"); */

     search_str_ptr = memmem(&buf[1],(*size),search_str,2);
     if(search_str_ptr != '\0')
     {
          buf = search_str_ptr;
          printf("Not Null\n");
          if((buf[0] == 0xFF) && (buf[1] == 0x00) &&
          ((buf[2] == 0x53) || (buf[2] == 0x5C) || (buf[2] == 0x96)))
          {
               synced = true;
               (*size) = (buffer_start+size_start) - search_str_ptr;
               memmove(buffer_start,search_str_ptr,(buffer_start+size_start) - search_str_ptr);
          }
     } else {
          printf("Null\n");
          (*size) = 0;
          memset(buffer_start,'\0',size_start);
     }
}

int
telerad_process_incoming(struct str_telerad_message *tm, unsigned char buf[], int *size, int *messageSize)
{
     int return_val = 0;

     if((*size) < 7) /* 6 Bytes is the shortes message possible */
     {
          return 0;
     }

     if((buf[0] == 0xFF) && (buf[1] == 0x00) &&
        ((buf[2] == 0x53) || (buf[2] == 0x5C) || (buf[2] == 0x96)))
     {
          (*messageSize) = buf[3] + 6;
          if((*messageSize) > (*size))
          {
               /* get more data */
               return_val = 0;
               /* return 0; */
          } else {
               /* process the message */
               tm->ms_id = 0;
               tm->tlm.lm.mt = 0;
               telerad_process(tm, buf, (*messageSize));
               return_val = 1;
          }
     } else {
          printf("Invalid Message Type\n");
          printf("Size before sync: %d\n",(*size));
          telerad_sync_message(buf,size);
          printf("Size after sync: %d\n",(*size));
          return_val = -1;
     }

     /* printf("telerad_process_incoming(): Message Type: %d\n", tm->tlm.lm.mt); */
     return return_val;
}

void
telerad_process(struct str_telerad_message *tm, unsigned char telerad_message[], int num_bytes)
{

    tm->checksum_valid = false;

    if(!(telerad_validate_checksum(telerad_message,num_bytes)))
    {
        printf("Error: Telerad Checksum Invalid\n");
        return;
    }

    tm->checksum_valid = true;

    tm->ps[0] = telerad_message[0];
    tm->ps[1] = telerad_message[1];
    tm->ms_id = telerad_message[2];
    tm->length = telerad_message[3];
    tm->checksum = telerad_message[num_bytes - 2]<<8 | telerad_message[num_bytes - 1];

    switch(tm->ms_id)
    {
        case 0x5C:  telerad_cat_1_message_process(tm,&telerad_message[4]);
                    break;
        case 0x53:  telerad_status_process(tm,&telerad_message[4]);
                    break;
        case 0x96:  telerad_command_acknowledgement_process(tm,&telerad_message[4]);
                    break;
        default:    printf("Error: Invalid VDB Receiver Message Type\n");
    }
    /* printf("telerad_process(): Message Type: %d\n\n", tm->tlm.lm.mt); */
}
