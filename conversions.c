/*
 *  conversions.c
 *		Description: Utilities to convert data types basic operations
					necessary for defined data types.
 *		Created: 6.22.2005
 *		Author:  Andy Archinal (andy@ou.edu)
 *
 *
 */

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <math.h>
#include "conversions.h"



/*

struct ecef llh_to_ecef(struct llh *pos)
{
  double n;
  struct ecef result;
  n=a*a/sqrt(a*a*cos(pos->lat)*cos(pos->lat)+b*b*sin(pos->lat)*sin(pos->lat));
  result.x=(n+pos->hae)*cos(pos->lat)*cos(pos->lon);
  result.y=(n+pos->hae)*cos(pos->lat)*sin(pos->lon);
  result.z=(b*b/(a*a)*n+pos->hae)*sin(pos->lat);
  return(result);
}
*/
/*
 * swap bytes in a char[]
 */
void swap(unsigned char *x, unsigned char *y)
{
	unsigned char tmp;
	tmp = *x;
	*x = *y;
	*y = tmp;
}

/*
 * convert char[] to a float
 * GG12's IEEE float come in big endian so they're swapped
 */
float raw_to_float(unsigned char *in)
{
	float t;
     unsigned char tempString[4];

     memcpy(tempString,in,4);

	swap(&tempString[0], &tempString[3]);
	swap(&tempString[1], &tempString[2]);

	memcpy(&t, tempString, 4);

	return t;
}

/*
 * convert char[] to a double
 * GG12's IEEE doubles come in big endian so they're swapped
 */
double raw_to_double(unsigned char *in)
{
     double t;
     unsigned char tempString[8];

     memcpy(tempString,in,8);

     swap(&tempString[0], &tempString[7]);
     swap(&tempString[1], &tempString[6]);
     swap(&tempString[2], &tempString[5]);
     swap(&tempString[3], &tempString[4]);

     memcpy(&t, tempString, 8);

     return t;
}

/*
 * convert char[] to a short
 */
short raw_to_short(unsigned char *in)
{
	return in[0] << 8 | in[1];
}

/*
 * convert char[] to a unsigned short
 */
ushort raw_to_ushort(unsigned char *in)
{
	return in[0] << 8 | in[1];
}

/*
 * convert char[] to int
 */
int raw_to_int(unsigned char *in)
{
	return in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
}

/*
 * convert char[] to unsigned int
 */
uint raw_to_uint(unsigned char *in)
{
	return in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
}
