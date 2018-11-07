#ifndef TELERAD_H
#define TELERAD_H 1

#include "../laas.h"
#include "laasMessage.h"

/* Command Acknowledgment*/
#define CA_UNUSED              0xFE  /* Bits 7-1 */
#define CA_COMMAND_SUCCESSFUL  0x01  /* Bit  0   */


/* Receiver Status Bit Fields */
#define RS_RECEIVER_FALURE_UNUSED            0xFE /* BYTE */
#define RS_RECEIVER_BIT_FAILURE              0x01 /* BIT  */
#define RS_RECEIVER_FAILURE                  0x80 /* BIT  */
#define RS_RECEIVER_NOT_READY                0x40 /* BIT  */
#define RS_FREQUENCY_SYNTHESIZER_OUT_OF_LOCK 0x20 /* BIT  */
#define RS_SPARE1                            0x10 /* BIT  */
#define RS_VDC_SUPPLY_TOO_HIGH               0x08 /* BIT  */
#define RS_VDC_SUPPLY_TOO_LOW                0x04 /* BIT  */
#define RS_GPS_1PPS_NOT_PRESENT              0x02 /* BIT  */
#define RS_SPARE2                            0x01 /* BIT  */
#define RS_EEPROM_ERROR                      0x80 /* BIT  */
#define RS_MICROPROCESSOR_RAM_ERROR          0x40 /* BIT  */
#define RS_FLASH_MEMORY_ERROR                0x20 /* BIT  */
#define RS_DSP_RAM_ERROR                     0x10 /* BIT  */
#define RS_DSP_ERROR                         0x08 /* BIT  */
#define RS_SPARE3                            0x04 /* BIT  */
#define RS_SPARE4                            0x02 /* BIT  */
#define RS_RESERVED1                         0x01 /* BIT  */


/* CAT 1 Message Status Bit Format */
/* From Telerad Document */
#define CAT1_MS_UNUSED      0xE0
#define CAT1_MS_SSID        0x16
#define CAT1_MS_FEC         0x02
#define CAT1_MS_FEC_APPLIED 0x01

struct telerad_laas_message
{
    unsigned int ms;     /* Message Status */
    unsigned int rssi;   /* Received Signal Strength Indicator */
    unsigned int tdma;   /* TDMA Received Frame / Slot(0-15) */

    struct laasMessage lm;
    
};

struct telerad_status
{
    unsigned int rf;
    unsigned int flb;
    unsigned int fhb;
    unsigned char dbr[2];
};

struct telerad_command_acknowledgement
{
    unsigned int ca;
};

struct str_telerad_message
{
    unsigned char ps[2];      /* Packet Sync               */
    unsigned int ms_id;      /* Message ID                */
    unsigned int length;     /* Message Length            */
    unsigned int  checksum;   /* 16 bit additive checksum  */

    unsigned checksum_valid;

    struct telerad_laas_message tlm;
    struct telerad_status trs;
    struct telerad_command_acknowledgement trca;
    /* Application Data          */
    /* 1. Laas Message           */
    /* 2. Receiver Status        */
    /* 3. Command Acknowledgment */
};

struct telerad_receiver_dev
{
	unsigned char buf[MAX_BUFFER_SIZE];	/* incoming data stream buffer */
	unsigned int buf_end;			/* end of incoming data stream */
	int fid;						/* device file descriptor */
	int device_num;					/* ID number of device */
	char port_name[80];				/* name of serial port the device is connected to */

	/* struct str_telerad_message tm; */
};

struct str_telerad_device
{
     struct str_serial_device *serial_device;
};


struct str_telerad_message * telerad_init(struct str_telerad_message *tm);
struct str_telerad_message * telerad_delete(struct str_telerad_message *tm);
int  telerad_process_incoming(struct str_telerad_message *tm, unsigned char buf[], int *size, int *messageSize);
void telerad_process(struct str_telerad_message *tm, unsigned char string[], int num_bytes);
void telerad_print(struct str_telerad_message *tm, unsigned char string[], int num_bytes);
void telerad_print_csv(struct str_telerad_message *tm, unsigned char string[], int num_bytes);
int  telerad_validate_checksum(unsigned char str_telerad_message[], int num_bytes);
unsigned int telerad_generate_checksum(unsigned char str_telerad_message[], int num_bytes);
void telerad_clear(struct str_telerad_message *tm);

void
telerad_laas_message_clear(struct telerad_laas_message *tlm);

struct telerad_laas_message *
telerad_laas_message_init();

int
getTmAsByteArray(struct str_telerad_message *tm, unsigned char rawMsg[]);

int
getTlmAsByteArray(struct telerad_laas_message *tlm, unsigned char message[]);

unsigned char
getTlmMS(struct telerad_laas_message *tlm);

void
setTlmMS(struct telerad_laas_message *tlm, unsigned char var);

unsigned char
getTlmRSSI(struct telerad_laas_message *tlm);

void
setTlmRSSI(struct telerad_laas_message *tlm, unsigned char var);

unsigned char
getTlmTDMA(struct telerad_laas_message *tlm);

void
setTlmTDMA(struct telerad_laas_message *tlm, unsigned char var);


#endif
