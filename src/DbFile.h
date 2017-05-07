#ifndef DBFILE_H
#define DBFILE_H

#include "Vector.h"
#include "Artist.h"
#include "TitleDir.h"

typedef struct {
	char *fname;
	Vector *artists;
} DbFile;

DbFile *DbFile_new(char*);
char DbFile_explore(DbFile*, Artist*, int);
char DbFile_load(DbFile*);

#endif
