#include <SDL.h>
#include <math.h>
#include <stdlib.h>

#include "map.h"
#include "physics.h"
#include "player.h"
#include "video.h"


int checkCollision(float x, float y, int x1, int y1)
{
    int rx = round(x * TILE_WIDTH);
    int ry = round(y * TILE_HEIGHT);

    SDL_Rect A, B;
    A.x = rx;
    A.y = ry;
    A.h = TILE_HEIGHT;
    A.w = TILE_WIDTH;

    B.x = x1 * TILE_WIDTH;
    B.y = y1 * TILE_WIDTH;
    B.h = TILE_HEIGHT;
    B.w = TILE_WIDTH;

    return SDL_HasIntersection(&A, &B);
}

void gravity(float *x, float *y)
{
    *x += 0;
    *y += GRAVITY;

    pstate.falling = 1; // falling unless on ground

    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        for(int j = 0; j < MAP_WIDTH; j++)
        {
            if(checkCollision(*x, *y, j, i))
            {
                switch(lvl[i][j])
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 8:
                    case 9:
                    case 10:
                    case 56:
                    case 57:
                        if(*y < i) // coming from top
                        {
                            *y = (i - 1);
                            pstate.falling = 0;
                        }
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

void applyVelocity(float *x, float *y, float *dx, float *dy)
{
    // limit to terminal velocity
    //if ((*dx > 0) && (*dx > TERMINAL_VELOCITY)) *dx = TERMINAL_VELOCITY;
    //if ((*dy > 0) && (*dy > TERMINAL_VELOCITY)) *dy = TERMINAL_VELOCITY;

    int amt[4]; // 0 top, clockwise to 3 left
    int lowest;
    int side = -1; // side that collided

    *x += *dx;
    *y += *dy;

    int rx = round(*x * TILE_WIDTH);
    int ry = round(*y * TILE_HEIGHT);

    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        for(int j = 0; j < MAP_WIDTH; j++)
        {
            if(checkCollision(*x, *y, j, i))
            {
                switch (lvl[i][j])
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 8:
                    case 9:
                    case 10:
                    case 56:
                    case 57:
                        // find amount needed to move out of tile
                        amt[0] = abs(i - (ry+1)); // top
                        amt[1] = abs((j+1) - rx); // right
                        amt[2] = abs((i+1) - ry); // bottom
                        amt[3] = abs(j - (rx+1)); //left

                        // find lowest difference
                        lowest = amt[0];
                        for (int k = 0; k < 4; k++)
                        {
                            if (amt[k] < lowest)
                            {
                                lowest = amt[k];
                                side = k;
                            }
                        }

                        // handle collisions
                        if (*dx > 0) // player moving right
                        {
                            *x = (j - 1);
                            *dx = 0;
                        }
                        else if (*dx < 0) // player moving left
                        {
                            *x = (j + 1);
                            *dx = 0;
                        }

                        else if (*dy > 0) // player moving down
                        {
                            *y = (i - 1);
                            *dy = 0;
                        }
                        else if (*dy < 0) // player moving up
                        {
                            *y = (i + 1);
                            *dy = 0;
                        }

                        /*
                        if (1)
                        {
                            if(*x < j * TILE_WIDTH) // coming from the left
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
                            else if(*y > i * TILE_HEIGHT) // coming from below
                            {
                                *y = (i + 1) * TILE_HEIGHT;
                            }
                        }
                        else
                        {
                            if(*y < i * TILE_HEIGHT) // coming from top
                            {
                                *y = (i - 1) * TILE_HEIGHT;
                                pstate.falling = 0;
                            }
                            else if(*y > i * TILE_HEIGHT) // coming from below
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

                        }*/
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

