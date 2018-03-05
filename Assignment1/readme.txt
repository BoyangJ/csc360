Name: Boyang Jiao
UVicID: V00800928
Date: Oct 3, 2016
Course: CSC 360
Instructor: Kui Wu

Assignment 1: Process Manager (PMan)

--------------------
--- INTRODUCTION ---
--------------------
This project involved creating a process manager capable of running background processes using the fork() and exec*() family commands.


---------------------------------
--- PROJECT FILE DESCRIPTIONS ---
---------------------------------
1. PMan.c - Code for the process manager program. This program requires no arguments, and can be run directly through command line (after compilation with the makefile) with "./PMan"

    EXAMPLE: $ ./PMan

2. processlist.h - Utility file that contains functions and structs for a linked list data structure.

3. Makefile - The make file used for easy compilation of the project files.

4. readme.txt - This readme file. Contains project description and instructions.


---------------------------
--- PROGRAM COMPILATION ---
---------------------------
A makefile has been included in the submission of this project, and can be used to easily compile all assets.

Instructions:
    1. In your terminal, switch to the project's directory.
    2. Type the command "make" to compile all the project files in accordance to the makefile.
        2a. (Alternative) The command "make -f makefile" will do the same thing.

Alternatively, the project files can be manually compiled separately with the following commands:
    1. "gcc PMan.c processlist.h -o Pman"


-------------------------
--- PROGRAM EXECUTION ---
-------------------------
Run the process manager program with command "$ ./PMan"

The following commands are supported by the program:

bg [args] - Starts execution of a linux command in the background, where [args] is the Linux command.
    EXAMPLE - "PMan: > bg cat foo.txt" will cause "cat foo.txt" to execute in the background, printing out the contents of foo.txt. Once the process terminates, PMan will notify the user.

bglist - Displays a list of background processes currently running which were started using PMan's bg command.

bgkill [pid] - Terminates a process with pid [pid]

bgstop [pid] - Stops a process with pid [pid]

bgstart [pid] - Continues a stopped process with pid [pid]

pstat [pid] - Reports the following information about the process with pid [pid]:
    - comm (filename) of the executed process,
    - state (R Running, S Sleeping, T Stopped, etc),
    - utime (amount of time process has been scheduled in user mode),
    - stime (amount of time process has been scheduled in kernel mode),
    - rss (resident set size),
    - voluntary_ctxt_switches,
    - nonvoluntary_ctxt_switches

exit/quit - Terminates all processes started by PMan and exits the program.

Inputting an unrecognized command will prompt a "command not found" error message.


------------------
--- REFERENCES ---
------------------
Various sample code files from the UVic CSC 36 connex site were used, including:
inf.c, rsi.c, A1-hunt-official.c, and Makefile.txt

Assignment specifications file can also be found on connex.























