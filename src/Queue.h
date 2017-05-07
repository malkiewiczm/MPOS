#ifndef QUEUE_H
#define QUEUE_H

#include "Node.h"
#include "Vector.h"

typedef struct {
	Node *head;
	Node *last;
	int count;
} Queue;

Queue *Queue_new(void);
void Queue_push(Queue*, void*);
void *Queue_pop(Queue*);
void Queue_pop_all(Queue*);
void Queue_push_all(Queue*, Vector*);

#endif
