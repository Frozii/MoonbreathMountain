#include "types.h"
#include "util.c"
#include "level_gen.c"
#include "fov.c"
#include "render.c"
#include "ui.c"
#include "pathfind.c"
// TODO(rami): Work on conf when we need it again
// #include "conf.c"
#include "assets.c"
#include "pop_up_text.c"
#include "monster.c"
#include "item.c"
#include "player.c"

/*
Compression oriented programming:
  Make it work, can you clean it/simplify it/make it more robust?
   What can you pull out as reusable?
*/

/*
- When anyone tries to move anywhere, they have to go through the player, the monsters,
the NPC's positions to make sure they can move there, that's pretty expensive.
Instead it would be nice to just check if the position you want to move to is occupied
or not, if it's not occupied you can move, otherwise you can't.

- Create corridors between rooms.
- Create more room types.
*/

internal void
resize_window(u32 w, u32 h)
{
    SDL_SetWindowSize(game.window, w, h);
    game.window_size = V2u(w, h);
    game.console_size.w = game.window_size.w;
    game.camera = V4i(0, 0, game.window_size.w, game.window_size.h - game.console_size.h);
}

internal void
toggle_fullscreen()
{
    u32 flags = SDL_GetWindowFlags(game.window);
    if(flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
    {
        SDL_SetWindowFullscreen(game.window, 0);
        resize_window(1280, 720);
        SDL_SetWindowPosition(game.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    else
    {
        SDL_SetWindowFullscreen(game.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        
        v2u window_size = {0};
        SDL_GetWindowSize(game.window, (i32 *)&window_size.x, (i32 *)&window_size.y);
        resize_window(window_size.x, window_size.y);
    }
}

internal void
update_camera()
{
    // TODO(rami): Debug
#if 0
    printf("camera.x1: %i\n", game.camera.x);
    printf("camera.y1: %i\n", game.camera.y);
    printf("camera.x2: %i\n", game.camera.x + game.camera.w);
    printf("camera.y2: %i\n\n", game.camera.y + game.camera.h);
#endif
    
    game.camera.x = tile_mul(player.pos.x) - (game.camera.w / 2);
    // NOTE(rami): This gives us 24 pixels from the top and bottom
    // initially  when the camera is not locked to an edge which seems to be
    // the closest we can get to 32 pixels.
    game.camera.y = tile_mul(player.pos.y) - (game.camera.h / 2) + (player.size.h / 2);
    
    if(game.camera.x < 0)
    {
        game.camera.x = 0;
    }
    
    if(game.camera.y < 0)
    {
        game.camera.y = 0;
    }
    
    if(game.camera.x >= tile_mul(MAX_LEVEL_WIDTH) - game.camera.w)
    {
        game.camera.x = tile_mul(MAX_LEVEL_WIDTH) - game.camera.w;
    }
    
    if(game.camera.y >= tile_mul(MAX_LEVEL_HEIGHT) - game.camera.h)
    {
        game.camera.y = tile_mul(MAX_LEVEL_HEIGHT) - game.camera.h;
    }
}

internal void
update_events()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            game.state = state_quit;
        }
        
        else if(event.type == SDL_KEYDOWN)
        {
#if 1
            // TODO(rami): Debug
            if(1)
#else
                if(!event.key.repeat)
#endif
            {
                SDL_Scancode key = event.key.keysym.scancode;
                
                b32 alt_key_was_down = event.key.keysym.mod & KMOD_ALT;
                if((key == SDL_SCANCODE_F4) && alt_key_was_down)
                {
                    game.state = state_quit;
                }
                else if((key == SDL_SCANCODE_RETURN) && alt_key_was_down)
                {
                    SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);
                    if(window)
                    {
                        toggle_fullscreen();
                    }
                }
                else
                {
                    player_keypress(key);
                }
            }
        }
    }
}

internal b32
set_window_icon()
{
    b32 result = false;
    
    SDL_Surface *icon = IMG_Load("data/images/icon.png");
    if(icon)
    {
        SDL_SetWindowIcon(game.window, icon);
        SDL_FreeSurface(icon);
        result = true;
    }
    else
    {
        printf("ERROR: Failed to load window icon\n");
    }
    
    return(result);
}

internal b32
set_fonts()
{
    b32 result = true;
    
    fonts[font_classic] = create_bmp_font("data/fonts/classic16x16.png", 16, 16, 14, 8, 12);
    fonts[font_cursive] = create_ttf_font("data/fonts/alkhemikal.ttf", 16, 4);
    fonts[font_misc] = create_ttf_font("data/fonts/monaco.ttf", 16, 4);
    
    for(u32 i = 0; i < font_total; ++i)
    {
        if(!fonts[i]->success)
        {
            result = false;
            printf("ERROR: Font atlas %u could not be created\n", i);
        }
    }
    
    return(result);
}

internal b32
set_textures()
{
    b32 result = true;
    
    textures[tex_tilemap].tex = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tile_mul(MAX_LEVEL_WIDTH), tile_mul(MAX_LEVEL_HEIGHT));
    textures[tex_tilemap].w = tile_mul(MAX_LEVEL_WIDTH);
    textures[tex_tilemap].h = tile_mul(MAX_LEVEL_HEIGHT);
    
    textures[tex_game_tileset] = load_texture("data/images/game_tileset.png", 0);
    textures[tex_item_tileset] = load_texture("data/images/item_tileset.png", 0);
    textures[tex_wearable_item_tileset] = load_texture("data/images/wearable_item_tileset.png", 0);
    textures[tex_sprite_sheet] = load_texture("data/images/sprite_sheet.png", 0);
    textures[tex_inventory_win] = load_texture("data/images/inventory_win.png", 0);
    textures[tex_inventory_item_win] = load_texture("data/images/inventory_item_win.png", 0);
    textures[tex_inventory_selected_item] = load_texture("data/images/inventory_selected_item.png", 0);
    textures[tex_interface_bottom_win] = load_texture("data/images/interface_bottom_win.png", 0);
    textures[tex_health_bar_outside] = load_texture("data/images/health_bar_outside.png", 0);
    textures[tex_health_bar_inside] = load_texture("data/images/health_bar_inside.png", 0);
    textures[tex_player_parts] = load_texture("data/images/player_parts.png", 0);
    
    for(u32 i = 0; i < tex_total; ++i)
    {
        if(!textures[i].tex)
        {
            result = false;
            printf("ERROR: Texture %u could not be created\n", i);
        }
    }
    
    return(result);
}

internal void
set_game_data()
{
    // TODO(rami): Debug
    u64 seed = 1565467134;
    //u64 seed = time(0);
    srand(seed);
    printf("Random Seed: %lu\n\n", seed);
    
    game.state = state_running;
    game.window_size = V2u(1280, 720);
    game.console_size = V2u(game.window_size.w, 160);
    game.camera = V4i(0, 0, game.window_size.w, game.window_size.h - game.console_size.h);
    game.turn_changed = false;
    
    level.current_level = 1;
    level.w = 64;
    level.h = 64;
    
    for(u32 i = 0; i < array_count(items); ++i)
    {
        items[i].unique_id = i + 1;
    }
    
    for(u32 i = 0; i < array_count(console_messages); ++i)
    {
        strcpy(console_messages[i].msg, CONSOLE_MESSAGE_EMPTY);
        console_messages[i].color = color_black;
    }
    
    set_monster_spawn_chances();
    set_item_info_data();
}

internal u32
init_game()
{
    b32 result = false;
    
    set_game_data();
    
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        u32 window_flags = 0;
        game.window = SDL_CreateWindow("Moonbreath", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       game.window_size.w, game.window_size.h,
                                       window_flags);
        if(game.window)
        {
            printf("Monitor refresh rate: %dHz\n", get_window_refresh_rate());
            
            u32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
            game.renderer = SDL_CreateRenderer(game.window, -1, renderer_flags);
            if(game.renderer)
            {
                u32 img_flags = IMG_INIT_PNG;
                if(IMG_Init(img_flags) & img_flags)
                {
                    if(!TTF_Init())
                    {
                        if(set_window_icon())
                        {
                            if(set_fonts())
                            {
                                if(set_textures())
                                {
                                    result = true;
                                }
                                else
                                {
                                    // NOTE(rami): Texture failed
                                }
                            }
                            else
                            {
                                // NOTE(rami): Font failed
                            }
                        }
                        else
                        {
                            // NOTE(rami): Window icon failed
                        }
                    }
                    else
                    {
                        printf("ERROR: SDL TTF library could not initialize: %s\n", SDL_GetError());
                    }
                }
                else
                {
                    printf("ERROR: SLD image library could not initialize: %s\n", SDL_GetError());
                }
            }
            else
            {
                printf("ERROR: SDL could not create a renderer: %s\n", SDL_GetError());
            }
        }
        else
        {
            printf("ERROR: SDL could not create window: %s\n", SDL_GetError());
        }
    }
    else
    {
        printf("ERROR: SDL could not initialize: %s\n", SDL_GetError());
    }
    
    return(result);
}

internal void
array_debug()
{
    // NOTE(rami): Pop up text
#if 0
    for(i32 i = array_count(pop_up_text) - 1; i > -1; --i)
    {
        if(pop_up_text[i].active)
        {
            printf("\npop_up_text[%u]\n", i);
            printf("str: %s\n", pop_up_text[i].str);
            printf("x: %u, y: %u\n", pop_up_text[i].pos.x, pop_up_text[i].pos.y);
            printf("change: %.02f\n", pop_up_text[i].change);
            printf("speed: %.02f\n", pop_up_text[i].speed);
            printf("duration_time: %ums\n", pop_up_text[i].duration_time);
            printf("start_time: %ums\n", pop_up_text[i].start_time);
        }
    }
#endif
    
    // NOTE(rami): Inventory
#if 0
    for(i32 i = INVENTORY_SLOT_COUNT - 1; i > -1; --i)
    {
        if(inventory.slot[i].id)
        {
            printf("\nInventory.slots[%u]\n", i);
            printf("id %u\n", inventory.slot[i].id);
            printf("unique_id %u\n", inventory.slot[i].unique_id);
            printf("x: %u, y: %u\n", inventory.slot[i].x, inventory.slot[i].y);
            printf("in_inventory %u\n", inventory.slot[i].in_inventory);
            printf("equipped %u\n", inventory.slot[i].equipped);
        }
    }
#endif
    
    // NOTE(rami): Item
#if 0
    for(i32 i = array_count(items) - 1; i > -1; --i)
    {
        if(items[i].id)
        {
            printf("\nitems[%u]\n", i);
            printf("id %u\n", items[i].id);
            printf("unique_id %u\n", items[i].unique_id);
            printf("x: %u, y: %u\n", items[i].pos.x, items[i].pos.y);
            printf("in_inventory %u\n", items[i].in_inventory);
            printf("is_equipped %u\n", items[i].equipped);
        }
    }
#endif
    
    // NOTE(rami): Player
#if 0
    printf("\nPlayer\n");
    printf("frame_start.x, y: %u, %u\n", player.sprite.start_frame.x,
           player.sprite.start_frame.y);
    printf("frame_current.x, y: %u, %u\n", player.sprite.current_frame.x,
           player.sprite.current_frame.y);
    printf("frame_count: %u\n", player.sprite.frame_count);
    printf("frame_delay: %u\n", player.sprite.frame_duration);
    printf("frame_last_changed: %u\n", player.sprite.frame_last_changed);
    printf("new_x, new_y: %u, %u\n", player.new_pos.x, player.new_pos.y);
    printf("x, y: %u, %u\n", player.pos.x, player.pos.y);
    printf("w, h: %u, %u\n", player.size.w, player.size.h);
    printf("name: %s\n", player.name);
    printf("max_hp: %u\n", player.max_hp);
    printf("hp: %u\n", player.hp);
    printf("damage: %u\n", player.damage);
    printf("armor: %u\n", player.armor);
    printf("speed: %u\n", player.speed);
    printf("level: %u\n", player.level);
    printf("money: %u\n", player.money);
    printf("fov: %u\n", player.fov);
    printf("sprite_flip: %u\n", player.sprite_flip);
#endif
    
    // NOTE(rami): Monster
#if 0
    for(i32 i = array_count(monsters) - 1; i > -1; --i)
    {
        if(monsters[i].type)
        {
            printf("\nmonster[%u]\n", i);
            printf("type: %u\n", monsters[i].type);
            printf("ai: %u\n", monsters[i].ai);
            
            printf("x, y: %u, %u\n", monsters[i].pos.x, monsters[i].pos.y);
            printf("w, h: %u, %u\n", monsters[i].size.w, monsters[i].size.h);
            printf("in_combat: %u\n", monsters[i].in_combat);
            printf("max_hp: %u\n", monsters[i].max_hp);
            printf("hp: %u\n", monsters[i].hp);
            printf("damage: %u\n", monsters[i].damage);
            printf("armor: %u\n", monsters[i].armor);
            printf("speed: %u\n", monsters[i].speed);
            printf("level: %u\n", monsters[i].level);
        }
    }
#endif
}

internal void
run_game()
{
    add_player();
    generate_level();
    update_fov();
    
#if 1
    add_monster(monster_slime, 55, 11);
    add_monster(monster_slime, 56, 11);
    add_monster(monster_skeleton, 57, 11);
    add_monster(monster_skeleton, 58, 11);
#endif
    
#if 0
    add_item(id_rune_helmet, player.pos.x, player.pos.y);
    add_item(id_rune_amulet, player.pos.x, player.pos.y);
    add_item(id_rune_chestplate, player.pos.x, player.pos.y);
    add_item(id_rune_platelegs, player.pos.x, player.pos.y);
    add_item(id_rune_boots, player.pos.x, player.pos.y);
    add_item(id_iron_sword, player.pos.x, player.pos.y);
    add_item(id_iron_sword, player.pos.x, player.pos.y);
    add_item(id_rune_shield, player.pos.x, player.pos.y);
    add_item(id_rune_ring, player.pos.x, player.pos.y);
#endif
    
    /*add_item(id_red_chestplate, 50, 31);
    add_item(id_red_sword, 51, 31);
    add_item(id_lesser_health_potion, 52, 31);*/
    
    u32 frames_per_second = 60;
    f32 target_seconds_per_frame = 1.0f / (f32)frames_per_second;
    
    u64 old_counter = SDL_GetPerformanceCounter();
    f32 old_dt = SDL_GetPerformanceCounter();
    f32 perf_count_frequency = (f32)SDL_GetPerformanceFrequency();
    
    while(game.state)
    {
        // TODO(rami): Debug
        array_debug();
        
        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);
        
        f32 new_dt = SDL_GetPerformanceCounter();
        game.dt = (f32)(new_dt - old_dt) / perf_count_frequency;
        old_dt = new_dt;
        
        update_events();
        
        if(game.turn_changed)
        {
            update_player();
            update_monsters();
            update_fov();
            
            game.turn_changed = false;
        }
        
        update_camera();
        update_pop_up_text();
        
        render_tilemap();
        render_items();
        render_monsters();
        render_player();
        render_ui();
        render_pop_up_text();
        
        if(inventory.open)
        {
            render_inventory();
        }
        
        u64 work_counter_elapsed = SDL_GetPerformanceCounter() - old_counter;
        f32 ms_for_work = (1000.0f * (f32)work_counter_elapsed) / perf_count_frequency;
        
        if(get_seconds_elapsed(old_counter, SDL_GetPerformanceCounter(), perf_count_frequency) < target_seconds_per_frame)
        {
            u32 time_to_delay =
                ((target_seconds_per_frame - get_seconds_elapsed(old_counter, SDL_GetPerformanceCounter(), perf_count_frequency)) * 1000) - 1;
            SDL_Delay(time_to_delay);
            
            while(get_seconds_elapsed(old_counter, SDL_GetPerformanceCounter(), perf_count_frequency)
                  < target_seconds_per_frame)
            {
            }
        }
        else
        {
            // NOTE(rami): Valgrind will trigger this!
            //assert(0, "Missed frate rate");
        }
        
        u64 new_counter = SDL_GetPerformanceCounter();
        u64 elapsed_counter = new_counter - old_counter;
        
        f32 ms_per_frame = (1000.0f * (f32)elapsed_counter) / perf_count_frequency;
        f32 frames_per_second = perf_count_frequency / (f32)elapsed_counter;
        old_counter = new_counter;
        
        // TODO(rami): Debug
#if 1
        render_text("Frames Per Second: %.02f", V2u(25, 25), color_white, fonts[font_classic], frames_per_second);
        render_text("MS Per Frame: %.02f", V2u(25, 50), color_white, fonts[font_classic], ms_per_frame);
        render_text("MS For Update and Render: %.02f", V2u(25, 75), color_white, fonts[font_classic], ms_for_work);
        render_text("DT Per Frame: %.02f", V2u(25, 100), color_white, fonts[font_classic], game.dt);
#endif
        
        SDL_RenderPresent(game.renderer);
    }
}

internal void
exit_game()
{
    free_assets();
    
    if(game.renderer)
    {
        SDL_DestroyRenderer(game.renderer);
        game.renderer = 0;
    }
    
    if(game.window)
    {
        SDL_DestroyWindow(game.window);
        game.window = 0;
    }
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

i32
main(int argc, char *argv[])
{
    if(init_game())
    {
        run_game();
        exit_game();
        return(EXIT_SUCCESS);
    }
    
    exit_game();
    return(EXIT_FAILURE);
}
