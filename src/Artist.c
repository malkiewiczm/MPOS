#include "Artist.h"
#include <stdlib.h>

Artist *Artist_new(char* name)
{
	Artist *buf = (Artist*)malloc(sizeof(Artist));
	buf->name = name;
	buf->albums = Vector_new();
	buf->songs = Vector_new();
	return buf;
}
