#ifndef NODE_H
#define NODE_H

typedef struct _node {
	struct _node *next;
	void *value;
} Node;

Node *Node_new(void*);

#endif
