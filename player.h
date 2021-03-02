#ifndef PLAYER_H
#define PLAYER_H


#define MAX_ITEMS   6

extern struct protagonist
{
    float x;
    float y;
    float dx;     // current velocities
    float dy;
    int inventory[MAX_ITEMS]; // 0 -> no item, 1 -> have item, 2 -> used item
    int frame;
    int quest[4];   // current place in each quest for each part
} player;

extern struct states
{
    int movingLeft;
    int movingRight;
    int movingUp;
    int movingDown;
    int jumping;
    int falling;
    int talking;
    int facingLeft;
    int facingRight;
    int nograv;
} pstate;

#endif
