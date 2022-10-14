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

void schedule() {
    while (head != NULL) {
        struct node* tempNode = head;
        struct task* tempTask = head->task;
        struct task* highestPriority = tempTask;

        while (tempNode->next != NULL) {
            tempNode = tempNode->next;
            tempTask = tempNode->task;
            if (tempTask->priority > highestPriority->priority) {
                highestPriority = tempTask;
            }
        }

        run(highestPriority, highestPriority->priority);
        delete(&head, highestPriority);
    }
}