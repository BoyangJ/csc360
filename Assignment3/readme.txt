Name: Boyang Jiao
UVicID: V00800928
Date: Nov 28, 2016
Course: CSC 360
Instructor: Kui Wu

Assignment 3: Simple File System

--------------------
--- INTRODUCTION ---
--------------------
This project involved implementing multiple programs that performed operations on a simple FAT12 file system.
The structure and organization of the FAT12 system was used.

---------------------------------
--- PROJECT FILE DESCRIPTIONS ---
---------------------------------
1. diskinfo.c - A program which lists various properties of a FAT12 disk image file: OS Name, Disk Label, total size, free size, number of files in Root Directory, number of FAT copies, and sectors per FAT.
    This program requires one argument: the input disk image file.

    EXAMPLE: $ ./diskinfo disk.IMA

2. disklist.c - A program which displays the contents of the disk image file's root directory. Information about each file in the disk will be shown, including F/D label (file/directory), size, filename, extension, create date, and create time.
    This program requires one argument: the input disk image file.

    EXAMPLE: $ ./disklist disk.IMA

3. diskget.c - A program which copies a file from the disk image to the current directory.
    This program requires two arguments: the input disk image file, and the file (and extension) to be retrieved.

    EXAMPLE: $ ./diskget disk.IMA ICEBERGS.TEX (where ICEBERGS.TEX is a file in the disk image - check with disklist)

4. diskput.c - A program which copies a file from the current directory to the disk image.
    This program requires two arguments: the input disk image file, and the file (and extension) to be put.

    EXAMPLE: $ ./diskput disk.IMA myFile.txt (where myFile.txt is a file in the current directory)

5. disk.h - A utility header file with functions and definitions used by all project files.

6. Makefile - The make file used for easy compilation of project files.

7. readme.txt - This readme file. Contains project description and instructions.


---------------------------
--- PROGRAM COMPILATION ---
---------------------------
A makefile has been included in the submission of this project, and can be used to easily compile all assets.

Instructions:
    1. In your terminal, switch to the project's directory.
    2. Type the command "make" to compile all the project files in accordance to the makefile.
        2a. (Alternative) The command "make -f makefile" will do the same thing.

Alternatively, the project files can be manually compiled separately with the following commands:
    1. "gcc diskinfo.c disk.h -o diskinfo"
    2. "gcc disklist.c disk.h -o disklist"
    3. "gcc diskget.c disk.h -o diskget"
    4. "gcc diskput.c disk.h -o diskput"

-------------------------
--- PROGRAM EXECUTION ---
-------------------------
Run the FAT12 operation programs with one of the following commands:
    "$ ./diskinfo [disk_image_file]"
    "$ ./disklist [disk_image_file]"
    "$ ./diskget [disk_image_file] [target_file]"
    "$ ./diskput [disk_image_file] [target_file]"

No more user input is required after the program begins.

----------------------
--- PROGRAM OUTPUT ---
----------------------
Output for diskinfo includes the following properties:
    OS Name, Label of the disk, Total size of the disk, Free size of the disk, Number of files in the root directory (not including subdirectories), number of FAT copies, and sectors per FAT.

Output for disklist includes the following properties:
    F/D (file/directory), size, filename, extension, create date, and create time.

diskget and diskput do not produce any output.

------------------
--- REFERENCES ---
------------------
Various sample code files from the UVic CSC 360 connex site were used, including:
A3-hint.c, FAT12 reference (1).pdf, and FAT12 reference (2).pdf.

Assignment specifications file can also be found on connex.























