#include <stdio.h>
#include <stdlib.h>

struct QueueNode {
    struct node* tempNode;
	struct QueueNode* next;
};

// Queue structure and its operations
struct QueueNode* newNode(struct node* param);
struct Queue* createQueue();
void enQueue(struct Queue* q, struct node* param);
void deQueue(struct Queue* q);