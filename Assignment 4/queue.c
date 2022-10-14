#include "queue.h"

struct Queue {
	struct QueueNode *front, *rear;
};

// queue delcaration
struct QueueNode* newNode(struct node* param)
{
	struct QueueNode* temp = (struct QueueNode*)malloc(sizeof(struct QueueNode));
	temp->tempNode = param;
	temp->next = NULL;
	return temp;
}

// queue delcaration
struct Queue* createQueue()
{
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;
	return q;
}

// putting values into the queue
void enQueue(struct Queue* q, struct node* param)
{
	struct QueueNode* temp = newNode(param);
	if (q->rear == NULL) {
		q->front = q->rear = temp;
		return;
	}
	q->rear->next = temp;
	q->rear = temp;
}

// removing values from the queue
void deQueue(struct Queue* q)
{
	if (q->front == NULL)
		return;
	struct QueueNode* temp = q->front;
	q->front = q->front->next;
	if (q->front == NULL)
		q->rear = NULL;
	free(temp);
}