#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "DbFile.h"

DbFile *DbFile_new(char* fname)
{
	DbFile *buf = (DbFile*)malloc(sizeof(DbFile));
	buf->fname = fname;
	buf->artists = Vector_new();
	return buf;
}

char *DbFile_prune_song_name(char *name)
{
	int to = 0;
	int i;
	for (i = 0; i < 200; i++) {
		if (name[i] == '\0')
			break;
		if (name[i] == '.')
			to = i;
	}
	char *buf = (char*)malloc(sizeof(char) * (to + 1));
	for (i = 0; i < to; i++) {
		buf[i] = name[i];
	}
	buf[i] = '\0';
	return buf;
}

char *DbFile_prune_song_dir(char *dir)
{
	int i;
	for (i = 0; i < 200; i++) {
		if (dir[i] == '\0')
			break;
	}
	char *buf = (char*)malloc(sizeof(char) * (i + 1));
	for (; i >= 0; i--) {
		buf[i] = dir[i];
	}
	return buf;
}

char DbFile_add_files(char *artist_folder, char *album_folder, char *ext, Vector *songs)
{
	char buf[200];
	sprintf(buf, ".\\%s\\%s\\*%s", artist_folder, album_folder, ext);
	WIN32_FIND_DATA file_data;
	HANDLE handle = FindFirstFile(buf, &file_data);
	if (handle == INVALID_HANDLE_VALUE)
		return 1;
	char *title = DbFile_prune_song_name(file_data.cFileName);
	sprintf(buf, ".\\%s\\%s\\%s", artist_folder, album_folder, file_data.cFileName);
	char *dir = DbFile_prune_song_dir(buf);
	Vector_push(songs, TitleDir_new(title, dir));
	while (FindNextFile(handle, &file_data)) {
		if(! (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			title = DbFile_prune_song_name(file_data.cFileName);
			sprintf(buf, ".\\%s\\%s\\%s", artist_folder, album_folder, file_data.cFileName);
			dir = DbFile_prune_song_dir(buf);
			Vector_push(songs, TitleDir_new(title, dir));
		}
	}
	return 0;
}

#define ext_explore(ext)(DbFile_add_files(artist_folder, album_folder, ext, vec))

char DbFile_explore(DbFile *self, Artist *artist, int album_index)
{
	char *artist_folder = artist->name;
	char *album_folder = ((TitleDir*)Vector_get(artist->albums, album_index))->dir;
	char buf[200];
	sprintf(buf, ".\\%s\\%s\\*.*", artist_folder, album_folder); 
	WIN32_FIND_DATA file_data;
	HANDLE handle = FindFirstFile(buf, &file_data);
	if (handle == INVALID_HANDLE_VALUE) {
		printf("Invalid handle `%s`\n", buf);
		return 1;
	}
	Vector *vec = (Vector*)Vector_get(artist->songs, album_index);
	if (ext_explore(".mp3") && ext_explore(".aiff") && ext_explore(".wav")) {
		printf("No files found `%s`\n", buf);
		return 1;
	}

	return 0;
}

char DbFile_load(DbFile *self)
{
	FILE *f = fopen(self->fname, "r");
	if (f == NULL)
		return 1;
	char op;
	Artist *last_artist = NULL;
	while ((op = fgetc(f)) != '*') {
		char tmp[100];
		char c;
		int i = 0;
		while ((c = fgetc(f)) != '\n') {
			tmp[i] = c;
			i++;
		}
		int to = i;
		char *buf = (char*)malloc(sizeof(char) * (to + 1));
		for (i = 0; i < to; i++) {
			buf[i] = tmp[i];
		}
		buf[i] = '\0';
		switch (op) {
		case '+':
			last_artist = Artist_new(buf);
			Vector_push(self->artists, last_artist);
			break;
		case '-':
			if (last_artist == NULL)
				return 3;
			Vector_push(last_artist->albums, TitleDir_new(buf, buf));
			Vector_push(last_artist->songs, Vector_new());
			break;
		case '_':
			Vector_push(last_artist->albums, TitleDir_new(buf, NULL));
			break;
		case '/':
			((TitleDir*)Vector_get(last_artist->albums, last_artist->albums->count - 1))->dir = buf;
			Vector_push(last_artist->songs, Vector_new());
			break;
		default:
			printf("Operator is %c\n", op);
			return 2;
		}
	}
	fclose(f);
	int i, j, n, m;
	for (i = 0, n = self->artists->count; i < n; i++) {
		Artist *artist = (Artist*)Vector_get(self->artists, i);
		for (j = 0, m = artist->albums->count; j < m; j++) {
			if (DbFile_explore(self, artist, j))
				return 4;
		}
	}
	return 0;
}
