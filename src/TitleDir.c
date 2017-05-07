#include "TitleDir.h"
#include <stdlib.h>

TitleDir *TitleDir_new(char *title, char *dir)
{
	TitleDir *buf = (TitleDir*)malloc(sizeof(TitleDir));
	buf->title = title;
	buf->dir = dir;
	return buf;
}
