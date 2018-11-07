#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include "key_value_parser.h"

#define KEY_SEP         "="
#define VAL_TERM        ';'

/* Returns -1 if error or number of key=value pairs read from fname */
/* Pass in the file to be parsed and the callback function */
/*
int parse_key_value_pairs(const char *fname, int (*KeyValueCallback)(const char *, const char *))
*/
int parse_key_value_pairs(char buf[], char key[], char value[])
{
    /* char *p; */
    char *key_end, *value_end, *token;
    int counter = 0;

/* Open the the file return -1 if there is an error */
/*
    int cnt = 0;
    if ((fp = fopen(fname, "rt")) == NULL)
    {
        perror(fname);
        return -1;
    }
*/

/* Read one line at a time till EOF */
/*
    while (fgets(buf, LINE_LEN, fp) != NULL)
    {
*/

        /* Does string contain '='  If not, skip it */
        /* The  continue  utility shall return to the top of the smallest
           enclosing for, while, or until loop, or to the top of
           the nth enclosing loop, if n is specified. */
        if (strchr(buf, *KEY_SEP) == NULL)
            return -1;

        token = strtok(buf, KEY_SEP);               /* Extract key */

        strcpy(key,token);               /* Extract key */

        if (key != NULL)
        {
            /* Remove leading whitespace from key */
            /* While there is a space and key is not null */
            while (isspace(key[counter]) && (bool) key[counter])
                counter++;
              
            memmove(key,&key[counter],strlen(&key[counter]));

            if (key[0] == '#')
                return -1;

            /* Remove trailing whitespace from key */
            key_end = key;

            /* Stop on return '\n', space, or null */
            while ((*key_end != '\n') && (*key_end != ' ') && (bool) *key_end)
                key_end++;

            *key_end = '\0';

            counter = 0;
            strcpy(value,strtok(NULL, KEY_SEP));          /* Extract value */

            /* Remove leading whitespace from key */
            while (isspace(value[counter]) && (bool) value[counter])
                counter++;

            memmove(value,&value[counter],strlen(&value[counter]));

            value_end = value;

            /* Remove trailing whitespace from key */
            while ((*value_end != '\n') && (*value_end != ' ') && (bool) *value_end)
                value_end++;

            *value_end = '\0';

/* Remove trailing semicolon */
/*
            if ((p = strchr(val, VAL_TERM)) != NULL)
                *p = '\0';
*/
/*
            cnt++;
            (*KeyValueCallback)(key, val);
            printf("PARSE: KEY: %s, VALUE:%s\n", key, value);
*/
        }
/*
    }
    fclose(fp);
*/
    return 1;
}
