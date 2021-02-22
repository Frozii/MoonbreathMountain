typedef enum
{
    ExamineFlags_Open = (1 << 1),
    ExamineFlags_ReadyForKeypress = (1 << 2),
} ExamineFlags;

typedef enum
{
    ExamineType_None,
    
    ExamineType_Item,
    ExamineType_Entity,
    ExamineType_EntitySpell,
    ExamineType_Tile
} ExamineType;

typedef struct
{
    u32 flags;
    
    b32 key_pressed[GameKey_Count];
    u32 key_pressed_start[GameKey_Count];
    u32 key_hold_duration;
    v2u pos;
    
    ExamineType type;
    Item *item;
    Entity *entity;
    Spell *spell;
    TileID tile_id;
    
    // For dungeon passages
    u32 passage_index;
} Examine;

typedef struct
{
    char c;
    char *str;
} PrintableKey;

typedef struct
{
    b32 is_initialized;
    b32 show_item_ground_outline;
    
    GameMode mode;
    Random random;
    Examine examine;
    v4s camera;
    f32 time;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Key keybinds[GameKey_Count];
    
    b32 should_update;
    f32 action_count;
    } Game;

typedef struct
{
    b32 set;
    u32 used;
    u32 size;
    void *storage; // Required to be initialized to zero.
} GameMemory;

internal void render_draw_rect(Game *game, v4u rect, Color color);
internal void render_fill_rect(Game *game, v4u rect, Color color);
internal void render_window(Game *game, v4u rect, u32 border_size);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest, b32 is_flipped);
internal char get_pressed_keyboard_char(Input *input);
internal char get_pressed_alphabet_char(Input *input);
internal b32 was_pressed(InputState *state);
internal v4u get_tile_rect(v2u tile);
internal v4u get_game_dest(Game *game, v2u pos);
internal v2u get_direction_pos(v2u pos, Direction direction);
internal PrintableKey get_printable_key(Input *input, Key key);
internal Direction get_random_direction(Random *random);
internal Direction get_direction_moved_from(v2u old_pos, v2u new_pos);