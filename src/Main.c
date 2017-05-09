#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include "SongPlayer.h"
#include "DbFile.h"
#include "Queue.h"

#define RI_HARDQUIT 555
#define RI_QUIT -1
#define RI_PLAY -2
#define RI_ADD -3
#define RI_PAUSE -4
#define RI_STOP -5

int read_console_int(HANDLE handle)
{
	INPUT_RECORD record;
	DWORD num_read;
	int res = 0;
	while (ReadConsoleInput(handle, &record, 1, &num_read)) {
		if (! record.Event.KeyEvent.bKeyDown)
			continue;
		WORD w = record.Event.KeyEvent.wVirtualKeyCode;
		switch (w) {
		case '\t':
		case ' ':
		case '\r':
			return res;
		case 'Q':
			return RI_QUIT;
		case 'R':
			return RI_PLAY;
		case 'W':
			return RI_ADD;
		case 'P':
			return RI_PAUSE;
		case 'O':
			return RI_STOP;
		case 'T':
		case VK_OEM_MINUS:
			res = 0;
			putchar('-');
			break;
		default:
			if (w >= '0' && w <= '9') {
				putchar(w);
				res *= 10;
				res += w - '0';
			}
		}
	}
	return 0;
}

#define cls()(FillConsoleOutputCharacter(console_out, ' ', console_info_size, zero, &blank))
#define zero_cursor()(SetConsoleCursorPosition(console_out, zero))
#define lf()(putchar('\n'));
#define read_int()(read_console_int(console_in))
#define key_down(k)(GetKeyState(k) & 0x8000)

Queue *Playlist;
SongPlayer *Song_Player;
char Run_Thread;
HANDLE Background_Lockette;

inline void next_track()
{
	if (Playlist->count > 0) {
		if (SongPlayer_load(Song_Player, (TitleDir*)Queue_pop(Playlist)))
			SongPlayer_play(Song_Player);
	}
}

inline void on_key_down(int key)
{
	switch (key) {
	case VK_MEDIA_PREV_TRACK:
	case VK_MEDIA_STOP:
		SongPlayer_stop(Song_Player);
		break;
	case VK_MEDIA_PLAY_PAUSE:
		SongPlayer_toggle_pause(Song_Player);
		break;
	case VK_MEDIA_NEXT_TRACK:
		next_track();
		break;
	}
}

void BackgroundProc(void* ptr)
{
	WaitForSingleObject(Background_Lockette, INFINITE);
	int chk_key[4] = { VK_MEDIA_STOP, VK_MEDIA_PLAY_PAUSE, VK_MEDIA_NEXT_TRACK, VK_MEDIA_PREV_TRACK };
	char chk_down[4] = { 0 };
	while (Run_Thread) {
		SongPlayer_lock(Song_Player);
		int i;
		for (i = 0; i < 4; i++) {
			int key = chk_key[i];
			if (key_down(key)) {
				if (! chk_down[i]) {
					on_key_down(key);
				}
				chk_down[i] = 1;
			} else {
				chk_down[i] = 0;
			}
		}
		if (SongPlayer_song_ended(Song_Player))
			next_track();
		SongPlayer_unlock(Song_Player);
		Sleep(20);
	}
	ReleaseMutex(Background_Lockette);
}

int main(int argc, char **argv)
{
	HANDLE console_out = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE console_in = GetStdHandle(STD_INPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO console_info;
	if (! GetConsoleScreenBufferInfo(console_out, &console_info)) {
		puts("You are not in a console");
		return 1;
	}
	DWORD console_info_size = console_info.dwSize.X * console_info.dwSize.Y;
	Song_Player = SongPlayer_new();
	if (Song_Player == NULL) {
		puts("bass.dll could not be initialized");
		return 1;
	}
	DbFile *db = DbFile_new("song_lists.txt");
	int result = DbFile_load(db);
	if (result) {
		printf("DB file missing or invalid (err = %d)\n", result);
		return result;
	}
	DWORD blank;
	COORD zero;
	zero.X = 0;
	zero.Y = 0;
	int menu = 1;
	Artist *sel_artist = NULL;
	int sel_album;
	char *sel_album_title;
	TitleDir *sel_song;
	Playlist = Queue_new();
	Run_Thread = 1;
	Background_Lockette = CreateMutex(NULL, FALSE, NULL);
	if (Background_Lockette == NULL) {
		puts("Could not create mutex");
		return 1;
	}
	DWORD thread_id;
	HANDLE background_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BackgroundProc, NULL, 0, &thread_id);
	if (background_thread == NULL) {
		puts("Could not make thread");
		return 1;
	}
	while (1) {
		cls();
		zero_cursor();
		lf();
		switch (menu) {
		case 0:
			Run_Thread = 0;
			WaitForSingleObject(Background_Lockette, INFINITE);
			CloseHandle(background_thread);
			CloseHandle(console_in);
			CloseHandle(console_out);
			return 0;
		case 1: {
			puts("Now Playing");
			if (Song_Player->loaded_song)
				printf("   %s\n", Song_Player->loaded_song->title);
			else
				puts("   NOTHING");
			printf("Playlist (%d songs)\n", Playlist->count);
			Node *i;
			for (i = Playlist->head; i != NULL; i = i->next) {
				printf("   %s\n", ((TitleDir*)i->value)->title);
			}
			zero_cursor();
			int k = read_int();
			SongPlayer_lock(Song_Player);
			switch (k) {
			case RI_HARDQUIT:
				menu --;
				break;
			case RI_QUIT:
				break;
			case RI_PLAY:
				next_track();
				break;
			case 0:
			case RI_ADD:
				menu ++;
				break;
			case RI_PAUSE:
				SongPlayer_toggle_pause(Song_Player);
				break;
			case RI_STOP:
				SongPlayer_stop(Song_Player);
				break;
			}
			SongPlayer_unlock(Song_Player);
			break;
		}
		case 2: {
			int i;
			int n;
			for (i = 0, n = db->artists->count; i < n; i++) {
				printf("%2d] %s\n", i, ((Artist*)Vector_get(db->artists, i))->name);
			}
			zero_cursor();
			int k = read_int();
			SongPlayer_lock(Song_Player);
			switch (k) {
			case RI_QUIT:
				menu --;
				break;
			case RI_PAUSE:
				SongPlayer_toggle_pause(Song_Player);
				break;
			case RI_STOP:
				SongPlayer_stop(Song_Player);
				break;
			default:
				if (k >= 0 && k < db->artists->count) {
					sel_artist = (Artist*)Vector_get(db->artists, k);
					menu ++;
				}	
			}
			SongPlayer_unlock(Song_Player);
			break;
		}
		case 3: {
			puts(sel_artist->name);
			lf();
			int i;
			int n;
			for (i = 0, n = sel_artist->albums->count; i < n; i++) {
				printf("%2d] %s\n", i, ((TitleDir*)Vector_get(sel_artist->albums, i))->title);
			}
			zero_cursor();
			int k = read_int();
			SongPlayer_lock(Song_Player);
			switch (k) {
			case RI_QUIT:
				menu --;
				break;
			case RI_PAUSE:
				SongPlayer_toggle_pause(Song_Player);
				break;
			case RI_STOP:
				SongPlayer_stop(Song_Player);
				break;
			default:
				if (k >= 0 && k < n) {
					sel_album = k;
					sel_album_title = ((TitleDir*)Vector_get(sel_artist->albums, sel_album))->title;
					menu ++;
				}
			}
			SongPlayer_unlock(Song_Player);
			break;
		}
		case 4: {
			puts(sel_album_title);
			int i;
			int n;
			Vector* songs = (Vector*)Vector_get(sel_artist->songs, sel_album);
			for (i = 0, n = songs->count; i < n; i++) {
				printf("%2d] %s\n", i, ((TitleDir*)Vector_get(songs, i))->title);
			}
			zero_cursor();
			int k = read_int();
			SongPlayer_lock(Song_Player);
			switch (k) {
			case RI_PLAY:
				Queue_pop_all(Playlist);
			case RI_ADD:
				Queue_push_all(Playlist, songs);
			case RI_QUIT:
				menu --;
				break;
			case RI_PAUSE:
				SongPlayer_toggle_pause(Song_Player);
				break;
			case RI_STOP:
				SongPlayer_stop(Song_Player);
				break;
			default:
				if (k >= 0 && k < n) {
					sel_song = (TitleDir*)Vector_get(songs, k);
					menu ++;
				}
			}
			SongPlayer_unlock(Song_Player);
			break;
		}
		case 5: {
			puts(sel_artist->name);
			puts(sel_album_title);
			puts(sel_song->title);
			zero_cursor();
			int k = read_int();
			SongPlayer_lock(Song_Player);
			switch (k) {
			case RI_PLAY:
				Queue_pop_all(Playlist);
			case RI_ADD:
				Queue_push(Playlist, sel_song);
			case RI_QUIT:
				menu --;
				break;
			case RI_PAUSE:
				SongPlayer_toggle_pause(Song_Player);
				break;
			case RI_STOP:
				SongPlayer_stop(Song_Player);
				break;
			}
			SongPlayer_unlock(Song_Player);
			break;
		}
		default:
			printf("Unknown menu state %d\n", menu);
			return 1;
		}
	}
}
