/*
 *  conversions.h
 *		Description: Utilities to convert data types basic operations
					necessary for defined data types.
 *		Created: 6.22.2005
 *		Author:  Andy Archinal (andy@ou.edu)
 *
 *
 */
#define uchar      unsigned char
#define ushort     unsigned short
#define uint       unsigned int

void    swap(unsigned char *x, unsigned char *y);
float   raw_to_float(unsigned char *in);
double  raw_to_double(unsigned char *in);
short   raw_to_short(unsigned char *in);
ushort  raw_to_ushort(unsigned char *in);
int     raw_to_int(unsigned char *in);
uint    raw_to_uint(unsigned char *in);
