/* Boyang Jiao
 * V00800928
 * Nov 28, 2016
 * CSC360
 * Assignment 3
 * diskinfo.c
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

/* --- Function Prototypes --- */
void getOSName(char *src, char *os_name);
void getLabelName(char *src, char *label_name);
int getNumFiles(char *src);
int getNumFATs(char *src);
int getSectorsPerFAT(char *src);


void getOSName(char *src, char *os_name) {
    // os name found at boot sector + 3
    unsigned char *p = src + OS_OFFSET;
    int i;
    for (i = 0; i < 8; i++) {
        os_name[i] = *p;
        p++;
    }
    return;
}

void getLabelName(char *src, char *label_name) {
    // check Root directory first
    int base = BYTES_PER_SECTOR * ROOT_SECTOR;

    int i;
    for (i = 0; i < ROOT_DIR_ENTRIES; i++) {
        // 0x0b is offset for attribute
        int attr = src[base + 0x0b + i*ROOT_ENTRY_SIZE];

        // flag 0x08 for Volume Label
        if (attr == 0x08) {
            char *p = src+base+(i*ROOT_ENTRY_SIZE);

            for (i = 0; i < 8; i++) {
                label_name[i] = *p;
                p++;
            }
            break;
        }
    }
    
    // if Volume Label not in Root Directory, check Boot Sector
    if (!strcmp(label_name, "")) {
        // Boot Sector volume label
        label_name = src + LABEL_OFFSET;
    }

    return;
}

int getNumFiles(char *src) {
    // check Root Directory for number of files
    int base = BYTES_PER_SECTOR * ROOT_SECTOR;

    int i;
    int file_count = 0;
    for (i = 0; i < ROOT_DIR_ENTRIES; i++) {
        char *filename = src+base+(i*ROOT_ENTRY_SIZE);
        int attr = src[base + 0x0b + i*ROOT_ENTRY_SIZE];

        // if filename not empty, and attribute indicates it's a file (not directory)
        if (strcmp(filename, "") && ((attr&0xFC) == 0x00) && attr != 0x0F) {
            file_count++;
        }
    }
    return file_count;
}

int getNumFATs(char *src) {
    // check Boot Sector for number of FATs
    unsigned char *buffer = src + NUM_FAT_OFFSET;
    int num_FATs = *buffer;

    return num_FATs;
}

int getSectorsPerFAT(char *src) {
    // check Boot Sector for number of sectors per FAT
    unsigned char *bytes[2];
    int num_sectors = 0;

    bytes[0] = src + SECTORS_PER_FAT_OFFSET;
    bytes[1] = src + SECTORS_PER_FAT_OFFSET + 1;

    // read in little endian
    num_sectors += *bytes[0] | (*bytes[1]<<8);

    return num_sectors;
}


int main(int argc, char *argv[] ) {
    struct stat sf;
    
    // open the disk image file
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

        // get all desired properties
        char *os_name = malloc(sizeof(char)*8);
        getOSName(p, os_name);
        char *label_name = malloc(sizeof(char)*11);
        getLabelName(p, label_name);
        int total_size = getTotalSize(p);
        int free_size = getFreeSize(p);

        int num_files = getNumFiles(p);
        int num_FATs = getNumFATs(p);
        int FAT_sectors = getSectorsPerFAT(p);

        /* --- OUTPUT --- */
        printf("OS Name: %s\n", os_name);
        printf("Label of the disk: %s\n", label_name);
        printf("Total size of the disk: %d\n", total_size);
        printf("Free size of the disk: %d\n", free_size);
        
        printf("\n===============\n");
        printf("The number of files in the root directory (not including subdirectories): %d\n", num_files);

        printf("\n===============\n");
        printf("Number of FAT copies: %d\n", num_FATs);
        printf("Sectors per FAT: %d\n", FAT_sectors);

        free(os_name);
        
    } else {
        printf("Error: Failed to open file %s.\n", argv[1]);
        exit(0);
    }

}

















