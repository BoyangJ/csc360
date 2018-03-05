#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "processlist.h"

#define MAX_ARGS 200
#define MAX_STR_LEN 4096


//function prototypes
void update_bg_process();
int getUserInput(char **args);
void processCommands(char **args, int num_args);

//command function prototypes
void startBackgroundProcess(char **args, int num_args, struct node *head);
void killProcess(int pid);
void stopProcess(int pid);
void continueProcess(int pid);
void getStatus(int pid);

//head of process list
struct node *head = NULL;

//file constants
const char *procDir = "/proc";
const char *statDir = "stat";
const char *statusDir = "status";

void update_bg_process() {
    int pid = 1;
    int status = 0;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Process with pid %d was terminated.\n", pid);
        delNode(pid);    
    }

    return;
}

int getUserInput(char **args) {
    char *input = NULL;
    char *prompt = "PMan: > ";

    input = readline(prompt);

    //if user input is empty
    if (strcmp(input, "") == 0) {
        return 0;
    }

    //tokenize user input
    char *p;
    p = strtok(input, " ");

    int num_args = 0;
    while (p != NULL) {

        args[num_args] = p;
        p = strtok(NULL, " ");
        num_args++;
    }
    
    return num_args;
}

void processCommands(char **args, int num_args) {
    //first argument is command
    char *command = args[0];

    /* --- bg --- */
    if (strcmp(command, "bg") == 0) {
        
        //bg must be followed by another command
        if (num_args < 2) {
            printf("Error: not enough arguments.\n");
            return;
        }

        startBackgroundProcess(args, num_args, head);
    }

    /* --- bglist --- */
    else if (strcmp(command, "bglist") == 0) {
        printList(head);
    }

    /* --- bgkill --- */
    else if (strcmp(command, "bgkill") == 0) {
        if (num_args < 2) {
            printf("Error: not enough arguments.\n");
            return;
        }
        int pid = 0;
        if (sscanf(args[1], "%d", &pid) == 1) {
            killProcess(pid);
        } else {
            printf("Error: pid must be integer.\n");
            return;
        }

    }

    /* --- bgstop --- */
    else if (strcmp(command, "bgstop") == 0) {
        if (num_args < 2) {
            printf("Error: not enough arguments.\n");
            return;
        }
        int pid = 0;
        if (sscanf(args[1], "%d", &pid) == 1) {
            stopProcess(pid);
        } else {
            printf("Error: pid must be an integer.\n");
            return;
        } 

    }

    /* --- bgstart --- */
    else if (strcmp(command, "bgstart") == 0) {
        if (num_args < 2) {
            printf("Error: not enough arguments.\n");
            return;
        }
        int pid = 0;
        if (sscanf(args[1], "%d", &pid) == 1) {
            continueProcess(pid);
        } else {
            printf("Error: pid must be an integer.\n");
            return;
        }

    }

    /* --- pstat --- */
    else if (strcmp(command, "pstat") == 0) {
        if (num_args < 2) {
            printf("Error: not enough arguments.\n");
            return;
        }
        int pid = 0;
        if (sscanf(args[1], "%d", &pid) == 1) {
            getStatus(pid);
        } else {
            printf("Error: pid must be an integer.\n");
            return;
        }

    }

    /* --- exit --- */
    else if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
        //kill every process in list
        while (head != NULL) {
            killProcess(head->pid);
            sleep(1);
        }
        exit(1);
    }

    /* --- other --- */
    else {
        printf("%s: command not found.\n", command);
    }

    return;
}

int main(){

    //pointer array of command arguments
    char *args[MAX_ARGS];
    int num_args;

    while (1) {
        //clear args from last command
        *args = "";
        num_args = 0;

        //update background processes
        update_bg_process();

        //wait for user input
        num_args = getUserInput(args);

        update_bg_process();
        
        //process user input commands
        processCommands(args, num_args);

    }
}


void startBackgroundProcess(char **args, int num_args, struct node *head) {
    //create child process to run command
    int pid = fork();

    if (pid >= 0) {

        //child process
        if (pid == 0) {
            
            //create new array of arguments
            char *argv[num_args];

            int i;
            for (i = 0; i < num_args-1; i++) {
                argv[i] = args[i+1];
            }
            argv[num_args-1] = NULL;

            printf("Executing background process.\n");
            execvp( args[1], argv );
            printf("Error: could not execute command.\n.");
            exit(1);

        } 

        //parent process
        else {
            //add child process to list of processes
            addNode(pid, args[1]);
            sleep(2); 
        }

    } else {
        //fork failed
        printf("ERROR: fork failed.\n");

    }

}

void killProcess(int pid) {

    int status = 0;
    waitpid(pid, &status, WNOHANG | WCONTINUED | WUNTRACED);

    //if a process is stopped, it must first be continued, then killed.
    if (WIFSTOPPED(status) != 0) {
        kill(pid, SIGCONT);
    }    

    if (!(kill(pid, SIGTERM))) {
        printf("Process %d terminated.\n", pid);
        delNode(pid);
    } else {
        printf("Error: Process %d does not exist.\n", pid);
    }

}

void stopProcess(int pid) {

    if (!(kill(pid, SIGSTOP))) {
        printf("Process %d stopped.\n", pid);
    } else {
        printf("Error: Process %d does not exist.\n", pid);
    }

}

void continueProcess(int pid) {

    if (!(kill(pid, SIGCONT))) {
        printf("Process %d continued.\n", pid);
    } else {
        printf("Error: Process %d does not exist.\n", pid);
    }

}

void getStatus(int pid) {

    char procPidDir[MAX_STR_LEN];
    
    //check access to \proc\[pid] directory
    sprintf(procPidDir, "%s/%d", procDir, pid);
    if (access(procPidDir, R_OK) < 0) {
        if (errno == ENOENT) {
            printf("Error: Process %d does not exist.\n", pid);
        } else {
            printf("Error: Could not access %s.\n", procDir);
        }
        return;
    }

    FILE *statFile;
    char fullStatDir[MAX_STR_LEN];
    FILE *statusFile;
    char fullStatusDir[MAX_STR_LEN];

    sprintf(fullStatDir, "%s/%s", procPidDir, statDir);
    sprintf(fullStatusDir, "%s/%s", procPidDir, statusDir);

    //Open /proc/[pid]/stat and tokenize each element
    char file_contents[MAX_STR_LEN];

    statFile = fopen(fullStatDir, "r");
    if (statFile) {
        fgets(file_contents, MAX_STR_LEN, statFile);
    }

    //Store each section of file_contents into an array
    char *stat_elements[MAX_ARGS];
    char *p;
    p = strtok(file_contents, " ");

    int i = 0;
    while (p != NULL) {
        stat_elements[i] = p;
        p = strtok(NULL, " ");
        i++;
    }
    
    //report elements 1(comm), 2(state), 13(utime), 14(stime), 23(rss)
    printf("Status of process %d:\n", pid);
    printf("comm:\t%s\n", stat_elements[1]);
    printf("state:\t%s\n", stat_elements[2]);
    printf("utime:\t%s\n", stat_elements[13]);
    printf("stime:\t%s\n", stat_elements[14]);
    printf("rss:\t%s\n", stat_elements[23]);


    //Open /proc/[pid]/status and get
    statusFile = fopen(fullStatusDir, "r");
    char status_elements[MAX_STR_LEN][MAX_ARGS];

    if (statusFile) {
        i = 0;
        while (fgets(status_elements[i], MAX_STR_LEN, statusFile) != NULL) {
            i++;
        }
    }

    //report elements 46(voluntary_ctxt_switches) and 47(nonvoluntary_ctxt_switches)
    printf("%s", status_elements[46]);
    printf("%s", status_elements[47]);

}












