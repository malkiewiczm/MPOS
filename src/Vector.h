#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	void **arr;
	int count;
	int size;
} Vector;

Vector *Vector_new(void);
void *Vector_get(Vector*, int);
void Vector_set(Vector*, int, void*);
void Vector_push(Vector*, void*);

#endif
