#ifndef CRC_32_BIT_H
#define CRC_32_BIT_H 1

unsigned long calculate_crc_32bit(unsigned char *ch_ptr, int length);
unsigned long generate_crc_32bit(unsigned char *ch_ptr, int length);

#endif
