#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <gdbm.h>
#include <stdlib.h>
#include "../laas.h"
#include "../find_string.h"
#include "../conversions.h"
#include "../time_struct.h"
#include "../time_conversions.h"
#include "../gdbm/gdbm_dictionary.h"
#include "../snv_update.h"
#include "gg12.h"

/*
#include "../gps.h"
*/

void gg12_print_statistics(struct str_gg12_device gg12[], struct str_limits *limits)
{
     int i = 0;
     for(i = 0; i < limits->NUMBER_OF_GG12S; i++)
     {
          fprintf(gg12[i].serial_device->console, "\nMessage Summary\n");
          fprintf(gg12[i].serial_device->console, "MCA Count:       %d\n", gg12[i].stats.mca_count);
          fprintf(gg12[i].serial_device->console, "MCAChecksum:     %d\n", gg12[i].stats.mca_checksum_count);
          fprintf(gg12[i].serial_device->console, "Total MCA:       %d\n", gg12[i].stats.mca_count +
                    gg12[i].stats.mca_checksum_count);
          fprintf(gg12[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12[i].serial_device->console, "MIS Count:       %d\n", gg12[i].stats.mis_count);
          fprintf(gg12[i].serial_device->console, "MISChecksum:     %d\n", gg12[i].stats.mis_checksum_count);
          fprintf(gg12[i].serial_device->console, "Total MIS:       %d\n", gg12[i].stats.mis_count +
                    gg12[i].stats.mis_checksum_count);
          fprintf(gg12[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12[i].serial_device->console, "PBN Count:       %d\n", gg12[i].stats.pbn_count);
          fprintf(gg12[i].serial_device->console, "PBNChecksum:     %d\n", gg12[i].stats.pbn_checksum_count);
          fprintf(gg12[i].serial_device->console, "Total PBN:       %d\n", gg12[i].stats.pbn_count +
                    gg12[i].stats.pbn_checksum_count);
          fprintf(gg12[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12[i].serial_device->console, "SAL Count:       %d\n", gg12[i].stats.sal_count);
          fprintf(gg12[i].serial_device->console, "SALChecksum:     %d\n", gg12[i].stats.sal_checksum_count);
          fprintf(gg12[i].serial_device->console, "Total SAL:       %d\n", gg12[i].stats.sal_count +
                    gg12[i].stats.sal_checksum_count);
          fprintf(gg12[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12[i].serial_device->console, "SNV Count:       %d\n", gg12[i].stats.snv_count);
          fprintf(gg12[i].serial_device->console, "SNVChecksum:     %d\n", gg12[i].stats.snv_checksum_count);
          fprintf(gg12[i].serial_device->console, "Total SNV:       %d\n", gg12[i].stats.snv_count +
                    gg12[i].stats.snv_checksum_count);
          fprintf(gg12[i].serial_device->console, "-----------------------------------\n");
          fprintf(gg12[i].serial_device->console, "XYZ Count:       %d\n", gg12[i].stats.xyz_count);
          fprintf(gg12[i].serial_device->console, "XYZChecksum:     %d\n", gg12[i].stats.xyz_checksum_count);
          fprintf(gg12[i].serial_device->console, "Total XYZ:       %d\n", gg12[i].stats.xyz_count +
                    gg12[i].stats.xyz_checksum_count);
          fprintf(gg12[i].serial_device->console, "-----------------------------------\n");

          fprintf(gg12[i].serial_device->console, "Other Count:     %d\n", gg12[i].stats.other_count);
          fprintf(gg12[i].serial_device->console, "-----------------------------------\n");


          fprintf(gg12[i].serial_device->console, "Total Checksums: %d\n",
                    gg12[i].stats.mca_checksum_count +
                    gg12[i].stats.mis_checksum_count +
                    gg12[i].stats.pbn_checksum_count +
                    gg12[i].stats.sal_checksum_count +
                    gg12[i].stats.snv_checksum_count +
                    gg12[i].stats.xyz_checksum_count);

          fprintf(gg12[i].serial_device->console, "Total Complete Messages:  %d\n",
                    gg12[i].stats.mca_count +
                    gg12[i].stats.mis_count +
                    gg12[i].stats.pbn_count +
                    gg12[i].stats.sal_count +
                    gg12[i].stats.snv_count +
                    gg12[i].stats.xyz_count);

          fprintf(gg12[i].serial_device->console, "(Messages + Checksums)\nTotal Messages Processed: %d\n",
                    gg12[i].stats.mca_count +
                    gg12[i].stats.mca_checksum_count +
                    gg12[i].stats.mis_count +
                    gg12[i].stats.mis_checksum_count +
                    gg12[i].stats.pbn_count +
                    gg12[i].stats.pbn_checksum_count +
                    gg12[i].stats.sal_count +
                    gg12[i].stats.sal_checksum_count +
                    gg12[i].stats.snv_count +
                    gg12[i].stats.snv_checksum_count +
                    gg12[i].stats.xyz_count +
                    gg12[i].stats.xyz_checksum_count);

          fprintf(gg12[i].serial_device->console, "(Messages + Checksums + Others Headers)\nTotal Headers Found:      %d\n",
                    gg12[i].stats.mca_count +
                    gg12[i].stats.mca_checksum_count +
                    gg12[i].stats.mis_count +
                    gg12[i].stats.mis_checksum_count +
                    gg12[i].stats.pbn_count +
                    gg12[i].stats.pbn_checksum_count +
                    gg12[i].stats.sal_count +
                    gg12[i].stats.sal_checksum_count +
                    gg12[i].stats.snv_count +
                    gg12[i].stats.snv_checksum_count +
                    gg12[i].stats.xyz_count +
                    gg12[i].stats.xyz_checksum_count +
                    gg12[i].stats.other_count);
     }
}

/*
 * returns type of message in queue
 */
int gg12_process_incoming(
          struct str_gg12_device *c,
          struct str_satellite_almanac_ephemeris_position satellites_position_information[],
          struct str_binary_search_array binary_search_ephemeris[],
          struct str_location *centroid_location,
          struct str_time *master_time,
          FILE *alm_csv,
          FILE *eph_csv,
          GDBM_FILE dbf)
{
     int length = 0;
     int offset = 0;
     int gg12_message_type = 0;
     /* static int last_offset = 0; */
     char tempString[12];
     unsigned char *header;

     unsigned int chnind = 0;

     int xyz_size = 0;

     static int count = 0;

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
               gg12_message_type = header[PASH_HEADER_SIZE]     << 16 | 
                    header[PASH_HEADER_SIZE + 1] <<  8 | 
                    header[PASH_HEADER_SIZE + 2];
               switch(gg12_message_type)
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
                                   c->time_mark = true;
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
                                   /* printf("Device: %s, PBN RCVTime:%d\n",c->serial_device->name, c->pbn.rcvtime); */
                                   c->serial_device->time_received.tm_gps.rcvtime = c->pbn.rcvtime;
                                   c->time_mark = true;
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
                                   memcpy(&c->sal.time_received, master_time, sizeof(struct str_time));
                                   /*ashtech_store_sal(&c->sal, dbf); */
                                   gdbm_sal_update_current(dbf, &c->sal);
                                   memcpy(&satellites_position_information[c->sal.prn].sal, &c->sal, sizeof(struct str_ashtech_sal));
                                   if(c->sal.logging_parsed)
                                   {
                                        ashtech_print_sal(&c->sal, c->sal.time_received.tm_gps.rcvtime);
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
                                   c->stats.snv_count++;
                                   ashtech_parse_snv(&header[PASH_HEADER_TOTAL], &c->snv);
                                   if(c->snv.prnnum < NUMBER_OF_GPS_SATELLITES)
                                   {
                                        memcpy(&c->snv.time_received, master_time, sizeof(struct str_time));
/* TEST_CASE */                         /* Not sure why I need to do this but I will to make chad davis */
                                        gps_add_time(&c->snv.time_received, 500);
/*
                                        ashtech_store_snv(&c->snv, dbf);
                                        memcpy(&satellites_position_information[c->snv.prnnum].snv, &c->snv, sizeof(struct str_ashtech_snv));
                                        satellites_position_information[c->snv.prnnum].snv.csv = NULL;
*/
                                        /* !!!!!! This is just for testing                           !!!!!!*/
                                        /* !!!!!! This if statement should be removed for production !!!!!!*/
                                        /* !!!!!! TEST_CASE                                          !!!!!!*/
                                        if(strcmp("LM",c->serial_device->type_semantic) == 0)
                                        {
                                             snv_validate(&satellites_position_information[c->snv.prnnum],
                                                       &c->snv,
                                                       &binary_search_ephemeris[c->snv.prnnum],
                                                       centroid_location,
                                                       master_time,
                                                       alm_csv,
                                                       eph_csv,
                                                       dbf);
                                        }
                                   }
                                   /* c->snv.time_received.tm_gps.rcvtime = c->serial_device->rcvtime; */
                                   /*  Controls the setting of week number */
                                   /*
                                   if(c->snv.wn > c->serial_device->week)
                                   {
                                        c->serial_device->week = c->snv.wn;
                                   }
                                   */
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
                              if(raw_to_ushort(&header[15]) > GG12_CHANNELS)
                              {
                                   /* Replace the header with null values so that it
                                      get skipped the next pass through */
                                   /* printf("XYZ %d > %d\n", raw_to_ushort(&header[15]), GG12_CHANNELS); */
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
     
/*
     printf("%d %s %s\n",c->serial_device->type_index, c->serial_device->type_semantic, c->serial_device->name);
     fflush(stdout);
*/
     fflush(c->serial_device->console);

     return 0;
}

