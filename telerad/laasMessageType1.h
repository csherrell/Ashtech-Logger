#ifndef LAAS_MESSAGE_TYPE_1_H
#define LAAS_MESSAGE_TYPE_1_H 1
/***************************************************************************/
struct laasMessageType1
{
    unsigned int valid;     /* do the range of values fall in the valid range for the field */
    /* Valid values defined in RTCA DO-246B Section 2.4.3.1 */
    /* Note 1: 1000 0000 indicated the measurment is not available                        */
    /* Note 2: For SBAS satellites and GBRs, the parameter is setRawLmt1 to all 0's              */
    /* Note 3: 1111 1111 indicates the source in invalid                                  */
    /* Note 4: 1111 1111 indicates that the value is not computed and should not be used. */
    /* Note 5: Parameter is associated with the first transmitted measurment block        */

    double mzc;         /* modifiedZCount             */
    unsigned int amf;  /* additionalMessageFlag      */
    /* unsigned int nom; */ /* numberOfMeasurements       */
    int nom;  /* numberOfMeasurements       */
    unsigned int mt;   /* measurementType            */
    double ed;          /* ephemerisDecorrelation     */
    unsigned int ecrc;  /* ephemerisCRC               */
    unsigned int sad;   /* sourceAvailabilityDuration */

    struct str_mesaurement_block
    {
        unsigned int  valid;

        unsigned int rs_id;  /* Ranging Source Id      */
        unsigned int iod;    /* Issue of Data          */
        double prc;              /* Pseudorange Correction */
        double rrc;              /* Range Rate Correction  */
        double spg;    /* Sigma pr_gnd           */
        double B1;
        double B2;
        double B3;
        double B4;
    } mb[12];
};

    double        getLmt1ModifiedZCount(struct laasMessageType1 *msg);
    unsigned int getLmt1AdditionalMessageFlag(struct laasMessageType1 *msg);
    unsigned int getLmt1NumberOfMeasurements(struct laasMessageType1 *msg);
    unsigned int getLmt1MeasurementType(struct laasMessageType1 *msg);
    double        getLmt1EphemerisDecorrelation(struct laasMessageType1 *msg);
    unsigned int  getLmt1EphemerisCRC(struct laasMessageType1 *msg);
    unsigned int  getLmt1SourceAvailabilityDuration(struct laasMessageType1 *msg);
    unsigned int getLmt1RangingSourceID(struct laasMessageType1 *msg, int index);
    unsigned int getLmt1IssueOfData(struct laasMessageType1 *msg, int index);
    double        getLmt1PseudorangeCorrection(struct laasMessageType1 *msg, int index);
    double        getLmt1RangeRateCorrection(struct laasMessageType1 *msg, int index);
    double        getLmt1SigmaPrGnd(struct laasMessageType1 *msg, int index);
    double        getLmt1B1(struct laasMessageType1 *msg, int index);
    double        getLmt1B2(struct laasMessageType1 *msg, int index);
    double        getLmt1B3(struct laasMessageType1 *msg, int index);
    double        getLmt1B4(struct laasMessageType1 *msg, int index);

    void setLmt1ModifiedZCount(struct laasMessageType1 *msg, double param);
    void setLmt1AdditionalMessageFlag(struct laasMessageType1 *msg, unsigned int param);
    void setLmt1NumberOfMeasurements(struct laasMessageType1 *msg, unsigned int param);
    void setLmt1MeasurementType(struct laasMessageType1 *msg, unsigned int param);
    void setLmt1EphemerisDecorrelation(struct laasMessageType1 *msg, double param);
    void setLmt1EphemerisCRC(struct laasMessageType1 *msg, unsigned int param);
    void setLmt1SourceAvailabilityDuration(struct laasMessageType1 *msg, unsigned int param);
    void setLmt1RangingSourceID(struct laasMessageType1 *msg, int index, unsigned int param);
    void setLmt1IssueOfData(struct laasMessageType1 *msg, int index, unsigned int param);
    void setLmt1PseudorangeCorrection(struct laasMessageType1 *msg, int index, double param);
    void setLmt1RangeRateCorrection(struct laasMessageType1 *msg, int index, double param);
    void setLmt1SigmaPrGnd(struct laasMessageType1 *msg, int index, double param);
    void setLmt1B1(struct laasMessageType1 *msg, int index, double param);
    void setLmt1B2(struct laasMessageType1 *msg, int index, double param);
    void setLmt1B3(struct laasMessageType1 *msg, int index, double param);
    void setLmt1B4(struct laasMessageType1 *msg, int index, double param);

    void setLmt1Validity(struct laasMessageType1 *msg);

    int getLmt1AsByteArray(struct laasMessageType1 *mt1, unsigned char msgData[]);

    struct laasMessageType1 *
            laasMessageType1_init();
    struct laasMessageType1 *
            laasMessageType1_delete(struct laasMessageType1 *msg);
    void laasMessageType1_clear(struct laasMessageType1 *msg);

    void laasMessageType1_process(struct laasMessageType1 *msg, unsigned char msgData[]);
    void laasMessageType1_print(struct laasMessageType1 *msg, unsigned char msgData[]);
    void laasMessageType1_print_csv(struct laasMessageType1 *msg, FILE *csv);

    int isValidLmt1ModifiedZCount(struct laasMessageType1 *msgType1);
    int isValidLmt1AdditionalMessageFlag(struct laasMessageType1 *msgType1);
    int isValidLmt1NumberOfMeasurements(struct laasMessageType1 *msgType1);
    int isValidLmt1MeasurementType(struct laasMessageType1 *msgType1);
    int isValidLmt1EphemerisDecorrelation(struct laasMessageType1 *msgType1);
    int isValidLmt1SourceAvailabilityDuration(struct laasMessageType1 *msgType1);
    int isValidLmt1RangingSourceID(struct laasMessageType1 *msg, int index);
    int isValidLmt1PseudorangeCorrection(struct laasMessageType1 *msg, int index);
    int isValidLmt1RangeRateCorrection(struct laasMessageType1 *msg, int index);
    int isValidLmt1SigmaPrGnd(struct laasMessageType1 *msg, int index);
    int isValidLmt1B1(struct laasMessageType1 *msg, int index);
    int isValidLmt1B2(struct laasMessageType1 *msg, int index);
    int isValidLmt1B3(struct laasMessageType1 *msg, int index);
    int isValidLmt1B4(struct laasMessageType1 *msg, int index);

/***************************************************************************/
#endif

