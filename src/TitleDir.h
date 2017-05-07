#ifndef TITLEDIR_H
#define TITLEDIR_H

typedef struct {
	char *title;
	char *dir;
} TitleDir;

TitleDir *TitleDir_new(char*, char*);

#endif
