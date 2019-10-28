#define array_count(array) (sizeof(array) / sizeof((array)[0]))

internal v2s
V2s(s32 a, s32 b)
{
    v2s result = {a, b};
    return(result);
}

internal v2u
V2u(u32 a, u32 b)
{
    v2u result = {a, b};
    return(result);
}

internal v2f
V2f(f32 a, f32 b)
{
    v2f result = {a, b};
    return(result);
}

internal b32
V2u_equal(v2u a, v2u b)
{
    b32 result = (a.x == b.x && a.y == b.y);
    return(result);
}

internal v4s
V4s(s32 a, s32 b, s32 c, s32 d)
{
    v4s result = {a, b, c, d};
    return(result);
}

internal v4u
V4u(u32 a, u32 b, u32 c, u32 d)
{
    v4u result = {a, b, c, d};
    return(result);
}

internal v4f
V4f(f32 a, f32 b, f32 c, f32 d)
{
    v4f result = {a, b, c, d};
    return(result);
}

internal file_t
read_file_contents(char *path)
{
    file_t result = {0};
    
    FILE *file = fopen(path, "rb");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        result.size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        result.contents = malloc(result.size + 1);
        u32 ret = fread(result.contents, result.size, 1, file);
        if(ret != 1)
        {
            result.size = 0;
            free(result.contents);
            result.contents = 0;
        }
        
        fclose(file);
        result.contents[result.size] = 0;
    }
    else
    {
        printf("ERROR: Cannot open file %s\n", path);
    }
    
    return(result);
}

internal u32
tile_dist(v2u a, v2u b)
{
    u32 result = abs(a.x - b.x) + abs(a.y - b.y);
    return(result);
}

internal u32
tile_div(u32 value)
{
    u32 result = value / 32;
    return(result);
}

internal u32
tile_mul(u32 value)
{
    u32 result = value * 32;
    return(result);
}

internal v2u
get_game_position(v2u pos)
{
    v2u result = {0};
    result.x = tile_mul(pos.x) - game.camera.x;
    result.y = tile_mul(pos.y) - game.camera.y;
    return(result);
}

internal u32
get_window_refresh_rate()
{
    u32 refresh_rate = 60;
    
    u32 display_index = SDL_GetWindowDisplayIndex(game.window);
    SDL_DisplayMode mode = {0};
    if(!SDL_GetDesktopDisplayMode(display_index, &mode))
    {
        refresh_rate = mode.refresh_rate;
    }
    
    if(mode.refresh_rate == 0)
    {
        refresh_rate = 60;
    }
    
    return(refresh_rate);
}

internal f32
get_seconds_elapsed(u64 old_counter, u64 new_counter, f32 perf_count_frequency)
{
    f32 result = (f32)(new_counter - old_counter) / perf_count_frequency;
    return(result);
}

internal texture_t
load_texture(char *path, v4u *color_key)
{
    texture_t result = {0};
    
    SDL_Surface *loaded_surf = IMG_Load(path);
    if(loaded_surf)
    {
        result.w = loaded_surf->w;
        result.h = loaded_surf->h;
        
        if(color_key)
        {
            // NOTE(rami):
            // Store the rgb color into formatted_key in the color format of the surface.
            // All pixels with the color of formatted_key will be transparent.
            u32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, color_key->g, color_key->b);
            SDL_SetColorKey(loaded_surf, 1, formatted_key);
        }
        
        SDL_Texture *new_tex = SDL_CreateTextureFromSurface(game.renderer, loaded_surf);
        if(new_tex)
        {
            result.tex = new_tex;
        }
        else
        {
            printf("ERROR: SDL could not create a texture from surface: %s\n", SDL_GetError());
        }
    }
    else
    {
        printf("ERROR: SDL could not load image %s: %s\n", path, IMG_GetError());
    }
    
    SDL_FreeSurface(loaded_surf);
    return(result);
}

internal s32
rand_num(s32 min, s32 max)
{
    if(min > max)
    {
        s32 temp = max;
        max = min;
        min = temp;
    }
    
    s32 result = min + rand() % (max - min + 1);
    return(result);
}

internal b32
str_equal(char *a, char *b)
{
    b32 result = false;
    
    while(*a && *b && *a++ == *b++)
    {
        if(*a == '\0' && *b == '\0')
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal f32
distance(u32 ax, u32 ay, u32 bx, u32 by)
{
    f32 result = sqrt(pow(bx - ax, 2) + pow(by - ay, 2));
    return(result);
}


internal f32
slope(f32 ax, f32 ay, f32 bx, f32 by)
{
    f32 result = (ax - bx) / (ay - by);
    return(result);
}

internal v2u
center(v4u rect)
{
    v2u result = {0};
    result.x = (rect.x + rect.w / 2);
    result.y = (rect.y + rect.h / 2);
    return(result);
}

internal b32
update_sprite(sprite_t *sprite, entity_state state)
{
    b32 did_sprite_die = true;
    u32 time_elapsed = SDL_GetTicks();
    
    if(state == state_idle)
    {
        if(time_elapsed > sprite->idle_frame_last_changed + sprite->idle_frame_duration)
        {
            if(sprite->current_frame.x < sprite->idle_start_frame.x + sprite->idle_frame_count)
            {
                ++sprite->current_frame.x;
            }
            else
            {
                sprite->current_frame = sprite->idle_start_frame;
            }
            
            sprite->idle_frame_last_changed = time_elapsed;
        }
    }
    else if(state == state_dead)
    {
        if(time_elapsed > sprite->dead_frame_last_changed + sprite->dead_frame_duration)
        {
            if(sprite->current_frame.x < sprite->dead_start_frame.x + sprite->dead_frame_count)
            {
                ++sprite->current_frame.x;
            }
            else
            {
                sprite->current_frame = sprite->dead_start_frame;
                did_sprite_die = false;
            }
            
            sprite->dead_frame_last_changed = time_elapsed;
        }
    }
    
    return(did_sprite_die);
}

internal u32
cmp_num(u32 a, u32 b)
{
    u32 result = 0;
    
    if(a > b)
    {
        result = 1;
    }
    else if(a < b)
    {
        result = 2;
    }
    
    return(result);
}

internal u32
round_f32_to_u32(f32 value)
{
    u32 result = (u32)(value + 0.5f);
    return(result);
}

internal v4u
f32_to_u32_color(v4f color)
{
    v4u result =
    {
        round_f32_to_u32(color.r * 255.0f),
        round_f32_to_u32(color.g * 255.0f),
        round_f32_to_u32(color.b * 255.0f),
        round_f32_to_u32(color.a * 255.0f)
    };
    
    return(result);
}

internal u32
get_ratio(f32 min, f32 max, f32 width)
{
    u32 result = (u32)((min / max) * width);
    return(result);
}

internal void
set_render_color(v4f color)
{
    v4u render_color = f32_to_u32_color(color);
    SDL_SetRenderDrawColor(game.renderer,
                           render_color.r,
                           render_color.g, 
                           render_color.b,
                           render_color.a);
}