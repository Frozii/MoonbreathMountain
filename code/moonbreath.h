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
    b32 is_open;
    b32 is_ready_for_pressed_letter;
    
    b32 is_key_pressed[GameKey_Count];
    u32 key_pressed_start[GameKey_Count];
    v2u pos;
    
    ExamineType type;
    Item *item;
    Entity *entity;
    Spell *spell;
    TileID tile_id;
    
    // For dungeon passages.
    b32 start_up_passages_from_first;
    u32 up_passage_index;
    
    b32 start_down_passages_from_first;
    u32 down_passage_index;
} Examine;

typedef struct
{
    b32 is_initialized;
    b32 show_item_ground_outline;
    
    Random random;
    GameMode mode;
    Examine examine;
    v4s camera;
    f32 time;
    
    v2u window_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    u32 alphabet[AlphabetKey_Count];
    u32 keybinds[GameKey_Count];
} Game;

internal Direction get_random_direction(Random *random);
internal v2u get_direction_pos(v2u pos, Direction direction);
internal v4u get_tile_rect(v2u tile);
internal v4u get_game_dest(Game *game, v2u pos);
internal b32 is_window_1920x1080(v2u window_size);
internal b32 is_window_1280x720(v2u window_size);
internal b32 was_pressed(InputState *state);
internal void render_texture_half_color(SDL_Renderer *renderer, SDL_Texture *texture, v4u src, v4u dest, b32 is_flipped);
internal void render_draw_rect(Game *game, v4u rect, Color color);
internal void render_fill_rect(Game *game, v4u rect, Color color);