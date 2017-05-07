#include "SongPlayer.h"
#include "TitleDir.h"
#include <stdio.h>
#include <windows.h>

HANDLE SongPlayer_Lockette = NULL;

#define PLAYING_STOPPED 0
#define PLAYING_PLAYING 1
#define PLAYING_PAUSED 2

#define lock()(WaitForSingleObject(SongPlayer_Lockette, INFINITE))
#define unlock()(ReleaseMutex(SongPlayer_Lockette))

SongPlayer *SongPlayer_new(void)
{
	if (! BASS_Init(-1, 44100, 0, 0, NULL))
		return NULL;
	SongPlayer_Lockette = CreateMutex(NULL, FALSE, NULL);
	if (SongPlayer_Lockette == NULL)
		return NULL;
	SongPlayer *buf = (SongPlayer*)malloc(sizeof(SongPlayer));
	buf->stream = 0;
	buf->playing = PLAYING_STOPPED;
	buf->loaded_song = NULL;
	return buf;
}

char SongPlayer_load(SongPlayer *self, TitleDir *song)
{
	lock();
	SongPlayer_stop(self);
	self->stream = BASS_StreamCreateFile(0, song->dir, 0, 0, 0);
	self->loaded_song = song;
	char ret = self->stream != 0;
	unlock();
	return ret;
}

void SongPlayer_play(SongPlayer *self)
{
	lock();
	if (self->stream) {
		self->playing = PLAYING_PLAYING;
		BASS_ChannelPlay(self->stream, 0);
	}
	unlock();
}

void SongPlayer_pause(SongPlayer *self)
{
	lock();
	if (self->stream && self->playing != PLAYING_PAUSED) {
		self->playing = PLAYING_PAUSED;
		BASS_ChannelPause(self->stream);
	}
	unlock();
}

void SongPlayer_stop(SongPlayer *self)
{
	lock();
	if (self->stream && self->playing != PLAYING_STOPPED) {
		self->playing = PLAYING_STOPPED;
		BASS_StreamFree(self->stream);
		self->stream = 0;
		self->loaded_song = NULL;
	}
	unlock();
}

void SongPlayer_toggle_pause(SongPlayer *self)
{
	lock();
	if (self->stream) {
		if (self->playing == PLAYING_PAUSED)
			SongPlayer_play(self);
		else
			SongPlayer_pause(self);
	}
	unlock();
}

char SongPlayer_song_ended(SongPlayer* self)
{
	if (self->stream) {
		return BASS_ChannelGetPosition(self->stream, BASS_POS_BYTE) == BASS_ChannelGetLength(self->stream, BASS_POS_BYTE);
	} else {
		return 0;
	}
}
