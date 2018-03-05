#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


struct node {
    int pid;
    char *args;
    struct node *next;

};

extern struct node *head;

//linkedlist function prototypes
void addNode(int pid, char *args);
void delNode(int pid);
void printList();

void addNode( int pid, char *args ) {

    struct node *process = malloc(sizeof(struct node));

    process->pid = pid;
    process->args = args;
    process->next = NULL;

    //empty list
    if (head == NULL) {

        head = process;

    } else {

        //find last node in list
        struct node *p = head;
        while (p->next != NULL) { p = p->next; }

        p->next = process;
    }

}

void delNode( int pid ) {

    //empty list
    if (head == NULL) {
        return;
    } 

    //if head of list is wanted node
    else if (head->pid == pid) {

        struct node *p = head;
        head = head->next;
        free(p);
        return;

    } 
    else {

        //find node to remove
        struct node *p = head;
        struct node *prev = NULL;

        while (p != NULL) {
            if (p->pid != pid) {
                prev = p;
                p = p->next;
            } else {
                break;
            }
        }

        if (p == NULL) {
            printf("No process found with pid %d.\n", pid);
            return;
        } else if (p == head) {
            head = p->next;
        } else {
            prev->next = p->next;
        }

        free(p);
        return;

    }

}



void printList() {
    int counter = 0;

    //empty list
    if (head == NULL) {
        
    }

    //non-empty list
    else {
        struct node *p = head;

        while (p != NULL) {
            printf("%d: %s\n", p->pid, p->args);
            counter++;

            p = p->next;
        }

    }

    printf("Total background jobs: %d\n", counter);    

} 






















