#ifndef SONGPLAYER_H
#define SONGPLAYER_H

#include "bass.h"
#include "TitleDir.h"

typedef struct {
	HSTREAM stream;
	char playing;
	TitleDir *loaded_song;
	HANDLE lockette;
} SongPlayer;

SongPlayer *SongPlayer_new(void);
char SongPlayer_load(SongPlayer*, TitleDir*);
void SongPlayer_play(SongPlayer*);
void SongPlayer_pause(SongPlayer*);
void SongPlayer_stop(SongPlayer*);
void SongPlayer_toggle_pause(SongPlayer*);
void SongPlayer_restart(SongPlayer*);
char SongPlayer_song_ended(SongPlayer*);
void SongPlayer_lock(SongPlayer*);
void SongPlayer_unlock(SongPlayer*);

#endif
