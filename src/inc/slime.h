#ifndef SLIME_H
#define SLIME_H

#include <game.h>

#define SLIME_COUNT 2

extern slime_t slimes[SLIME_COUNT];

void create_slimes(int tile, int x, int y, int w, int h, int hp);
void delete_slimes(int i);
void update_slimes(char *level);
void render_slimes(SDL_Rect *camera);

#endif // SLIME_H