#ifndef VIDEO_H
#define VIDEO_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "map.h"

// Files
#define FILE_TILES  "tiles.png"
#define FILE_ITEMS  "items.png"
#define FILE_CHARS  "chars.png"
#define FILE_BG_OUTDOOR     "bg.png"
#define FILE_BG_INDOOR  "bg1.png"
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

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Surface* screen;
extern SDL_Texture* palette;
extern SDL_Texture* bgOutdoor;
extern SDL_Texture* bgIndoor;
extern SDL_Texture* items;
extern SDL_Texture* chars;
extern SDL_Texture* cursor;

extern TTF_Font* font;
extern SDL_Color black;
extern SDL_Color white;

extern SDL_Rect tile[MAX_TILES];
extern SDL_Rect item[MAX_ITEMS];
extern SDL_Rect character[MAX_TILES];

extern int entityanimframe;

void blitTile(SDL_Texture* image, SDL_Rect tileRect, int x, int y, int flip);
void drawBG(Level l);
void drawLevel(int array[MAP_HEIGHT][MAP_WIDTH]);
void drawEntities();
void drawPlayer(float x, float y);
void drawText(char *text, int x, int y, SDL_Color fg);
void drawInventory(int x, int y, int xinterval);
void fillRect(SDL_Rect rect, SDL_Color color, int border);
void textBox(char *text, char *text2);
void setupTile(SDL_Rect t[], int num);
void drawTextBox(int n);
void drawCursor(int x, int y);

#endif
