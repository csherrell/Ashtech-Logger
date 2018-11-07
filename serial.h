/*
 *  Serial.h Alpha Test Unit
 *	Created: 6.20.2005
 *	Author: Andy Archinal (andy@ou.edu)
 *
 */

#ifndef _SERIAL_H
#define _SERIAL_H

#define COM_1           "/dev/ttyS0"
#define COM_2           "/dev/ttyS1"
#define SEALEVEL_P1		"/dev/ttyS11"
#define SEALEVEL_P2		"/dev/ttyS12"
#define SEALEVEL_P3		"/dev/ttyS13"
#define SEALEVEL_P4		"/dev/ttyS14"
#define SEALEVEL_P5		"/dev/ttyS15"
#define SEALEVEL_P6		"/dev/ttyS16"
#define SEALEVEL_P7		"/dev/ttyS17"
#define SEALEVEL_P8		"/dev/ttyS18"

int init_serial_settings9600(int fid);
int serial_set_speed(int fid, int baud_rate);

#endif /* _SERIAL_H */
