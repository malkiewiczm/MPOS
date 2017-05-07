#ifndef ARTIST_H
#define ARTIST_H

#include "Vector.h"

typedef struct {
	char *name;
	Vector *albums;
	Vector *songs;
} Artist;

Artist *Artist_new(char*);

#endif
