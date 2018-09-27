#ifndef PLAYER_H
#define PLAYER_H


#define MAX_ITEMS   6

struct protagonist
{
    int x;
    int y;
    // current velocities
    int dx;
    int dy;
    int inventory[MAX_ITEMS]; // 0 -> no item, 1 -> have item, 2 -> used item
} player;

struct states
{
    int movingLeft;
    int movingRight;
    int movingUp;
    int movingDown;
    int jumping;
    int falling;
    int onGround;
    int talking;
} pstate;

#endif
