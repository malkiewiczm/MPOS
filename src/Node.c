#include "Node.h"
#include <stdlib.h>

Node *Node_new(void *value)
{
	Node *buf = (Node*)malloc(sizeof(Node));
	buf->value = value;
	buf->next = NULL;
	return buf;
}
