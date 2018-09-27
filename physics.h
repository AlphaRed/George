#ifndef PHYSICS_H
#define PHYSICS_H

// Physics
#define GRAVITY             9
#define MOVE_SPEED_LIMIT    4  // player movement limit
#define TERMINAL_VELOCITY   25
#define PLAYER_MOVE_ACCEL   2
#define PLAYER_JUMP_VEL     15
#define PLAYER_JUMP_LIMIT   10
#define PLAYER_FRICTION_X   1
#define PLAYER_FRICTION_Y   1

int nograv;

int checkCollision(int x, int y, int x1, int y1);
void gravity(int *x, int *y);
void applyVelocity(int *x, int *y, int dx, int dy);

#endif
