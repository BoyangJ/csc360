/* Boyang Jiao
 * V00800928
 * Nov 28, 2016
 * CSC360
 * Assignment 3
 * diskget.c
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "disk.h"

extern int errno;

int main(int argc, char *argv[] ) {
    struct stat sf;
    
    // open disk image file
    int fd;
    if (argc < 3) {
        printf("Error: not enough arguments.\n");
        exit(0);
    } else {
        fd = open(argv[1], O_RDONLY);
    }

    if (fd) {
        // create mmap of disk image file
        fstat(fd, &sf);

        char *src;
        src = mmap(NULL, sf.st_size, PROT_READ, MAP_SHARED, fd, 0);
        
        char *target = argv[2];

	    /* --- Check for file in Root Dir --- */
	    int base = BYTES_PER_SECTOR * ROOT_SECTOR;
	    
	    int file_found = 0;
	    unsigned int file_size = 0;
	    unsigned int first_cluster = 0;
	    int i = 0;
	    for (i = 0; i < ROOT_DIR_ENTRIES; i++) {
            char *root_start = src+base+(i*ROOT_ENTRY_SIZE);
            int attr = src[base + 0x0b + i*ROOT_ENTRY_SIZE];
        
            // if file found
            if (strcmp(root_start, "") && ((attr&0xFC) == 0x00) && attr != 0x0F) {
            
	            // filename
	            unsigned char file_name[20] = "";
                strncpy(file_name, root_start, 8);
                
                unsigned char *end = strchr(file_name, ' ');
                if (end != NULL) {
                    *end = '.';
                } else {
                    file_name[8] = '.';
                }
                
                // extension
                char *extension = root_start+0x08;
                end = strrchr(file_name, '.');
                if (end != NULL) {
                    strncpy(end+1, extension, 3);
                }
                
                //Check if filename matches target
                if (!strcmp(file_name, target)) {
                    /* --- Store file size and first logical cluster --- */
                    // file size
                    char *size_loc = root_start+0x1c;
                    unsigned char buffer[4];
                    int j;
                    for (j = 0; j < 4; j++) {
                        buffer[j] = *size_loc;
                        size_loc++;
                    }
                
                    file_size = (buffer[0]) | (buffer[1]<<8) | (buffer[2]<<16) | (buffer[3]<<24);
                    
                    // first cluster
                    char *FLC_loc = root_start+0x1a;
                    unsigned char buffer2[2];
                    for (j = 0; j <2; j++) {
                        buffer2[j] = *FLC_loc;
                        FLC_loc++;
                    }
                    
                    first_cluster = (buffer2[0]) | (buffer2[1]<<8);
                    
                    file_found = 1;
                    break;
                }
                
            }
	    
	    }
	    
	    if (file_found) {
	        /* --- Create new file in current directory --- */
            int output_file;
            output_file = open(target, O_RDWR|O_CREAT, 0777);
            
            if (output_file != -1) {
                /* --- Create mmap of new file --- */
                
                if (ftruncate(output_file, file_size) != 0) {
                    printf("Error: ftruncate failed.\n");
                    printf("Value of errno: %d\n", errno);
                    exit(0);
                }
                
                char *dest;
                dest = mmap(NULL, file_size, PROT_WRITE, MAP_SHARED, output_file, 0);
            
                /* --- Copy all data sectors of file to dest --- */
                
                // change destination file permissions
                if (fchmod(output_file, 0777) != 0) {
                    printf("Error: failed to set destination file permissions.\n");
                    printf("Value of errno: %d\n", errno);
                    exit(0);
                }
                
                int entry = first_cluster;
                int phys_addr = 0;
                int count = 0;
                
                // loop to get next FAT entry
                while (getSectorValue(entry, src) < 0xFF8) {
                    
                    phys_addr = (entry+31)*BYTES_PER_SECTOR;
                    
                    int j = 0;
                    for (j = 0; j < BYTES_PER_SECTOR; j++) {
                        dest[count] = src[phys_addr];
                        count++;
                        phys_addr++;
                    }
                
                    entry = getSectorValue(entry, src);
                }
                // last sector
                phys_addr = (entry+31)*BYTES_PER_SECTOR;
                int j = 0;
                for (j = 0; j < BYTES_PER_SECTOR; j++) {
                    dest[count] = src[phys_addr];
                    count++;
                    phys_addr++;
                }
                
            } else {
                printf("Error: failed to open destination file %s.\n", target);
                exit(0);
            }
            
	    } else {
	        printf("File not found.\n");
	        exit(0);
	    }

    } else {
        printf("Error: Failed to open file %s.\n", argv[1]);
        exit(0);
    }

}

















