#include <stdlib.h>
#include <stdio.h>
#include "Vector.h"

Vector *Vector_new(void)
{
	Vector *buf = (Vector*)malloc(sizeof(Vector));
	buf->count = 0;
	buf->size = 10;
	buf->arr = (void**)malloc(sizeof(void*) * buf->size);
	return buf;
}

void *Vector_get(Vector *self, int index)
{
	return self->arr[index];
}

void Vector_set(Vector *self, int index, void *value)
{
	if (index >= self->size) {
		self->size <<= 1;
		self->arr = (void**)realloc(self->arr, sizeof(void*) * self->size);
	}
	self->arr[index] = value;
	if (index >= self->count) {
		self->count = index + 1;
	}
}

void Vector_push(Vector *self, void *value)
{
	Vector_set(self, self->count, value);
}
