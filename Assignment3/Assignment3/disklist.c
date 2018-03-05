/* Boyang Jiao
 * V00800928
 * Nov 28, 2016
 * CSC360
 * Assignment 3
 * disklist.c
 */
 
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "disk.h"

extern int errno;

int main(int argc, char *argv[] ) {
    struct stat sf;
    
    // open disk image file
    int fd;
    if (argc < 2) {
        printf("Error: not enough arguments.\n");
        exit(0);
    } else {
        fd = open(argv[1], O_RDONLY);
    }

    if (fd) {
        // create mmap for disk image file
        fstat(fd, &sf);

        char *p;
        p = mmap(NULL, sf.st_size, PROT_READ, MAP_SHARED, fd, 0);

        int base = BYTES_PER_SECTOR * ROOT_SECTOR;    //offset for Root Directory
        
        // check every Root Directory entry
        int i;
        for (i = 0; i < ROOT_DIR_ENTRIES; i++) {
            char *root_start = p+base+(i*ROOT_ENTRY_SIZE);
            int attr = p[base + 0x0b + i*ROOT_ENTRY_SIZE];
        
            // if file found
            if (strcmp(root_start, "") && ((attr&0xFC) == 0x00) && attr != 0x0F) {
                /* --- F/D label --- */
                char label = '\0';         // F/D
            
                // check if directory
                if (attr&0x10 == 0x10) {
                    label = 'D';
                } else {
                    label = 'F';
                }
        
                /* --- file size --- */
                char *file_size = root_start+0x1c;
                unsigned char buffer[4];
                int j;
                for (j = 0; j < 4; j++) {
                    buffer[j] = *file_size;
                    file_size++;
                }
                
                unsigned int size = (buffer[0]) | (buffer[1]<<8) | (buffer[2]<<16) | (buffer[3]<<24);
                
                /* --- file name --- */
                unsigned char file_name[20] = "";
                strncpy(file_name, root_start, 8);
                
                unsigned char *end = strrchr(file_name, ' ');
                if (end != NULL) {
                    *end = '.';
                } else {
                    file_name[8] = '.';
                }
                
                /* --- extension --- */
                char *extension = root_start+0x08;
                end = strrchr(file_name, '.');
                if (end != NULL) {
                    strncpy(end+1, extension, 3);
                }
                
                /* --- create date --- */
                unsigned int date[2];
                date[0] = p[base+(i*ROOT_ENTRY_SIZE)+(0x10)];
                date[1] = p[base+(i*ROOT_ENTRY_SIZE)+(0x10)+1];
                
                unsigned int total = (date[0] & 0xFF) | ((date[1] & 0xFF)<<8);
                unsigned int year = (total>>9) + 1980;
                unsigned int month = total>>5 & 0x0F;
                unsigned int day = total & 0x1F;
                
                /* --- create time --- */
                unsigned int time[2];
                time[0] = p[base+(i*ROOT_ENTRY_SIZE)+(0x0e)];
                time[1] = p[base+(i*ROOT_ENTRY_SIZE)+(0x0e)+1];
                
                unsigned int total2 = (time[0] & 0xFF) | ((time[1] & 0xFF)<<8);
                unsigned int hour = total2>>11;
                unsigned int minute = total2>>5 & 0x3F;
                
            /* --- OUTPUT --- */
            printf("%c %d %s %d-%02d-%02d %02d:%02d\n", label, size, file_name, year, month, day, hour, minute);
            
            }
            
        }

    } else {
        printf("Error: Failed to open file %s.\n", argv[1]);
        exit(0);
    }

}

















