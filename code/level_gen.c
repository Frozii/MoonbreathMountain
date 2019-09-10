internal void
set_tile(v2u pos, tile_type tile)
{
    level.tiles[pos.y][pos.x].tile = tile;
}

internal void
set_rect(v4u rect, tile_type tile)
{
    for(u32 y = rect.y; y < rect.y + rect.h; ++y)
    {
        for(u32 x = rect.x; x < rect.x + rect.w; ++x)
        {
            set_tile(V2u(x, y), tile);
        }
    }
}

internal b32
is_tile(v2u pos, u32 tile)
{
    b32 result = (level.tiles[pos.y][pos.x].tile == tile);
    return(result);
}

internal void
set_occupied(v2u pos, b32 value)
{
    level.tiles[pos.y][pos.x].occupied = value;
}

internal b32
is_occupied(v2u pos)
{
    b32 result = level.tiles[pos.y][pos.x].occupied;
    return(result);
}

internal b32
is_traversable(v2u pos)
{
    b32 result = (is_tile(pos, tile_floor_stone) ||
                  is_tile(pos, tile_floor_grass) ||
                  is_tile(pos, tile_door_open) ||
                  is_tile(pos, tile_path_up) ||
                  is_tile(pos, tile_path_down));
    
    return(result);
}

internal b32
is_inside_level(v2u pos)
{
    b32 result = false;
    
    if(pos.x >= 0 && pos.x < MAX_LEVEL_WIDTH &&
       pos.y >= 0 && pos.y < MAX_LEVEL_HEIGHT)
    {
        result = true;
    }
    
    return(result);
}

internal v2u
get_rand_rect_pos(v4u rect)
{
    v2u pos = {0};
    pos.x = rand_num(rect.x, rect.x + rect.w - 1);
    pos.y = rand_num(rect.y, rect.y + rect.h - 1);
    return(pos);
}

internal v2u
get_rand_free_level_pos()
{
    v2u result = {0};
    
    for(;;)
    {
        v2u pos = V2u(rand_num(0, level.w - 1), rand_num(0, level.h - 1));
        if(is_traversable(pos))
        {
            if(!is_occupied(pos))
            {
                result = pos;
                break;
            }
        }
    }
    
    return(result);
}

internal void
set_player_start(v2u start_pos)
{
    player.pos = start_pos;
    player.new_pos = start_pos;
    set_occupied(player.pos, true);
}

internal void
set_level_monsters()
{
    u32 slime_count = 0;
    u32 skeleton_count = 0;
    
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        // TODO(rami): Debug
        monster_type type = get_monster_for_level();
        if(type == monster_slime)
        {
            ++slime_count;
        }
        else if(type == monster_skeleton)
        {
            ++skeleton_count;
        }
        
        v2u pos = get_rand_free_level_pos();
        add_monster(type, pos.x, pos.y);
    }
    
    printf("slimes: %u\n", slime_count);
    printf("skeletons: %u\n", skeleton_count);
}

internal void
set_automaton_room(automaton_t *src, automaton_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u tile_pos = V2u(room.x + x, room.y + y);
            dest->ptr[(tile_pos.y * dest->width) + tile_pos.x].tile = src->ptr[(y * src->width) + x].tile;
        }
    }
}

internal u32
get_neighbour_wall_count(automaton_t *src, v2u pos, v4u room)
{
    u32 count = 0;
    
    for(i32 y = pos.y - 1; y < (i32)pos.y + 2; ++y)
    {
        for(i32 x = pos.x - 1; x < (i32)pos.x + 2; ++x)
        {
            if(x < room.x || y < room.y || x >= room.x + room.w || y >= room.y + room.h)
            {
                ++count;
            }
            else if(x != pos.x || y != pos.y)
            {
                if(src->ptr[(y * src->width) + x].tile == tile_wall_stone)
                {
                    ++count;
                }
            }
        }
    }
    
    return(count);
}

internal void
create_automaton_room(automaton_t *src, automaton_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u tile_pos = V2u(room.x + x, room.y + y);
            u32 wall_count = get_neighbour_wall_count(src, tile_pos, room);
            
            if(src->ptr[(tile_pos.y * src->width) + tile_pos.x].tile == tile_floor_stone)
            {
                if(wall_count >= 5)
                {
                    dest->ptr[(y * dest->width) + x].tile = tile_wall_stone;
                }
                else
                {
                    dest->ptr[(y * dest->width) + x].tile = tile_floor_stone;
                }
            }
            else if(src->ptr[(tile_pos.y * src->width) + tile_pos.x].tile == tile_wall_stone)
            {
                if(wall_count <= 4)
                {
                    dest->ptr[(y * dest->width) + x].tile = tile_floor_stone;
                }
                else
                {
                    dest->ptr[(y * dest->width) + x].tile = tile_wall_stone;
                }
            }
        }
    }
}

internal v4u
get_room_size(room_type type)
{
    v4u result = {0};
    
    switch(type)
    {
        case room_rectangle:
        {
            result.w = rand_num(rectangle_min_size, rectangle_max_size);
            result.h = rand_num(rectangle_min_size, rectangle_max_size);
        } break;
        
        case room_double_rectangle:
        {
            result.w = rand_num(double_rectangle_min_size, double_rectangle_max_size);
            result.h = rand_num(double_rectangle_min_size, double_rectangle_max_size);
        } break;
        
        case room_automaton:
        {
            result.w = rand_num(automaton_min_size, automaton_max_size);
            result.h = rand_num(automaton_min_size, automaton_max_size);
        } break;
    }
    
    result.x = rand_num(1, (level.w - 1) - result.w);
    result.y = rand_num(1, (level.h - 1) - result.h);
    
    return(result);
}

internal b32
is_area_free(v4u room, u32 padding)
{
    for(u32 y = room.y - padding; y < room.y + room.h + padding; ++y)
    {
        for(u32 x = room.x - padding; x < room.x + room.w + padding; ++x)
        {
            if(!is_tile(V2u(x, y), tile_wall_stone))
            {
                return(false);
            }
        }
    }
    
    return(true);
}

internal void
set_rectangle_room(v4u room)
{
    set_rect(room, tile_floor_stone);
}

internal room_result_t
set_double_rectangle_room(v4u room_one)
{
    room_result_t result = {0};
    
    v4u room_two = {0};
    room_two.w = rand_num(3, 6);
    room_two.h = rand_num(3, 6);
    room_two.x = room_one.x + rand_num(2, room_one.w - 2);
    room_two.y = room_one.y + rand_num(2, room_one.h - 2);
    
    v4u final_room = {0};
    final_room.x = room_one.x;
    final_room.y = room_one.y;
    
    // NOTE(rami): Set the correct final room width.
    if(room_one.x + room_one.w >= room_two.x + room_two.w)
    {
        final_room.w = (room_one.x + room_one.w) - room_one.x;
    }
    else
    {
        final_room.w = (room_two.x + room_two.w) - room_one.x;
    }
    
    // NOTE(rami): Set the correct final room height.
    if(room_one.y + room_one.h >= room_two.y + room_two.h)
    {
        final_room.h = (room_one.y + room_one.h) - room_one.y;
    }
    else
    {
        final_room.h = (room_two.y + room_two.h) - room_one.y;
    }
    
    // NOTE(rami): final_room top left point is inside the level,
    // check if final_room bottom right point is inside the level as well.
    if(is_inside_level(V2u(final_room.x + final_room.w,
                           final_room.y + final_room.h)))
    {
        if(is_area_free(final_room, 1))
        {
            set_rect(room_one, tile_floor_stone);
            set_rect(room_two, tile_floor_stone);
            
            result.valid = true;
            result.room = final_room;
        }
    }
    
    return(result);
}

internal void
set_cellular_automata_room(v4u room)
{
    for(u32 y = room.y; y < room.y + room.h; ++y)
    {
        for(u32 x = room.x; x < room.x + room.w; ++x)
        {
            u32 rand = rand_num(0, 100);
            if(rand <= 55)
            {
                set_tile(V2u(x, y), tile_floor_stone);
            }
        }
    }
    
    tile_t buff_one[automaton_max_size * automaton_max_size] = {tile_none};
    tile_t buff_two[automaton_max_size * automaton_max_size] = {tile_none};
    
    automaton_t level_data = {(tile_t *)level.tiles, MAX_LEVEL_WIDTH};
    automaton_t buff_one_data = {buff_one, automaton_max_size};
    automaton_t buff_two_data = {buff_two, automaton_max_size};
    
    create_automaton_room(&level_data, &buff_one_data, room);
    
    v4u buff_room = V4u(0, 0, room.w, room.h);
    create_automaton_room(&buff_one_data, &buff_two_data, buff_room);
    create_automaton_room(&buff_two_data, &buff_one_data, buff_room);
    create_automaton_room(&buff_one_data, &buff_two_data, buff_room);
    create_automaton_room(&buff_two_data, &buff_one_data, buff_room);
    
    set_automaton_room(&buff_one_data, &level_data, room);
}

internal room_result_t
generate_room(room_type type)
{
    room_result_t result = {0};
    v4u room = get_room_size(type);
    
    if(type == room_double_rectangle)
    {
        room_result_t new_room = set_double_rectangle_room(room);
        if(new_room.valid)
        {
            result.valid = true;
            result.room = new_room.room;
        }
    }
    else
    {
        if(is_area_free(room, 1))
        {
            if(type == room_rectangle)
            {
                set_rectangle_room(room);
            }
            else
            {
                set_cellular_automata_room(room);
            }
            
            result.valid = true;
            result.room = room;
        }
    }
    
    return(result);
}

internal u32
set_level_start(v4u *rooms, u32 room_count)
{
    u32 start_room_index = 0;
    v2u start_pos = {0};
    
    for(;;)
    {
        start_room_index = rand_num(0, room_count - 1);
        start_pos = get_rand_rect_pos(rooms[start_room_index]);
        
        if(is_traversable(start_pos))
        {
            set_tile(start_pos, tile_path_up);
            break;
        }
    }
    
    set_player_start(start_pos);
    return(start_room_index);
}

internal void
set_level_end(v4u *rooms, u32 room_count, u32 start_room_index)
{
    v2u start_room_pos = V2u(rooms[start_room_index].x, rooms[start_room_index].y);
    u32 end_room = 0;
    u32 best_dist = 0;
    
    for(u32 i = 0; i < room_count; ++i)
    {
        v2u current_room_pos = V2u(rooms[i].x, rooms[i].y);
        
        u32 dist = tile_dist(start_room_pos, current_room_pos);
        if(dist > best_dist)
        {
            end_room = i;
            best_dist = dist;
        }
    }
    
    for(;;)
    {
        v2u end_pos = get_rand_rect_pos(rooms[end_room]);
        if(is_traversable(end_pos))
        {
            set_tile(end_pos, tile_path_down);
            break;
        }
    }
}

internal void
generate_level()
{
    for(u32 y = 0; y < level.h; ++y)
    {
        for(u32 x = 0; x < level.w; ++x)
        {
            v2u pos = V2u(x, y);
            set_occupied(pos, false);
            set_tile(pos, tile_wall_stone);
        }
    }
    
    b32 rooms_done = false;
    v4u rooms[128] = {0};
    u32 room_count = 0;
    u32 tiles_occupied = 0;
    
    while(!rooms_done)
    {
        room_type type = rand_num(room_type_first, room_type_last);
        
        room_result_t result = generate_room(type);
        if(result.valid)
        {
            rooms[room_count++] = result.room;
            tiles_occupied += result.room.w * result.room.h;
            
            if((f32)tiles_occupied / (f32)(level.w * level.h) >= 0.45f)
            {
                rooms_done = true;
            }
        }
        
        // TODO(rami): Then take a look at the second room
        // that's an automata room, look at the room progression
        // over the five passes.
    }
    
    // TODO(rami): Debug
    printf("\nRoom Count: %u\n", room_count);
#if 0
    for(u32 i = 0; i < room_count; ++i)
    {
        printf("rooms[%u].x: %u\n", i, rooms[i].x);
        printf("rooms[%u].y: %u\n", i, rooms[i].y);
        printf("rooms[%u].w: %u\n", i, rooms[i].w);
        printf("rooms[%u].h: %u\n\n", i, rooms[i].h);
    }
#endif
    
    u32 start_room_index = set_level_start(rooms, room_count);
    set_level_end(rooms, room_count, start_room_index);
    
    set_level_monsters();
}