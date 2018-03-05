#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_STR_LEN 4096
#define MAX_FLOWS 200

struct flow
{
    float arrivalTime;
    float transTime;
    int priority;
    int id;
};

//function prototypes
float getTime();
void addToQueue(struct flow *item);
void cascadeQueue(int position);
void printQueue();
void requestPipe(struct flow *item);
void releasePipe();
void *thrFunction(void *flowItem);

int numFlows;
struct flow flowList[MAX_FLOWS];   // parse input in an array of flow

int waitingFlows = 0;
struct flow *queueList[MAX_FLOWS];  // store waiting flows while transmission pipe is occupied.

pthread_t threadList[MAX_FLOWS]; // each thread executes one flow

int busy = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;  

float getTime(struct timeval begin) {
    struct timeval current;
    if (gettimeofday(&current, NULL) == -1) {
        printf("Error: gettimeofday failed.\n");
        exit(0);
    }

    int microseconds = (current.tv_sec - begin.tv_sec)*1000000 + ((int)current.tv_usec - (int)begin.tv_usec);
    float seconds = microseconds / 1000000.0;

    return seconds;
}

void addToQueue(struct flow *item) {
    //find position of queue to add to
    //empty queue
    if (waitingFlows == 0) {
        queueList[0] = item;
        waitingFlows++;

    } else {
        int i;
        int position = waitingFlows;
        for (i = 0; i < waitingFlows; i++) {

            //highest priority value at head of list
            if (item->priority > queueList[i]->priority) {
                position = i;
                break;
    
            //tied in priority, check arrival time
            } else if (item->priority == queueList[i]->priority) {
                if (item->arrivalTime > queueList[i]->arrivalTime) {
                    position = i;
                    break;

                //tied in priority and ATime, check transmission time
                } else if (item->arrivalTime == queueList[i]->arrivalTime) {
                    if (item->transTime > queueList[i]->transTime) {
                        position = i;
                        break;

                    //tied in prio, ATime, and TTime, check id
                    } else if (item->transTime == queueList[i]->transTime) {
                        if (item->id > queueList[i]->id) {
                            position = i;
                            break;

                        }
                    }
                }
            }
        }
        
        //position to add item to has been found
        cascadeQueue(position);
        queueList[position] = item;
        waitingFlows++;

        
    }
}

//DEBUG ONLY
void printQueue() {
    int i;
    for (i = 0; i < waitingFlows; i++) {
        printf("   QUEUE %d = Flow %d, prio=%d, ATime=%.2f, TTime=%.2f\n", i, queueList[i]->id, queueList[i]->priority, queueList[i]->arrivalTime, queueList[i]->transTime);

    }
}

void cascadeQueue(int position) {
    int i;
    for (i = numFlows; i > position; i--) {
        queueList[i] = queueList[i-1];
    }

}

void requestPipe(struct flow *item) {
    //lock mutex
    if (pthread_mutex_lock(&mutex) != 0) {
        printf("Error: Pthread_mutex_lock failed!\n");
        exit(0);
    }

    if (busy == 0 && waitingFlows == 0) {
        
        //set busy flag
        busy = item->id;

        //unlock mutex
        if (pthread_mutex_unlock(&mutex) != 0) {
            printf("Error: Pthread_mutex_unlock failed!\n");
        }
        return;
    }
    addToQueue(item);
    printf("Flow %2d waits for the finish of flow %2d.\n", item->id, busy);

    //wait until pipe ready and flow at top of queue
    while (busy != 0 || queueList[waitingFlows-1]->id != item->id) {
        int wait = pthread_cond_wait(&cv, &mutex);
        if (wait != 0) {
            printf("Error: Pthread_cond_wait failed.\n");
            exit(0);
        }

    }

    //update queue
    busy = item->id;
    queueList[waitingFlows] = NULL;
    waitingFlows--;

    //unlock mutex
    if (pthread_mutex_unlock(&mutex) != 0) {
        printf("Error: Pthread_mutex_unlock failed!\n");
        exit(0);
    }

}

void releasePipe() {
    busy = 0;

    if (pthread_mutex_lock(&mutex) != 0) {
        printf("Error: Pthread_mutex_lock failed!\n");
        exit(0);
    }

    if (pthread_cond_broadcast(&cv) != 0) {
        printf("Error: Pthread_cond_broadcast failed.\n");
        exit(0);
    }

    if (pthread_mutex_unlock(&mutex) != 0) {
        printf("Error: Pthread_cond_unlock failed!\n");
        exit(0);
    }
}

// entry point for each thread created
void *thrFunction(void *flowItem) {
    struct flow *item = (struct flow*) flowItem;
    struct timeval start;
    float time = 0;

    //initialize thread start time
    if (gettimeofday(&start, NULL) == -1) {
        printf("Error: gettimeofday failed.\n");
        exit(0);
    }

    // Wait until arrival time
    if (usleep(item->arrivalTime * 1000000) == -1) {
        printf("Error: usleep failed.\n");
        exit(0);
    }
    printf("Flow %2d arrives: arrival time (%.2f), transmission time (%.2f), priority (%2d).\n", item->id, item->arrivalTime, item->transTime, item->priority);
    
    requestPipe(item);

    time = getTime(start);
    printf("Flow %2d starts its transmission at time %.2f.\n", item->id, time);

    if (usleep(item->transTime * 1000000) == -1) {
        printf("Error: usleep failed.\n");
        exit(0);
    }

    time = getTime(start);
    printf("Flow %2d finishes its transmission at time %.2f.\n", item->id, time);

    releasePipe();
    
    return 0;
}

int main(int argc, char *argv[] ) {
    FILE *fp;
    char buffer[MAX_STR_LEN];

    if (argc < 2) {
        printf("Error: not enough arguments.\n");
        exit(0);
    } else {
        fp = fopen(argv[1], "r");
    }

    if (fp) {
        //get number of flows
        if (fgets(buffer, MAX_STR_LEN, fp) == NULL) {
            printf("Error: fgets failed.\n");
            exit(0);
        }
        numFlows = atoi(buffer);
        printf("Number of flows: %d\n",numFlows);

        //store flows in flowList
        int i;
        char *p;

        for (i = 0; i < numFlows; i++) {
            if (fgets(buffer, MAX_STR_LEN, fp) == NULL) {
                printf("Error: fgets failed.\n");
                exit(0);
            }

            //id
            p = strtok(buffer, ":,");
            if (atoi(p) <= 0) {
                printf("Error: Invalid flow id value found.\n");
                exit(0);
            }
            flowList[i].id = atoi(p);

            //arrival time
            p = strtok(NULL, ":,");
            if (atoi(p) < 0) {
                printf("Error: Invalid arrival time found.\n");
                exit(0);
            }
            flowList[i].arrivalTime = atoi(p)/10.0;

            //transmission time
            p = strtok(NULL, ":,");
            if (atoi(p) <= 0) {
                printf("Error: Invalid transmission time found.\n");
                exit(0);
            }
            flowList[i].transTime = atoi(p)/10.0;

            //priority
            p = strtok(NULL, ":,");
            if (atoi(p) < 1 || atoi(p) > 10) {
                printf("Error: Invalid priority found.\n");
                exit(0);
            }
            flowList[i].priority = atoi(p);
           
        }

        fclose(fp);

    } else {
        printf("Error: Failed to open file %s.\n", argv[1]);
        exit(0);
    }
    
    int i;
    //Create thread for each flow in flowList
    for (i = 0; i < numFlows; i++) {
        if (pthread_create(&threadList[i], NULL, thrFunction, (void *)&flowList[i]) != 0) {
            printf("Error: failed to create thread %d.\n", flowList[i].id);
            exit(0);
        }
    }

    //Wait for all threads to terminate
    for (i = 0; i < numFlows; i++) {
        if (pthread_join(threadList[i], NULL) != 0) {
            printf("Error: pthread_join failed.\n");
            exit(0);
        }
    }

    //Destroy mutex and condition variable
    if (pthread_mutex_destroy(&mutex) != 0) {
        printf("Error: Pthread_mutex_destroy failed!\n");
        exit(0);
    }

    if (pthread_cond_destroy(&cv) != 0) {
        printf("Error: Pthread_cond_destroy failed!\n");
        exit(0);
    }

    printf("All threads have been terminated.\n");
    return 0;
}
