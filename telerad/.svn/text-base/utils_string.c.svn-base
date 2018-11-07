#include "utils_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
print_buffer(unsigned char buf[],int size)
{
    int i = 0;
    for(i=0;i < size;i++)
    {
        printf("%02X",buf[i]);
    }
}

void
print_buffer_in_binary(unsigned char data[], int size)
{
    int mask = 0x80;
    int i = 0;
    int j = 0;

    for(i=0;i<size;i++)
    {
        mask = 0x80;
        for(j=0;j<8;j++)
        {
            if((mask & data[i]) != 0)
                printf("1");
            else
                printf("0");

            mask = mask >> 1;
        }
        printf(" ");
    }
}

void utils_string_swap_bytes(char a, char b)
{
    a = a ^ b;
    b = b ^ a;
    a = a ^ b;
}

/*
 * swap bytes in a char[]
*/
void utils_string_swap(unsigned char *x, unsigned char *y)
{
    unsigned char tmp = 0;
    tmp = *x;
    *x = *y;
    *y = tmp;
}
                                                                                
/*
 * convert char[] to a float
 * GG12's IEEE float come in big endian so they're swapped
 */
float utils_string_raw_to_float(unsigned char in[])
{
    float t = 0;
                                                                                
    utils_string_swap(&in[0], &in[3]);
    utils_string_swap(&in[1], &in[2]);
                                                                                
    memcpy(&t, in, 4);
                                                                                
    return t;
}

/*
 * convert char[] to a double
 * GG12's IEEE doubles come in big endian so they're swapped
 */
double utils_string_raw_to_double(unsigned char in[])
{
    double t;

    utils_string_swap(&in[0], &in[7]);
    utils_string_swap(&in[1], &in[6]);
    utils_string_swap(&in[2], &in[5]);
    utils_string_swap(&in[3], &in[4]);
                                                                                
    memcpy(&t, in, 8);
    
    return t;
}
                                                                                
/*
 * convert char[] to a short
 */
short utils_string_raw_to_short(unsigned char in[])
{
    return in[0] << 8 | in[1];
}
                                                                                
/*
 * convert char[] to a unsigned short
 */
unsigned short utils_string_raw_to_ushort(unsigned char in[])
{
    return in[0] << 8 | in[1];
}
                                                                                
/*
 * convert char[] to int
 */
int utils_string_raw_to_int(unsigned char in[])
{
    return in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
}
                                                                                
/*
 * convert char[] to unsigned int
 */
unsigned int utils_string_raw_to_uint(unsigned char in[])
{
    return in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
}

char**
utils_string_parse(char *string, char c, int *i)
{

    char **data = 0;
    char *start = 0;              /* points to start of record */
    char *end = 0;                /* point to end of each field */

    /* Get a count of tokens */
    start = string;
    while ((end = strchr(start, c))) {      /* while char c found */
        *end = '\0';                      /* chop of field pointed to by end */
        (*i) = (*i) + 1;
        *end = c;                         /* return c to field pointed to by end */
        start = end+1;                    /* point to rest of record */
    }

    /* Allocate the memory for the token array */
    data = (char **) calloc((*i),sizeof(char *));
    start = string;
    (*i) = 0;

    /* Allocate the memory for each token and copy the token */
    while ((end = strchr(start, c))) {      /* while char c found */
        *end = '\0';                      /* chop of field pointed to by end */
        data[(*i)] = (char *) malloc(strlen(start));
        memcpy(data[(*i)],start,strlen(start));
        (*i) = (*i) + 1;
        *end = c;                         /* return c to field pointed to by end */
        start = end+1;                    /* point to rest of record */
    }
        data[(*i)] = (char *) malloc(strlen(start));
        memcpy(data[(*i)],start,strlen(start));

    return data;
}


unsigned char
ascii_hex_to_bin_hex(char c)
{
    switch(c)
    {
        case '0': return 0x0;
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0x4;
        case '5': return 0x5;
        case '6': return 0x6;
        case '7': return 0x7;
        case '8': return 0x8;
        case '9': return 0x9;
        case 'A': case 'a': return 0xA;
        case 'B': case 'b': return 0xB;
        case 'C': case 'c': return 0xC;
        case 'D': case 'd': return 0xD;
        case 'E': case 'e': return 0xE;
        case 'F': case 'f': return 0xF;
        default:  return 0x0;
    }
}

void 
ascii_hex_string_to_binary(char ascii[], unsigned char binary[], int size)
{
     int i = 0;
     int byteCount = 0;
     int shift = 1;
     for(i = 0; i < size; i++)
     {
          binary[byteCount] |= ascii_hex_to_bin_hex(ascii[i]);
          if(shift)
          {
               binary[byteCount] = binary[byteCount] << 4;
               shift = 0;
          }  else {
               shift = 1;
               byteCount++;
          }
     }
}

