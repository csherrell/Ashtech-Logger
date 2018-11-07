#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <gdbm.h>
#include <stdlib.h>
#include "../laas.h"
#include "../find_string.h"
#include "../conversions.h"
#include "gg12w.h"

void gg12w_print_statistics(struct str_gg12w_device gg12w[], struct str_limits *limits)
{
     int i = 0;
     for(i = 0; i < limits->NUMBER_OF_GG12S; i++)
     {
          fprintf(gg12w[i].serial_device->console, "\nMessage Summary\n");
          fprintf(gg12w[i].serial_device->console, "MCA Count:       %d\n", gg12w[i].stats.mca_count);
          fprintf(gg12w[i].serial_device->console, "MCAChecksum:     %d\n", gg12w[i].stats.mca_checksum_count);
          fprintf(gg12w[i].serial_device->console, "Total MCA:       %d\n", gg12w[i].stats.mca_count +
                    gg12w[i].stats.mca_checksum_count);
          fprintf(gg12w[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12w[i].serial_device->console, "MIS Count:       %d\n", gg12w[i].stats.mis_count);
          fprintf(gg12w[i].serial_device->console, "MISChecksum:     %d\n", gg12w[i].stats.mis_checksum_count);
          fprintf(gg12w[i].serial_device->console, "Total MIS:       %d\n", gg12w[i].stats.mis_count +
                    gg12w[i].stats.mis_checksum_count);
          fprintf(gg12w[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12w[i].serial_device->console, "PBN Count:       %d\n", gg12w[i].stats.pbn_count);
          fprintf(gg12w[i].serial_device->console, "PBNChecksum:     %d\n", gg12w[i].stats.pbn_checksum_count);
          fprintf(gg12w[i].serial_device->console, "Total PBN:       %d\n", gg12w[i].stats.pbn_count +
                    gg12w[i].stats.pbn_checksum_count);
          fprintf(gg12w[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12w[i].serial_device->console, "SAL Count:       %d\n", gg12w[i].stats.sal_count);
          fprintf(gg12w[i].serial_device->console, "SALChecksum:     %d\n", gg12w[i].stats.sal_checksum_count);
          fprintf(gg12w[i].serial_device->console, "Total SAL:       %d\n", gg12w[i].stats.sal_count +
                    gg12w[i].stats.sal_checksum_count);
          fprintf(gg12w[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12w[i].serial_device->console, "SNV Count:       %d\n", gg12w[i].stats.snv_count);
          fprintf(gg12w[i].serial_device->console, "SNVChecksum:     %d\n", gg12w[i].stats.snv_checksum_count);
          fprintf(gg12w[i].serial_device->console, "Total SNV:       %d\n", gg12w[i].stats.snv_count +
                    gg12w[i].stats.snv_checksum_count);
          fprintf(gg12w[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12w[i].serial_device->console, "XYZ Count:       %d\n", gg12w[i].stats.xyz_count);
          fprintf(gg12w[i].serial_device->console, "XYZChecksum:     %d\n", gg12w[i].stats.xyz_checksum_count);
          fprintf(gg12w[i].serial_device->console, "Total XYZ:       %d\n", gg12w[i].stats.xyz_count +
                    gg12w[i].stats.xyz_checksum_count);
          fprintf(gg12w[i].serial_device->console, "-----------------------------------\n");

          fprintf(gg12w[i].serial_device->console, "Other Count:     %d\n", gg12w[i].stats.other_count);
          fprintf(gg12w[i].serial_device->console, "-----------------------------------\n");


          fprintf(gg12w[i].serial_device->console, "Total Checksums: %d\n",
                    gg12w[i].stats.mca_checksum_count +
                    gg12w[i].stats.mis_checksum_count +
                    gg12w[i].stats.pbn_checksum_count +
                    gg12w[i].stats.sal_checksum_count +
                    gg12w[i].stats.snv_checksum_count +
                    gg12w[i].stats.xyz_checksum_count);

          fprintf(gg12w[i].serial_device->console, "Total Complete Messages:  %d\n",
                    gg12w[i].stats.mca_count +
                    gg12w[i].stats.mis_count +
                    gg12w[i].stats.pbn_count +
                    gg12w[i].stats.sal_count +
                    gg12w[i].stats.snv_count +
                    gg12w[i].stats.xyz_count);

          fprintf(gg12w[i].serial_device->console, "(Messages + Checksums)\nTotal Messages Processed: %d\n",
                    gg12w[i].stats.mca_count +
                    gg12w[i].stats.mca_checksum_count +
                    gg12w[i].stats.mis_count +
                    gg12w[i].stats.mis_checksum_count +
                    gg12w[i].stats.pbn_count +
                    gg12w[i].stats.pbn_checksum_count +
                    gg12w[i].stats.sal_count +
                    gg12w[i].stats.sal_checksum_count +
                    gg12w[i].stats.snv_count +
                    gg12w[i].stats.snv_checksum_count +
                    gg12w[i].stats.xyz_count +
                    gg12w[i].stats.xyz_checksum_count);

          fprintf(gg12w[i].serial_device->console, "(Messages + Checksums + Others Headers)\nTotal Headers Found:      %d\n",
                    gg12w[i].stats.mca_count +
                    gg12w[i].stats.mca_checksum_count +
                    gg12w[i].stats.mis_count +
                    gg12w[i].stats.mis_checksum_count +
                    gg12w[i].stats.pbn_count +
                    gg12w[i].stats.pbn_checksum_count +
                    gg12w[i].stats.sal_count +
                    gg12w[i].stats.sal_checksum_count +
                    gg12w[i].stats.snv_count +
                    gg12w[i].stats.snv_checksum_count +
                    gg12w[i].stats.xyz_count +
                    gg12w[i].stats.xyz_checksum_count +
                    gg12w[i].stats.other_count);
     }
}

/*
 * returns type of message in queue
 */
int gg12w_process_incoming(struct str_gg12w_device *c, GDBM_FILE dbf)
{
     int length = 0;
     int offset = 0;
     int gg12w_message_type = 0;
     /* static int last_offset = 0; */
     char tempString[12];
     unsigned char *header;

     unsigned int chnind = 0;

     int xyz_size = 0;

     static int count = 0;

     /*
        unsigned char *test_header;
        */

     ++count;

     memset(tempString,0x00, 12);

     header = (unsigned char *) find_string((void *) &c->serial_device->buffer[c->serial_device->search_start],
               c->serial_device->buffer_end - c->serial_device->search_start,
               PASH_HEADER,PASH_HEADER_SIZE);

     /*
        printf("IN Process INCOMMING\n");
        printf("0:%d, %d, BUFEND:%d\n",c->serial_device->device_num, count, c->serial_device->buffer_end);
        print_buffer(c->serial_device->buffer,c->serial_device->buffer_end);
        printf("\n");
        */

     /*
        c->serial_device->stats.Count++;
        printf("ID: %s, BUFFER START: %d, BUFFER END: %d\n", c->serial_device->name, c->serial_device->search_start, c->serial_device->buffer_end);
        */
     if (header != NULL)
     {
          offset = header - c->serial_device->buffer;
          length = c->serial_device->buffer_end - offset;
          if(length > MAX_BUFFER_SIZE)
          {
               printf("length > MAX_BUFFER_SIZE: %d\n", length);
          }
          if(length < PASH_HEADER_TOTAL)
          {
               shift_buffer(c->serial_device, header);
               c->serial_device->search_start = 0;
               /* last_offset = offset; */
               return 1;
          } else {
               gg12w_message_type = header[PASH_HEADER_SIZE]     << 16 | 
                    header[PASH_HEADER_SIZE + 1] <<  8 | 
                    header[PASH_HEADER_SIZE + 2];
               switch(gg12w_message_type)
               {
                    case MCA_IDENTIFIER:
                         if (length >= MCA_PACKET_SIZE)
                         {
                              c->serial_device->search_start = offset + PASH_HEADER_SIZE;
                              if (ashtech_checksum_8(&header[PASH_HEADER_TOTAL], MCA_PACKET_SIZE-PASH_HEADER_TOTAL, c->serial_device->console) >= 0)
                              {
                                   /* printf("Processing MCA: %d\n", c->serial_device->type_index); */
                                   c->stats.mca_count++;
                                   chnind = (unsigned int) header[PASH_HEADER_TOTAL+6] - 1; 
                                   ashtech_parse_mca(&header[PASH_HEADER_TOTAL], &c->mca, chnind); 
                                   if(c->mca.logging_parsed)
                                   {
                                        ashtech_print_mca(&c->mca,
                                                  chnind, 
                                                  c->serial_device->time_received.tm_gps.rcvtime);
                                        if(c->mca.channel[chnind].left == 0)
                                        {
                                             c->time_mark = true;
                                        }
                                   }
                              } else {
                                   printf("MCA BAD\n");
                                   c->stats.mca_checksum_count++;
                              }
                         } else {
                              shift_buffer(c->serial_device, header);
                              c->serial_device->search_start = 0;
                              return 1;
                         }
                         break;
                    case MIS_IDENTIFIER:
                         if (length >= MIS_PACKET_SIZE)
                         {
                              c->serial_device->search_start = offset + PASH_HEADER_SIZE;
                              if (ashtech_checksum_16(&header[PASH_HEADER_TOTAL], MIS_PACKET_SIZE-PASH_HEADER_TOTAL, c->serial_device->console) >= 0)
                              {
                                   /* printf("Processing MIS: %d\n", c->serial_device->type_index); */
                                   c->stats.mis_count++;
                                   ashtech_parse_mis(&header[PASH_HEADER_TOTAL], &c->mis);
                                   c->serial_device->time_received.tm_gps.rcvtime = c->mis.rcvtime;
                                   if(c->mis.logging_parsed)
                                   {
                                        ashtech_print_mis(&c->mis);
                                   }
                              } else {
                                   printf("MIS BAD\n");
                                   c->stats.mis_checksum_count++;
                              }
                         } else {
                              shift_buffer(c->serial_device, header);
                              c->serial_device->search_start = 0;
                              return 1;
                         }
                         break;
                    case PBN_IDENTIFIER:
                         if (length >= PBN_PACKET_SIZE)
                         {
                              c->serial_device->search_start = offset + PASH_HEADER_SIZE;
                              if (ashtech_checksum_16(&header[PASH_HEADER_TOTAL], PBN_PACKET_SIZE-PASH_HEADER_TOTAL, c->serial_device->console) >= 0)
                              {
                                   /* printf("Processing PBN: %d\n", c->serial_device->type_index); */
                                   c->stats.pbn_count++;
                                   ashtech_parse_pbn(&header[PASH_HEADER_TOTAL], &c->pbn);
                                   c->serial_device->time_received.tm_gps.rcvtime = c->pbn.rcvtime;
                                   if(c->pbn.logging_parsed)
                                   {
                                        ashtech_print_pbn(&c->pbn);
                                   }
                              } else {
                                   printf("PBN BAD\n");
                                   c->stats.pbn_checksum_count++;
                              }
                         } else {
                              shift_buffer(c->serial_device, header);
                              c->serial_device->search_start = 0;
                              return 1;
                         }
                         break;
                    case SAL_IDENTIFIER:
                         if (length >= SAL_PACKET_SIZE)
                         {
                              c->serial_device->search_start = offset + PASH_HEADER_SIZE;
                              if (ashtech_checksum_16(&header[PASH_HEADER_TOTAL], SAL_PACKET_SIZE-PASH_HEADER_TOTAL, c->serial_device->console) >= 0)
                              {
                                   /* printf("Processing SAL: %d\n", c->serial_device->type_index); */
                                   c->stats.sal_count++;
                                   ashtech_parse_sal(&header[PASH_HEADER_TOTAL], &c->sal);
                                   if(c->sal.logging_parsed)
                                   {
                                        ashtech_print_sal(&c->sal, c->serial_device->time_received.tm_gps.rcvtime);
                                   }
                              } else {
                                   printf("SAL BAD\n");
                                   c->stats.sal_checksum_count++;
                              }
                         } else {
                              shift_buffer(c->serial_device, header);
                              c->serial_device->search_start = 0;
                              return 1;
                         }
                         break;
                    case SNV_IDENTIFIER:
                         if (length >= SNV_PACKET_SIZE)
                         {
                              c->serial_device->search_start = offset + PASH_HEADER_SIZE;
                              if (ashtech_checksum_16(&header[PASH_HEADER_TOTAL], SNV_PACKET_SIZE-PASH_HEADER_TOTAL, c->serial_device->console) >= 0)
                              {
                                   /* printf("Processing SNV: %d\n", c->serial_device->type_index); */
                                   c->stats.snv_count++;
                                   ashtech_parse_snv(&header[PASH_HEADER_TOTAL], &c->snv);
                                   /* ashtech_store_snv(&c->snv, dbf); */
                                   c->snv.time_received.tm_gps.rcvtime = c->serial_device->time_received.tm_gps.rcvtime;
                                   /*  Controls the setting of week number */
                                   if(c->snv.wn > c->serial_device->time_received.tm_gps.week)
                                   {
                                        c->serial_device->time_received.tm_gps.week = c->snv.wn;
                                   }
                                   if(c->snv.logging_parsed)
                                   {
                                        ashtech_print_snv(&c->snv);
                                   }
                              } else {
                                   printf("SNV BAD\n");
                                   c->stats.snv_checksum_count++;
                              }
                         } else {
                              shift_buffer(c->serial_device, header);
                              c->serial_device->search_start = 0;
                              return 1;
                         }
                         break;
                    case XYZ_IDENTIFIER:
                         xyz_size = ashtech_xyz_size(&header[PASH_HEADER_TOTAL]);
                         if (length >= xyz_size + PASH_HEADER_TOTAL)
                         {
                              /* Check if the XYZ Message says there is more than 12 satellites */
                              if(raw_to_ushort(&header[15]) > GG12W_CHANNELS)
                              {
                                   /* Replace the header with null values so that it
                                      get skipped the next pass through */
                                   /* printf("XYZ %d > %d\n", raw_to_ushort(&header[15]), GG12W_CHANNELS); */
                                   memset(header, 0x0, 11);
                                   return 1;
                              }

                              c->serial_device->search_start = offset + PASH_HEADER_SIZE;
                              /*
                                 header[0] = 0x0;
                                 test_header = (unsigned char *) find_string((void *) &c->serial_device->buffer[c->serial_device->search_start],
                                 c->serial_device->buffer_end - c->serial_device->search_start,
                                 PASH_HEADER,PASH_HEADER_SIZE);

                                 printf("XYZ Size %d: Offset:%d Length:%d %d:%d\n", raw_to_ushort(&header[15]), 
                                 offset, length,
                                 xyz_size + 13, test_header - header);
                                 */
                              if(ashtech_checksum_16(&header[PASH_HEADER_TOTAL], xyz_size, c->serial_device->console) >= 0)
                              {
                                   /* printf("Processing XYZ: %d\n", c->serial_device->type_index); */
                                   c->stats.xyz_count++;
                                   ashtech_parse_xyz(&header[PASH_HEADER_TOTAL], &c->xyz);
                                   if(c->xyz.logging_parsed)
                                   {
                                        ashtech_print_xyz(&c->xyz);
                                   }
                              } else {
                                   printf("XYZ BAD\n"), 
                                        c->stats.xyz_checksum_count++;
                              }
                         } else {
                              shift_buffer(c->serial_device, header);
                              c->serial_device->search_start = 0;
                              return 1;
                         }
                         break;
                    default:
                         c->serial_device->search_start = offset + PASH_HEADER_SIZE;
                         strncpy(tempString,(char *)header,11);
                         /* printf("UNKNOWN TYPE->%s<-\n",tempString); */
                         break;
               }
          }
     } else {
          memmove(c->serial_device->buffer,&c->serial_device->buffer[c->serial_device->buffer_end - PASH_HEADER_SIZE + 1], PASH_HEADER_SIZE - 1);
          c->serial_device->buffer_end = PASH_HEADER_SIZE - 1;
          memset(&c->serial_device->buffer[c->serial_device->buffer_end], 0x00, MAX_BUFFER_SIZE - c->serial_device->buffer_end);
          c->serial_device->search_start = 0;
          return 1;
     }
     fflush(c->serial_device->console);
     return 0;
}

