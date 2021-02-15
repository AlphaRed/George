#ifndef PHYSICS_H
#define PHYSICS_H

// Physics
#define GRAVITY             0.28
#define MOVE_SPEED_LIMIT    0.08  // player movement limit
#define TERMINAL_VELOCITY   1
#define PLAYER_MOVE_ACCEL   0.03
#define PLAYER_JUMP_VEL     0.45
#define PLAYER_JUMP_LIMIT   15
#define PLAYER_FRICTION_X   0.025
#define PLAYER_FRICTION_Y   0.015

int nograv;

int checkCollision(float x, float y, int x1, int y1);
void gravity(float *x, float *y);
void applyVelocity(float *x, float *y, float dx, float dy);

#endif
