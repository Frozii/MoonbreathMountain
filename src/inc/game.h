#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <types.h>
#include <update.h>
#include <render.h>
#include <text_render.h>
#include <item.h>
#include <level_generation.h>
#include <interface.h>
#include <player.h>
#include <slime.h>

// large
// #define WINDOW_WIDTH 1216
// #define WINDOW_HEIGHT 960

// standard
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

// small
// #define WINDOW_WIDTH 768
// #define WINDOW_HEIGHT 640

#define CONSOLE_WIDTH WINDOW_WIDTH
#define CONSOLE_HEIGHT 160

#define TEXTURE_COUNT 10

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern int game_is_running;
extern int turn_changed;
extern SDL_Keycode key_pressed;
extern SDL_Rect camera;
extern uint32 time_elapsed;
extern SDL_Texture *textures[TEXTURE_COUNT];
extern uint8 level[LEVEL_WIDTH_IN_TILES * LEVEL_HEIGHT_IN_TILES];

int32 game_init();
void game_run();
void game_exit();

#endif // GAME_H