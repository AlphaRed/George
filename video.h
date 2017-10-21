#ifndef VIDEO_H
#define VIDEO_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "map.h"


#define FILE_TILES  "tiles.png"
#define FILE_FONT   "FSEX300.ttf"

#define FPS_LIMIT   60

#define SCREEN_WIDTH    672
#define SCREEN_HEIGHT   480

#define MAX_TILES   100
#define TILE_HEIGHT 32
#define TILE_WIDTH  32

#define FONT_SIZE   16

SDL_Window* window;
SDL_Surface* screen;
SDL_Surface* palette;
SDL_Surface* bg;
SDL_Surface* items;

TTF_Font* font;
SDL_Color black;

SDL_Rect tile[MAX_TILES];

SDL_Surface* loadImage(char* filename, SDL_Surface* dest);

void blitImage(SDL_Surface* image, SDL_Surface* dest, int x, int y);
void blitTile(SDL_Surface* image, SDL_Rect tileRect, SDL_Surface* dest, int x, int y);

void drawLevel(int array[MAP_HEIGHT][MAP_WIDTH]);
void drawEntities();
void drawPlayer(int x, int y);
void drawText(char *text, int x, int y, SDL_Color fg);

#endif
