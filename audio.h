#ifndef AUDIO_H
#define AUDIO_H


#define SOUND_JUMP      "jump.wav"
#define SOUND_ITEMGET   "item.wav"

extern Mix_Chunk* sndjump;
extern Mix_Chunk* snditemget;

#define MUSIC_FADE       100
#define MUSIC_1         "song.mp3"

extern Mix_Music* musmusic;

#endif // AUDIO_H
