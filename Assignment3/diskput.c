/* Boyang Jiao
 * V00800928
 * Nov 28, 2016
 * CSC360
 * Assignment 3
 * diskput.c
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "disk.h"

extern int errno;

void writeSectorValue(int entry, char *disk, int value);
int findFreeFAT(char *disk, int entry);

void writeSectorValue(int entry, char *disk, int value) {
    int base = FAT_OFFSET;
    int buffer[2];
    
    //if even
    if (entry%2 == 0) {
        buffer[0] = (value) & 0xFF;
        buffer[1] = ((value>>8) & 0x0F);
        
        // don't overwrite the high four bits
        buffer[1] |= (disk[base+1 + ((3*entry)/2)] & 0xF0);
        
        disk[base + ((3*entry)/2)] = buffer[0];
        disk[base+1 + ((3*entry)/2)] = buffer[1];
    
    //if odd
    } else {
        buffer[0] = ((value<<4) & 0xF0);
        buffer[1] = ((value>>4) & 0xFF);
        
        // don't overwrite the low four bits
        buffer[0] |= (disk[base + ((3*entry)/2)] & 0x0F);
        
        disk[base + ((3*entry)/2)] = buffer[0];
        disk[base+1 + ((3*entry)/2)] = buffer[1];
    }
}

int findFreeFAT(char *disk, int entry) {
    int i;
    int numSectors = getTotalSize(disk)/BYTES_PER_SECTOR;
    
    // find next available FAT entry (ignore current one)
    for (i = 2; i < numSectors; i++) {
        if (getSectorValue(i, disk) == 0x000 && i != entry) {
            return i;
        }
    }
}

int main(int argc, char *argv[] ) {
    struct stat sf;
    
    // open disk image file
    int fd;
    if (argc < 3) {
        printf("Error: not enough arguments.\n");
        exit(0);
    } else {
        fd = open(argv[1], O_RDWR);
    }

    if (fd) {
        char *target = argv[2];
		
		/* --- Create mmap of source file --- */
		int input_file;
        input_file = open(target, O_RDWR);
		if (input_file == -1) {
		    printf("File not found.\n");
		    exit(0);
		}
		
		// get file size
		struct stat input;
	    fstat(input_file, &input);
	    int input_file_size = input.st_size;
	    
		if (ftruncate(input_file, input_file_size) != 0) {
            printf("Error: ftruncate failed on source file.\n");
            printf("Value of errno: %s\n", strerror(errno));
            exit(0);
        } 
        
        char *src;
        src = mmap(NULL, input_file_size, PROT_READ, MAP_SHARED, input_file, 0);

        
        /* --- Create mmap of destination disk image --- */
        fstat(fd, &sf);
        
        if (ftruncate(fd, sf.st_size) != 0) {
            printf("Error: ftruncate failed on destination disk image.\n");
            printf("Value of errno: %s\n", strerror(errno));
            exit(0);
        } 
        
        char *dest;
        dest = mmap(NULL, sf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        int disk_free = getFreeSize(dest);
        
        if (input_file_size > disk_free) {
            printf("Not enough free space in the disk image.\n");
            exit(0);
        }
        
        // find first available FAT entry
        int free_FAT = findFreeFAT(dest, 0);

        // find open entry in Root directory
        int base = BYTES_PER_SECTOR * ROOT_SECTOR;
        
        int i;
        for (i = 0; i < ROOT_DIR_ENTRIES; i++) {
            char *root_start = dest+base+(i*ROOT_ENTRY_SIZE);
            int attr = dest[base + 0x0b + i*ROOT_ENTRY_SIZE];
            
            //empty slot found
            if (!strcmp(root_start, "") && attr == 0x00) {
            
                /* write filename */
                char filename[100];
                memset(filename, ' ', 100);               // pad with spaces
                strcpy(filename, target);
                char *eofn = strrchr(filename, '.');      // points to '.' before extension
                if (eofn != NULL) {
                    while (*eofn != '\0') {
                        *eofn = ' ';
                        eofn = eofn+1;
                    }
                
                }
                
                strncpy(root_start, filename, 8);
                
                /* write extension */
                char extension[100];
                memset(filename, ' ', 100);
                strcpy(extension, target);
                
                char *eofn2 = strrchr(extension, '.');           // points to '.' before extension
                if (eofn2 != NULL) {
                    char *ext = eofn2+1;
                    strncpy(root_start+0x08, ext, 3);
                }
                
                /* create time */
                time_t *temp = &input.st_ctime;
                struct tm *time = localtime(temp);
                
                unsigned int hour = ((time->tm_hour) << 11) & 0xF800;
                unsigned int minute = ((time->tm_min) << 5) & 0x07E0;
                unsigned int second = time->tm_sec & 0x001F;
                
                unsigned int total_time[2];
                total_time[0] = (hour | minute | second) & 0xFF;
                total_time[1] = ((hour | minute | second) >> 8) & 0xFF;
                
                // store in little endain
                dest[base+(i*32)+(0x0e)] = total_time[0];
                dest[base+(i*32)+(0x0e)+1] = total_time[1];
                
                
                /* create date */
                unsigned int year = ((time->tm_year - 80) << 9) & 0xFE00;
                unsigned int month = ((time->tm_mon + 1) << 5) & 0x01E0;
                unsigned int day = time->tm_mday & 0x001F;
                
                unsigned int total_date[2];
                total_date[0] = (year | month | day) & 0xFF;
                total_date[1] = ((year | month | day) >> 8) & 0xFF;
                
                // store in little endian
                dest[base+(i*32)+(0x10)] = total_date[0];
                dest[base+(i*32)+(0x10)+1] = total_date[1];
                

                /* first logical cluster */
                int flc[2];
                flc[0] = free_FAT & 0xFF;
                flc[1] = (free_FAT >> 8) & 0xFF;
                
                // store in little endian
                dest[base+(i*32)+(0x1a)] = flc[0];
                dest[base+(i*32)+(0x1a)+1] = flc[1];
                
                
                /* file size */
                int file_size[4];
                file_size[0] = input_file_size & 0xFF;
                file_size[1] = (input_file_size >> 8) & 0xFF;
                file_size[2] = (input_file_size >> 16) & 0xFF;
                file_size[3] = (input_file_size >> 24) & 0xFF;
                
                // store in little endian
                dest[base+(i*32)+(0x1c)] = file_size[0];
                dest[base+(i*32)+(0x1c)+1] = file_size[1];
                dest[base+(i*32)+(0x1c)+2] = file_size[2];
                dest[base+(i*32)+(0x1c)+3] = file_size[3];
                
                break;
            }
        
        }
        
        /* --- Write data to data sector --- */
        int entry = free_FAT;
        int next_FAT = 0;
        int phys_addr = 0;
        int count = 0;
        int done = 0;
        
        while (entry != 0xFFF) {
            // get next available FAT entry (ignoring current one)
            next_FAT = findFreeFAT(dest, entry);
            
            // write FAT entry
            writeSectorValue(entry, dest, next_FAT);
        
            phys_addr = (31+entry)*BYTES_PER_SECTOR;
            int j = 0;
            for (j = 0; j < BYTES_PER_SECTOR; j++) {
                dest[phys_addr] = src[count];
                count++;
                phys_addr++;
                
                if (count >= input_file_size) {
                    done = 1;
                    break;
                }
            }
            
            if (done) {
                writeSectorValue(entry, dest, 0xFFF);
                //break;
            }
            
            entry = getSectorValue(entry, dest);
        }

    } else {
        printf("Error: Failed to open file %s.\n", argv[1]);
        exit(0);
    }

}

















