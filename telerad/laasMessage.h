#ifndef LAAS_MESSAGE_H
#define LAAS_MESSAGE_H 1

#include "laasMessageType1.h"
#include "laasMessageType2.h"
#include "laasMessageType4.h"

struct laasMessage
{
/* laasMessageHeader     */
    unsigned int valid;       
    unsigned int mbi;         /* Message Block Identifier */
    char gbas_id[5];           /* GBAS ID                  */
    unsigned int mt;          /* Message Type             */
    unsigned int length;      /* Message Length           */

    unsigned int  crc;

/* 
    careful, careful
    Be sure to free(laasMessage->body) before free(laasMessage)
*/
    struct laasMessageType1 mt1; 
    struct laasMessageType2 mt2; 
    struct laasMessageType4 mt4; 
};


int isValidlaasMessageMBI(struct laasMessage *laasMsg);

struct laasMessage * 
    laasMessage_init();
struct laasMessage * 
    laasMessage_delete(struct laasMessage *laasMsg);
void
    laasMessage_clear(struct laasMessage *laasMsg);

void laasMessage_process(struct laasMessage *laasMsg, unsigned char rawMsg[]);
void laasMessage_print(struct laasMessage *laasMsg, unsigned char rawMsg[]);
void laasMessage_print_csv(struct laasMessage *laasMsg, FILE *csv);
int getLaasMessageAsByteArray(struct laasMessage *laasMsg, unsigned char message[]);

int
isValidlaasMessageMBI(struct laasMessage *laasMsg);

void
setlaasMessageMBI(struct laasMessage *laasMsg, unsigned int mbi);
void
setlaasMessageGbasId(struct laasMessage *laasMsg, char gbas_id[]);
void
setlaasMessageMT(struct laasMessage *laasMsg, unsigned int mt);
void
setlaasMessageLength(struct laasMessage *laasMsg, unsigned int length);

unsigned int
getlaasMessageMBI(struct laasMessage *laasMsg);
char *
getlaasMessageGbasId(struct laasMessage *laasMsg);
unsigned int
getlaasMessageMT(struct laasMessage *laasMsg);
unsigned int
getlaasMessageLength(struct laasMessage *laasMsg);

#endif

