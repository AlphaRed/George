#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

#define FILE_TILES  "tiles.png"

#define FPS_LIMIT   60

#define SCREEN_WIDTH    672
#define SCREEN_HEIGHT   672

#define MAX_TILES   8
#define TILE_HEIGHT 32
#define TILE_WIDTH  32

// Physics
#define GRAVITY             9
#define MOVE_SPEED_LIMIT    8  // player movement limit
#define TERMINAL_VELOCITY   20
#define PLAYER_MOVE_ACCEL   2
#define PLAYER_JUMP_VEL     20
#define PLAYER_FRICTION_X   1
#define PLAYER_FRICTION_Y   1


SDL_Window* window = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* palette = NULL;
SDL_Surface* bg = NULL;

SDL_Rect tile[MAX_TILES];
int collides[MAX_TILES]; // which tiles colide and don't

int lvl[21][21];
int lvlent[21][21];
int nextlvl[21][21];

struct protagonist
{
    int x;
    int y;
    // current velocities
    int dx;
    int dy;

} player;
struct states
{
    int movingLeft;
    int movingRight;
    int jumping;
    int falling;
    int onGround;
} pstate;

int initSDL()
{
    int returnflag = 0;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
       printf("SDL failed to initialize: %s\n", SDL_GetError());
       returnflag = 1;
    }
    else
    {
        window = SDL_CreateWindow("Biggun", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL)
        {
            printf("Window failed to be created: %s\n", SDL_GetError());
            returnflag = 1;
        }
        else
        {
            screen = SDL_GetWindowSurface(window);

            if(IMG_Init(IMG_INIT_PNG) < 0)
            {
                printf("SDL_Image library failed to initialize: %s", IMG_GetError());
                returnflag = 1;
            }
        }
    }
    return returnflag;
}

void cleanup()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Surface* loadImage(char* filename, SDL_Surface* dest)
{
    SDL_Surface* optimized = NULL;
    SDL_Surface* loaded = IMG_Load(filename);
    if(!loaded)
        printf("Image failed to load: %s", IMG_GetError());
    optimized = SDL_ConvertSurface(loaded, dest->format, 0);
    SDL_FreeSurface(loaded);
    return optimized;
}

void blitImage(SDL_Surface* image, SDL_Surface* dest, int x, int y)
{
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;

    SDL_BlitSurface(image, NULL, dest, &destRect);
}

void blitTile(SDL_Surface* image, SDL_Rect tileRect, SDL_Surface* dest, int x, int y)
{
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;

    SDL_BlitSurface(image, &tileRect, dest, &destRect);
}

int checkEvents(SDL_Event eve)
{
    if(eve.type == SDL_QUIT)
        return 0;
    else if(eve.type == SDL_KEYDOWN)
    {
        switch(eve.key.keysym.sym)
        {
        case SDLK_w:
            if (pstate.falling != 1) // can't double jump
            {
                pstate.jumping = 10;
            }
            break;
        case SDLK_s:
            // zippo
            break;
        case SDLK_a:
            pstate.movingLeft = 1;
            pstate.movingRight = 0;
            break;
        case SDLK_d:
            pstate.movingRight = 1;
            pstate.movingLeft = 0;
            break;
        case SDLK_RETURN:
            return 0;
            break;
        default:
            break;
        }
    }
    else if (eve.type == SDL_KEYUP)
    {
        switch(eve.key.keysym.sym)
        {
        case SDLK_a:
            pstate.movingLeft = 0;
            break;
        case SDLK_d:
            pstate.movingRight = 0;
            break;
        default:
            break;
        }
    }

    return 1;
}

int loadLevel(char* filename, int array[21][21])
{
    int x, y;

    FILE* f = fopen(filename, "r");
    if(f != NULL)
    {
        for(y = 0; y < 21; y++)
        {
            for(x = 0; x < 21; x++)
            {
                fscanf(f, "%d", &array[y][x]);
            }
        }
        fclose(f);
    }

    return 0;
}

void drawLevel(int array[21][21])
{
    int x, y;

    for(y = 0; y < 15; y++)
    {
        for(x = 0; x < 21; x++)
        {
            blitTile(palette, tile[array[y][x]], screen, x * 32, y * 32);
        }
    }
}

void drawPlayer(int x, int y)
{
    blitTile(palette, tile[7], screen, x, y);
}

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
                if(lvl[i][j] == 3)
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

//    if(checkCollision(*x, *y, 64, 448))
//    {
//        *x -= 0;
//        *y -= GRAVITY;
//        //*y = 448 - TILE_HEIGHT;
//        pstate.falling = 0;
//    }
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
                if(lvl[i][j] == 3)
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
            }
        }
    }

//    if(checkCollision(*x, *y, 64, 448))
//    {
//        //*x -= *dx;
//        //*y -= *dy;
//        if(*dx > 0) // moving right
//            *x = 64 - TILE_WIDTH;
//        if(*dx < 0) // moving left
//            *x = 200 + 64;
//        if(*dy > 0) // moving down
//            *y = 448 - TILE_HEIGHT;
//        else // moving up
//            *y -= *dy;
//    }
}

int main(int argc, char* args[])
{
    unsigned int ticksLastFrame = 0;

    if(initSDL() > 0)
        printf("Failed to initialize\n");

    int quit = 1;
    SDL_Event eve;

    // setup tile source rectangles
    for (int i=0; i < MAX_TILES; i++)
    {
        tile[i].w = TILE_WIDTH;
        tile[i].h = TILE_HEIGHT;
    }
    tile[1].x = 0;
    tile[1].y = 0;
    collides[1] = 0;

    tile[2].x = 32;
    tile[2].y = 0;
    collides[1] = 1;

    tile[3].x = 64;
    tile[3].y = 0;
    collides[1] = 1;

    tile[4].x = 96;
    tile[4].y = 0;
    collides[1] = 1;

    tile[5].x = 128;
    tile[5].y = 0;
    collides[1] = 0;

    tile[6].x = 160;
    tile[6].y = 0;
    collides[1] = 0;

    tile[7].x = 192;
    tile[7].y = 0;
    collides[1] = 0;

    palette = loadImage(FILE_TILES, screen);
    bg = loadImage("bg.png", screen);
    loadLevel("level1.txt", lvl);
    loadLevel("level1ent.txt", lvlent);

    player.x = 5;
    player.y = 5;
    player.dx = 0;
    player.dy = 0;

    pstate.movingLeft = 0;
    pstate.movingRight = 0;
    pstate.falling = 0;
    pstate.jumping = 0;

    //SDL_FlushEvent(SDL_USEREVENT);

    // Game Loop
    while(quit)
    {
        // Events
        SDL_PollEvent(&eve);
        quit = checkEvents(eve);

        // Physics
        gravity(&player.x, &player.y);
        //applyVelocity(&player.x, &player.y, 0, 9);

        if (pstate.movingLeft && player.dx > -MOVE_SPEED_LIMIT)
            player.dx -= PLAYER_MOVE_ACCEL;
        if (pstate.movingRight && player.dx < MOVE_SPEED_LIMIT)
            player.dx += PLAYER_MOVE_ACCEL;
        if (pstate.jumping)
        {
            player.dy = -PLAYER_JUMP_VEL;
            pstate.jumping--; // state is also a counter
            if (pstate.jumping == 0) pstate.falling = 1; // fall when done jumping
        }

        applyVelocity(&player.x, &player.y, player.dx, player.dy);

        // slow down player
        if (player.dx > 0) player.dx -= PLAYER_FRICTION_X;
        if (player.dx < 0) player.dx += PLAYER_FRICTION_X;
        if (player.dy > 0) player.dy -= PLAYER_FRICTION_Y;
        if (player.dy < 0) player.dy += PLAYER_FRICTION_Y;

        //Outer bounds
        if(player.x < 0)
            player.x = 0;
        if(player.x > 640)
            player.x = 640;
        if(player.y < 0)
            player.y = 0;
        if(player.y > 448)
        {
            player.y = 448;
            pstate.falling = 0;
        }

        // Check level entities

        if(lvlent[player.y / TILE_HEIGHT][player.x / TILE_WIDTH] == 1)
        {
            // Next level!
            printf("Next Level!\n");
            loadLevel("level2.txt", lvl);
            player.x = 0;
            player.y = 0;
        }

        // Draw
        drawLevel(lvl);
        drawPlayer(player.x, player.y);

        // Update
        while (SDL_GetTicks() < (1000/FPS_LIMIT + ticksLastFrame))
            SDL_Delay(1);
        SDL_UpdateWindowSurface(window);
        ticksLastFrame = SDL_GetTicks();
    }


    cleanup();

    return 0;
}
