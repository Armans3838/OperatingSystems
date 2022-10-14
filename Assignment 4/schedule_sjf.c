#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "schedulers.h"
#include "list.h"
#include "cpu.h"

struct node *head;
// struct node *tail = NULL;

// add a task to the list
// Shared with SJF and Priority
void add(char *name, int priority, int burst) {
    struct task* temp = malloc(sizeof(struct task));
    temp->name = name;
    temp->priority = priority;
    temp->burst = burst;
    insert(&head, temp);
}

// invoke the scheduler for shortest job first
void schedule() {
    while (head != NULL) {
        struct node* tempNode = head;
        struct task* tempTask = head->task;
        struct task* quickestTask = tempTask;

        while (tempNode->next != NULL) {
            tempNode = tempNode->next;
            tempTask = tempNode->task;
            if (tempTask->burst < quickestTask->burst) {
                quickestTask = tempTask;
            }
        }

        run(quickestTask, quickestTask->burst);
        delete(&head, quickestTask);
    }
}