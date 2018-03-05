Name: Boyang Jiao
UVicID: V00800928
Date: Nov 4, 2016
Course: CSC 360
Instructor: Kui Wu

Assignment 2: Multi-Flow Scheduler (MFS)

--------------------
--- INTRODUCTION ---
--------------------
This project involved creating a simulator of a router to schedule multiple transmission flows over a shared network.
This was done using pthreads, mutex, and condition variables.


---------------------------------
--- PROJECT FILE DESCRIPTIONS ---
---------------------------------
1. MFS.c - Code for the multi-flow scheduler program. This program requires one argument (the input file), and can be run directly through command line, after compilation, with "./MFS [input_file]"

    EXAMPLE: $ ./MFS flows.txt

2. Makefile - The make file used for easy compilation of project files.

3. design.txt - The design document for this program. It covers some basic descriptions of the data structures, system calls, and algorithms used in the project.

4. readme.txt - This readme file. Contains project description and instructions.


------------------
--- INPUT FILE ---
------------------
The input file must adhere to the following format:

X
[id]:[a_time],[t_time],[prio]
.
.
.

where X is the number of flows that will be simulated, followed by X lines each containing info about one flow.
id - the unique identifier for a flow
a_time - the arrival time of the flow, in 1/10 seconds (ex. 4 = 0.4s)
t_time - the transmission time of the flow, in 1/10 seconds (ex. 60 = 6s)
prio - the priority of the flow, which ranges from 1 (highest priority) to 10 (lowest priority), inclusive.


---------------------------
--- PROGRAM COMPILATION ---
---------------------------
A makefile has been included in the submission of this project, and can be used to easily compile all assets.

Instructions:
    1. In your terminal, switch to the project's directory.
    2. Type the command "make" to compile all the project files in accordance to the makefile.
        2a. (Alternative) The command "make -f makefile" will do the same thing.

Alternatively, the project files can be manually compiled separately with the following commands:
    1. "gcc MFS.c -pthread -o MFS"


-------------------------
--- PROGRAM EXECUTION ---
-------------------------
Run the process manager program with command "$ ./MFS [input_file]"

No more user input is required after the program begins.


----------------------
--- PROGRAM OUTPUT ---
----------------------
The program output begins with a single line displaying the total number of flows it expects to process.

Then, the program will print a message whenever one of the following events occurs:
    1. A flow arrives - the flow's id, arrival time, transmission time, and priority will be displayed.
    2. If, when a flow arrives, another flow is currently transmitting, a message will be printed detailing which flow the newly arriving flow is waiting for.
    3. A flow starts transmission - the flow's id and starting time will be printed.
    4. A flow finishes transmission - the flow's id and ending time will be printed.

Once all the flows have finished transmitting, a message will be printed notifying the user that all threads have finished.


------------------
--- REFERENCES ---
------------------
Various sample code files from the UVic CSC 36 connex site were used, including:
A2-hint.c, and CSC 360-Tutorial-A2-recap-new.pdf.

Assignment specifications file can also be found on connex.























