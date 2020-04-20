#ifndef VIDEO_H
#define VIDEO_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "map.h"

// Files
#define FILE_TILES  "tiles.png"
#define FILE_ITEMS  "items.png"
#define FILE_CHARS  "chars.png"
#define FILE_BG     "bg.png"
#define FILE_FONT   "dpcomic.ttf"

#define FPS_LIMIT   60

// Dimensions
#define TILE_HEIGHT     16
#define TILE_WIDTH      TILE_HEIGHT
#define SCREEN_SCALE    3
#define SCREEN_WIDTH    MAP_WIDTH * TILE_WIDTH * SCREEN_SCALE
#define SCREEN_HEIGHT   MAP_HEIGHT * TILE_HEIGHT * SCREEN_SCALE
#define FONT_SIZE   16*SCREEN_SCALE

// Limits
#define MAX_TILES       256
#define MAX_CHARS   MAX_TILES

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* screen;
SDL_Texture* palette;
SDL_Texture* bg;
SDL_Texture* items;
SDL_Texture* chars;

TTF_Font* font;
SDL_Color black;
SDL_Color white;

SDL_Rect tile[MAX_TILES];
SDL_Rect item[MAX_ITEMS];
SDL_Rect character[MAX_TILES];

int entityanimframe;

void blitTile(SDL_Texture* image, SDL_Rect tileRect, int x, int y, int flip);

void drawLevel(int array[MAP_HEIGHT][MAP_WIDTH]);
void drawEntities();
void drawPlayer(float x, float y);
void drawText(char *text, int x, int y, SDL_Color fg);
void drawInventory(int x, int y, int xinterval);
void textBox(char *text, char *text2);

#endif
