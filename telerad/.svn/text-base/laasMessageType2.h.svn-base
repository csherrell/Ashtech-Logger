#ifndef LAAS_MESSAGE_TYPE_2_H
#define LAAS_MESSAGE_TYPE_2_H 1

struct laasMessageType2
{
    unsigned int  valid;

    unsigned int gsrr;
    unsigned int gsad;
    unsigned int spare1;
    unsigned int gcid;
    double        lmv;
    unsigned int spare2;
    double        svig;
    unsigned int  ri;
    unsigned int  sh;
    unsigned int ru;
    double        lat;
    double        lon;
    double        rph;

    unsigned int adb;     /* Additional Data Block */

    unsigned int rsds;
    unsigned int  mud;
    double        kmepg;
    double        kmecg;
    double        kmepgl;
    double        kmecgl;
};

struct laasMessageType2 *
laasMessageType2_init();

struct laasMessageType2 *
laasMessageType2_delete(struct laasMessageType2 *mt2);

void
laasMessageType2_clear(struct laasMessageType2 *mt2);

int
getLmt2AsByteArray(struct laasMessageType2 *msgType2, unsigned char msgData[]);

void
laasMessageType2_process(struct laasMessageType2 *msgType2, unsigned char msgData[], unsigned int length);

int getLmt2AsBinaryArray(struct laasMessageType2 *msgType2, unsigned char msgData[]);

void
setValidityMessageType2(struct laasMessageType2 *msg);

void laasMessageType2_print(struct laasMessageType2 *msg, unsigned char rawMsg[]);
void laasMessageType2_print_csv(struct laasMessageType2 *msg, unsigned char rawMsg[]);
void laasMessageType2_raw_set(struct laasMessageType2 *msg, unsigned char *rawMessage);
void laasMessageType2_set(struct laasMessageType2 *laasMessageType2,
                          unsigned int gsrr,
                          unsigned int gsad,
                          unsigned int spare1,
                          unsigned int gcid,
                          int lmv,
                          unsigned int spare2,
                          unsigned int svig,
                          unsigned int ri,
                          unsigned int sh,
                          unsigned int ru,
                          int lat,
                          int lon,
                          int rph,
                          unsigned int rsds,
                          unsigned int mud,
                          unsigned int kmepg,
                          unsigned int kmecg,
                          unsigned int kmepgl,
                          unsigned int kmecgl);

unsigned int
getLmt2Adb(struct laasMessageType2 *msg);
void
setLmt2Adb(struct laasMessageType2 *msg, unsigned int val);

unsigned int
getLmt2Gsrr(struct laasMessageType2 *msg);

void
setLmt2Gsrr(struct laasMessageType2 *msg, unsigned int val);

unsigned int
getLmt2Gsad(struct laasMessageType2 *msg);

void
setLmt2Gsad(struct laasMessageType2 *msg, unsigned int val);

unsigned int
getLmt2Spare1(struct laasMessageType2 *msg);

void
setLmt2Spare1(struct laasMessageType2 *msg, unsigned int val);

unsigned int
getLmt2Gcid(struct laasMessageType2 *msg);

void
setLmt2Gcid(struct laasMessageType2 *msg, unsigned int val);

double
getLmt2Lmv(struct laasMessageType2 *msg);

void
setLmt2Lmv(struct laasMessageType2 *msg, double val);

unsigned int
getLmt2Spare2(struct laasMessageType2 *msg);

void
setLmt2Spare2(struct laasMessageType2 *msg, unsigned int val);

double
getLmt2Svig(struct laasMessageType2 *msg);

void
setLmt2Svig(struct laasMessageType2 *msg, double val);

int
getLmt2Ri(struct laasMessageType2 *msg);

void
setLmt2Ri(struct laasMessageType2 *msg, int val);

int
getLmt2Sh(struct laasMessageType2 *msg);

void
setLmt2Sh(struct laasMessageType2 *msg, int val);

unsigned int
getLmt2Ru(struct laasMessageType2 *msg);

void
setLmt2Ru(struct laasMessageType2 *msg, unsigned int val);

double
getLmt2Lat(struct laasMessageType2 *msg);

void
setLmt2Lat(struct laasMessageType2 *msg, double val);

double
getLmt2Lon(struct laasMessageType2 *msg);

void
setLmt2Lon(struct laasMessageType2 *msg, double val);

double
getLmt2Rph(struct laasMessageType2 *msg);

void
setLmt2Rph(struct laasMessageType2 *msg, double val);

unsigned int
getLmt2Rsds(struct laasMessageType2 *msg);

void
setLmt2Rsds(struct laasMessageType2 *msg, unsigned int val);

unsigned int
getLmt2Mud(struct laasMessageType2 *msg);

void
setLmt2Mud(struct laasMessageType2 *msg, unsigned int val);

double
getLmt2Kmepg(struct laasMessageType2 *msg);

void
setLmt2Kmepg(struct laasMessageType2 *msg, double val);

double
getLmt2Kmecg(struct laasMessageType2 *msg);

void
setLmt2Kmecg(struct laasMessageType2 *msg, double val);

double
getLmt2Kmepgl(struct laasMessageType2 *msg);

void
setLmt2Kmepgl(struct laasMessageType2 *msg, double val);

double
getLmt2Kmecgl(struct laasMessageType2 *msg);

void
setLmt2Kmecgl(struct laasMessageType2 *msg, double val);

int isValidLmt2Gsrr(struct laasMessageType2 *msg);
int isValidLmt2Gsad(struct laasMessageType2 *msg);
int isValidLmt2Gcid(struct laasMessageType2 *msg);
int isValidLmt2Lmv(struct laasMessageType2 *msg);
int isValidLmt2Svig(struct laasMessageType2 *msg);
int isValidLmt2Ri(struct laasMessageType2 *msg);
int isValidLmt2Sh(struct laasMessageType2 *msg);
int isValidLmt2Lat(struct laasMessageType2 *msg);
int isValidLmt2Lon(struct laasMessageType2 *msg);
int isValidLmt2Rph(struct laasMessageType2 *msg);
int isValidLmt2Rsds(struct laasMessageType2 *msg);
int isValidLmt2Mud(struct laasMessageType2 *msg);
int isValidLmt2Kmepg(struct laasMessageType2 *msg);
int isValidLmt2Kmecg(struct laasMessageType2 *msg);
int isValidLmt2Kmepgl(struct laasMessageType2 *msg);
int isValidLmt2Kmecgl(struct laasMessageType2 *msg);

#endif
