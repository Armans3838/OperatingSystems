#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *head = NULL;
struct node *tail = NULL;

// add a task to the list
// Shared with FCFS and RR
void add(char *name, int priority, int burst) {
    struct task* temp = malloc(sizeof(struct task));
    temp->name = name;
    temp->priority = priority;
    temp->burst = burst;
    insertTail(&tail, &head, temp);
}

// implement round roubin scheduling
void schedule() {
    while (head != NULL) {
        struct task* tempTask = head->task;
            
        if(tempTask->burst > 10){
            run(tempTask, 10);
            tempTask->burst -=10;
            delete(&head, tempTask);
            add(tempTask->name, tempTask->priority, tempTask->burst);
        }else if (tempTask->burst <= 10){
            run(tempTask, tempTask->burst);
            delete(&head, tempTask);
        }
    }
}