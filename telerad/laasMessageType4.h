#ifndef LAAS_MESSAGE_TYPE_4_H
#define LAAS_MESSAGE_TYPE_4_H 1
struct laasMessageType4
{
    /* unsigned int fas_count; */
    int fas_count;

/*****************************/
/******  Message Type 4 ******/
    struct str_fas_block
    {
        unsigned int  valid;

        unsigned int dsl;
        unsigned int ot;
        unsigned int ssp;
                 char aid[5]; /*  */
        unsigned int rn;
                 char rl;     /*  */
        unsigned int apd;
                 char ri;     /*  */
        unsigned int rpds;
                 char rpid[5]; /* */
        double        lat;
        double        lon;
        double        height;
        double        d_lat;
        double        d_lon;
        double        atch;
        unsigned int atus;
        double        gpa;
        double        cwat;
        unsigned int  dlo;
        unsigned int  fas_crc;
        double        fas_val;
        double        fas_lal;
    } fas[16];

/*****************************/

};

struct laasMessageType4 *
laasMessageType4_init();

struct laasMessageType4 *
laasMessageType4_delete(struct laasMessageType4 *mt4);

void
laasMessageType4_clear(struct laasMessageType4 *mt4);

void
laasMessageType4_process(struct laasMessageType4 *msgType4, unsigned char msgData[], int length);

void laasMessageType4_print(struct laasMessageType4 *msg, unsigned char msgData[]);
void laasMessageType4_print2(struct laasMessageType4 *msg, unsigned char msgData[]);
void laasMessageType4_print_csv(struct laasMessageType4 *msg, unsigned char msgData[]);

void laasMessageType4_raw_set(struct laasMessageType4 *msg, unsigned char *rawMessage);
void laasMessageType4_set(struct laasMessageType4 *laasMessageType4,
                          unsigned int dsl,
                          unsigned int ot,
                          unsigned int ssp,
                          unsigned int aid,
                          unsigned int rn,
                          unsigned int rl,
                          unsigned int apd,
                          unsigned int ri,
                          unsigned int rpds,
                          unsigned int rpid,
                          int lat,
                          int lon,
                          int height,
                          int d_lat,
                          int d_lon,
                          unsigned int atch,
                          unsigned int atus,
                          int gpa,
                          unsigned int cwat,
                          unsigned int dlo,
                          unsigned int fas_crc,
                          unsigned int fas_val,
                          unsigned int fas_lal);


void
setLmt4Validity(struct laasMessageType4 *msg,unsigned int index);

int
getLmt4AsByteArray(struct laasMessageType4 *msgType4, unsigned char msgData[]);

int
getLaasMessageType4FAS_Count(struct laasMessageType4 *msg);

void
setLaasMessageType4FAS_Count(struct laasMessageType4 *msg, int val);

unsigned int
getLaasMessageType4Dsl(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Dsl(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Dsl(struct laasMessageType4 *msg, unsigned int index);

unsigned int
getLaasMessageType4Ot(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Ot(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Ot(struct laasMessageType4 *msg, unsigned int index);

unsigned int
getLaasMessageType4Ssp(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Ssp(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Ssp(struct laasMessageType4 *msg, unsigned int index);

char *
getLaasMessageType4Aid(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Aid(struct laasMessageType4 *msg, unsigned int index, char val[]);

int
isValidLaasMessageType4Aid(struct laasMessageType4 *msg, unsigned int index);

unsigned int
getLaasMessageType4Rn(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Rn(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Rn(struct laasMessageType4 *msg, unsigned int index);

char
getLaasMessageType4Rl(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Rl(struct laasMessageType4 *msg, unsigned int index, char val);

int
isValidLaasMessageType4Rl(struct laasMessageType4 *msg, unsigned int index);

unsigned int
getLaasMessageType4Apd(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Apd(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Apd(struct laasMessageType4 *msg, unsigned int index);

char
getLaasMessageType4Ri(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Ri(struct laasMessageType4 *msg, unsigned int index, char val);

int
isValidLaasMessageType4Ri(struct laasMessageType4 *msg, unsigned int index);

unsigned int
getLaasMessageType4Rpds(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Rpds(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Rpds(struct laasMessageType4 *msg, unsigned int index);

char *
getLaasMessageType4Rpid(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Rpid(struct laasMessageType4 *msg, unsigned int index, char val[]);

int
isValidLaasMessageType4Rpid(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4Lat(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Lat(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4Lat(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4Lon(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Lon(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4Lon(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4Height(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Height(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4Height(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4D_lat(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4D_lat(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4D_lat(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4D_lon(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4D_lon(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4D_lon(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4Atch(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Atch(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4Atch(struct laasMessageType4 *msg, unsigned int index);

unsigned int
getLaasMessageType4Atus(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Atus(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Atus(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4Gpa(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Gpa(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4Gpa(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4Cwat(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Cwat(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4Cwat(struct laasMessageType4 *msg, unsigned int index);

unsigned int
getLaasMessageType4Dlo(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Dlo(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Dlo(struct laasMessageType4 *msg, unsigned int index);

unsigned int
getLaasMessageType4Fas_crc(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Fas_crc(struct laasMessageType4 *msg, unsigned int index, unsigned int val);

int
isValidLaasMessageType4Fas_crc(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4Fas_val(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Fas_val(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4Fas_val(struct laasMessageType4 *msg, unsigned int index);

double
getLaasMessageType4Fas_lal(struct laasMessageType4 *msg, unsigned int index);

void
setLaasMessageType4Fas_lal(struct laasMessageType4 *msg, unsigned int index, double val);

int
isValidLaasMessageType4Fas_lal(struct laasMessageType4 *msg, unsigned int index);
#endif
