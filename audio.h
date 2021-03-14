#ifndef AUDIO_H
#define AUDIO_H

extern int musicenabled;

#define SOUND_JUMP      "sounds/jump.wav"
#define SOUND_ITEMGET   "sounds/item.wav"

extern Mix_Chunk* sndjump;
extern Mix_Chunk* snditemget;

#define MUSIC_FADE       200
#define MUSIC_1         "sounds/song.mp3"

extern Mix_Music* musmusic;

#endif // AUDIO_H
