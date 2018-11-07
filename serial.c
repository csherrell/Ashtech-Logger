/*
 *  Serial.c 
 *		Description: POSIX Serial Communication Utilities
 *		Created: 6.20.2005
 *		Author:  Andy Archinal (andy@ou.edu)
 *
 *
 */

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

/*
 *  this sets serial port using POSIX termios struct
 */
int init_serial_settings9600(int fid)
{
	struct termios options;

	tcgetattr(fid, &options);

	memset(&options, 0x0, sizeof(options));

	cfsetispeed(&options, B9600);            /* set input speed */
	cfsetospeed(&options, B9600);            /* set output speed */
/*	cfsetispeed(&options, B115200); */       /* set input speed */
/*	cfsetospeed(&options, B115200); */       /* set output speed */

	options.c_cflag &= ~PARENB;              /* parity checking */
	options.c_cflag |= (CLOCAL | CREAD);     /* enable receiver */
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* raw mode */

	options.c_cflag &= ~CSTOPB;              /* set stop bit */
	options.c_cflag &= ~CSIZE;               /* clear size bit */
	options.c_cflag |= CS8;                  /* set to 8-bit */
/*	options.c_cflag &= ~CRTSCTS;    */       /* no hardware handshaking */ 

	options.c_oflag &= ~OPOST;               /* no post processing */
	options.c_iflag &= ~(IXON | IXOFF | IXANY);    /* no software flow */

	return tcsetattr(fid, TCSANOW, &options);
}

int serial_set_speed(int fid, int baud_rate)
{
     struct termios options;

     tcgetattr(fid, &options);

     switch(baud_rate)
     {
          case 9600:
               cfsetispeed(&options, B9600);            /* set input speed */
               cfsetospeed(&options, B9600);            /* set output speed */
          case 19200:
               cfsetispeed(&options, B19200);            /* set input speed */
               cfsetospeed(&options, B19200);            /* set output speed */
          case 38400:
               cfsetispeed(&options, B38400);            /* set input speed */
               cfsetospeed(&options, B38400);            /* set output speed */
          case 57600:
               cfsetispeed(&options, B57600);            /* set input speed */
               cfsetospeed(&options, B57600);            /* set output speed */
               break;
          case 115200:
               cfsetispeed(&options, B115200);            /* set input speed */
               cfsetospeed(&options, B115200);            /* set output speed */
               break;
          default:
               printf("Uknown Baud Rate\n");
               break;
     }

/* Do not flush the buffer when when using the spliters
   it breaks Chad Davis' Stuff
	tcflush(fid, TCIOFLUSH);
*/

	return tcsetattr(fid, TCSANOW, &options);
}
