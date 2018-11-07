#include <string.h>
#include "shift_buffer.h"

void shift_buffer(struct str_serial_device *c, unsigned char *header)
{
     int offset = header - c->buffer;
     int length = c->buffer_end - offset;
     memset(c->buffer, 0x00, offset);
     memmove(c->buffer, header, length);
     c->buffer_end = length;
     memset(&c->buffer[c->buffer_end], 0x00, offset);
}

