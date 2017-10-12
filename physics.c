#include <SDL.h>

#include "map.h"
#include "physics.h"
#include "player.h"
#include "video.h"


int checkCollision(int x, int y, int x1, int y1)
{
    SDL_Rect A, B;
    A.x = x;
    A.y = y;
    A.h = TILE_HEIGHT;
    A.w = TILE_WIDTH;

    B.x = x1;
    B.y = y1;
    B.h = TILE_HEIGHT;
    B.w = TILE_WIDTH;

    return SDL_HasIntersection(&A, &B);
}

void gravity(int *x, int *y)
{
    *x += 0;
    *y += GRAVITY;

    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 21; j++)
        {
            if(checkCollision(*x, *y, j * TILE_WIDTH, i * TILE_HEIGHT))
            {
                if(lvl[i][j] == 1 || lvl[i][j] == 2 || lvl[i][j] == 3)
                {
                    if(*y < i * TILE_HEIGHT) // coming from top
                    {
                        *y = (i - 1) * TILE_HEIGHT;
                        pstate.falling = 0;
                    }
                }
            }
        }
    }
}

void applyVelocity(int *x, int *y, int dx, int dy)
{
    // limit to terminal velocity
    //if ((*dx > 0) && (*dx > TERMINAL_VELOCITY)) *dx = TERMINAL_VELOCITY;
    //if ((*dy > 0) && (*dy > TERMINAL_VELOCITY)) *dy = TERMINAL_VELOCITY;

    *x += dx;
    *y += dy;

    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 21; j++)
        {
            if(checkCollision(*x, *y, j * TILE_WIDTH, i * TILE_HEIGHT))
            {
                if(lvl[i][j] == 1 || lvl[i][j] == 2 || lvl[i][j] == 3)
                {
                    if(*y > i * TILE_HEIGHT) // coming from below, works better if check this first
                    {
                        *y = (i + 1) * TILE_HEIGHT;
                    }
                    else if(*x < j * TILE_WIDTH) // coming from the left
                    {
                        *x = (j - 1) * TILE_WIDTH;
                    }
                    else if(*x > j * TILE_WIDTH) // coming from the right
                    {
                        *x = (j + 1) * TILE_WIDTH;
                    }
                    else if(*y < i * TILE_HEIGHT) // coming from top
                    {
                        *y = (i - 1) * TILE_HEIGHT;
                        pstate.falling = 0;
                    }
                }
            }
        }
    }
}

