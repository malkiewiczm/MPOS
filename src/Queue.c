#include "Queue.h"
#include <stdlib.h>

Queue *Queue_new(void)
{
	Queue *buf = (Queue*)malloc(sizeof(Queue));
	buf->head = NULL;
	buf->last = NULL;
	buf->count = 0;
	return buf;
}


void Queue_push(Queue *self, void *value)
{
	Node *node = Node_new(value);
	if (self->last) {
		self->last->next = node;
	} else {
		self->head = node;
	}
	self->last = node;
	self->count++;
}


void *Queue_pop(Queue *self)
{
	if (self->head == NULL)
		return NULL;
	void *value = self->head->value;
	Node *tmp = self->head;
	self->head = self->head->next;
	if (self->head == NULL)
		self->last = NULL;
	free(tmp);
	self->count --;
	return value;
}

void Queue_pop_all(Queue *self)
{
	while (Queue_pop(self));
}

void Queue_push_all(Queue *self, Vector *vec)
{
	int i, n;
	for (i = 0, n = vec->count; i < n; i++) {
		Queue_push(self, Vector_get(vec, i));
	}
}

