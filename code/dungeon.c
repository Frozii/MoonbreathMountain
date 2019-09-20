internal b32
is_tile(v2u pos, u32 tile)
{
    b32 result = (dungeon.tiles[pos.y][pos.x].tile == tile);
    return(result);
}

internal void
set_tile(v2u pos, u32 tile)
{
    dungeon.tiles[pos.y][pos.x].tile = tile;
}

internal void
set_occupied(v2u pos, b32 value)
{
    dungeon.tiles[pos.y][pos.x].occupied = value;
}

internal b32
is_occupied(v2u pos)
{
    b32 result = (dungeon.tiles[pos.y][pos.x].occupied);
    return(result);
}

internal b32
is_wall(v2u pos)
{
    b32 result = (is_tile(pos, tile_stone_wall_one) ||
                  is_tile(pos, tile_stone_wall_two) ||
                  is_tile(pos, tile_stone_wall_three) ||
                  is_tile(pos, tile_stone_wall_four));
    
    return(result);
}

internal void
set_wall(v2u pos)
{
    u32 wall = rand_num(tile_stone_wall_one, tile_stone_wall_four);
    set_tile(pos, wall);
}

internal void
set_floor(v2u pos)
{
    u32 floor = rand_num(tile_stone_floor_one, tile_stone_floor_five);
    set_tile(pos, floor);
}

internal b32
is_traversable(v2u pos)
{
    b32 result = (is_tile(pos, tile_stone_floor_one) ||
                  is_tile(pos, tile_stone_floor_two) ||
                  is_tile(pos, tile_stone_floor_three) ||
                  is_tile(pos, tile_stone_floor_four) ||
                  is_tile(pos, tile_stone_floor_five) ||
                  is_tile(pos, tile_grass_floor) ||
                  is_tile(pos, tile_stone_door_open));
    
    return(result);
}

internal b32
is_inside_dungeon(v2u pos)
{
    b32 result = (pos.x < MAX_DUNGEON_WIDTH && pos.y < MAX_DUNGEON_HEIGHT);
    return(result);
}

internal v2u
get_rand_rect_pos(v4u rect)
{
    v2u result = {0};
    result.x = rand_num(rect.x, rect.x + rect.w - 1);
    result.y = rand_num(rect.y, rect.y + rect.h - 1);
    return(result);
}

internal v2u
get_rand_dungeon_pos()
{
    v2u result = {0};
    result.x = rand_num(0, dungeon.w - 1);
    result.y = rand_num(0, dungeon.h - 1);
    return(result);
}

internal monster_type
get_dungeon_monster()
{
    monster_type result = monster_none;
    
    u32 rand = rand_num(0, 100);
    u32 count = 0;
    
    for(u32 i = 0; i < monster_total; ++i)
    {
        count += monster_spawn_chance[i][dungeon.level - 1];
        if(count >= rand)
        {
            result = i + 1;
            break;
        }
    }
    
    return(result);
}

internal void
set_dungeon_player_start(v2u start_pos)
{
    player.pos = start_pos;
    player.new_pos = start_pos;
    set_occupied(player.pos, true);
}

internal void
set_dungeon_monsters()
{
    memset(monsters, 0, sizeof(monsters));
    
    u32 slime_count = 0;
    u32 skeleton_count = 0;
    
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        // TODO(rami): Debug
        monster_type type = get_dungeon_monster();
        if(type == monster_slime)
        {
            ++slime_count;
        }
        else if(type == monster_skeleton)
        {
            ++skeleton_count;
        }
        
        for(;;)
        {
            v2u pos = get_rand_dungeon_pos();
            if(is_traversable(pos))
            {
                if(!is_occupied(pos))
                {
                    add_monster(type, pos);
                    break;
                }
            }
        }
    }
    
    printf("slimes: %u\n", slime_count);
    printf("skeletons: %u\n\n", skeleton_count);
}

internal void
place_automaton_room(automaton_t *src, automaton_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u tile_pos = {room.x + x, room.y + y};
            dest->ptr[(tile_pos.y * dest->width) + tile_pos.x].tile = src->ptr[(y * src->width) + x].tile;
        }
    }
}

internal b32
is_automaton_wall(automaton_t *automaton, v2u pos)
{
    b32 result = false;
    
    if(automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_wall_one ||
       automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_wall_two ||
       automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_wall_three ||
       automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_wall_four)
    {
        result = true;
    }
    
    return(result);
}

internal void
set_automaton_wall(automaton_t *automaton, v2u pos)
{
    u32 wall = rand_num(tile_stone_wall_one, tile_stone_wall_four);
    automaton->ptr[(pos.y * automaton->width) + pos.x].tile = wall;
}

internal b32
is_automaton_floor(automaton_t *automaton, v2u pos)
{
    b32 result = false;
    
    if(automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_floor_one ||
       automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_floor_two ||
       automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_floor_three ||
       automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_floor_four ||
       automaton->ptr[(pos.y * automaton->width) + pos.x].tile == tile_stone_floor_five)
    {
        result = true;
    }
    
    return(result);
}

internal void
set_automaton_floor(automaton_t *automaton, v2u pos)
{
    u32 floor = rand_num(tile_stone_floor_one, tile_stone_floor_five);
    automaton->ptr[(pos.y * automaton->width) + pos.x].tile = floor;
}

internal u32
get_automaton_neighbour_tile_count(automaton_t *src, v2u pos, v4u room)
{
    u32 count = 0;
    
    for(i32 y = pos.y - 1; y < (i32)pos.y + 2; ++y)
    {
        for(i32 x = pos.x - 1; x < (i32)pos.x + 2; ++x)
        {
            if(x < (i32)room.x || y < (i32)room.y || x >= (i32)room.x + (i32)room.w || y >= (i32)room.y + (i32)room.h)
            {
                ++count;
            }
            else if(x != (i32)pos.x || y != (i32)pos.y)
            {
                if(is_automaton_wall(src, V2u(x, y)))
                {
                    ++count;
                }
            }
        }
    }
    
    return(count);
}

internal void
apply_automaton(automaton_t *src, automaton_t *dest, v4u room)
{
    for(u32 y = 0; y < room.h; ++y)
    {
        for(u32 x = 0; x < room.w; ++x)
        {
            v2u src_tile_pos = {room.x + x, room.y + y};
            v2u dest_tile_pos = {x, y};
            u32 wall_count = get_automaton_neighbour_tile_count(src, src_tile_pos, room);
            
            if(is_automaton_floor(src, src_tile_pos))
            {
                if(wall_count >= 5)
                {
                    set_automaton_wall(dest, dest_tile_pos);
                }
                else
                {
                    set_automaton_floor(dest, dest_tile_pos);
                }
            }
            else if(is_automaton_wall(src, src_tile_pos))
            {
                if(wall_count <= 4)
                {
                    set_automaton_floor(dest, dest_tile_pos);
                }
                else
                {
                    set_automaton_wall(dest, dest_tile_pos);
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
    
    result.x = rand_num(1, (dungeon.w - 1) - result.w);
    result.y = rand_num(1, (dungeon.h - 1) - result.h);
    
    return(result);
}

internal b32
is_area_free(v4u room, u32 padding)
{
    for(u32 y = room.y - padding; y < room.y + room.h + padding; ++y)
    {
        for(u32 x = room.x - padding; x < room.x + room.w + padding; ++x)
        {
            v2u pos = {x, y};
            if(!is_wall(pos))
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
    for(u32 y = room.y; y < room.y + room.h; ++y)
    {
        for(u32 x = room.x; x < room.x + room.w; ++x)
        {
            v2u pos = {x ,y};
            set_floor(pos);
        }
    }
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
    
    // NOTE(rami): final_room top left point is inside the dungeon,
    // check if final_room bottom right point is inside the dungeon as well.
    if(is_inside_dungeon(V2u(final_room.x + final_room.w,
                             final_room.y + final_room.h)))
    {
        if(is_area_free(final_room, 1))
        {
            set_rectangle_room(room_one);
            set_rectangle_room(room_two);
            
            result.valid = true;
            result.room = final_room;
        }
    }
    
    return(result);
}

internal void
set_automaton_room(v4u room)
{
    for(u32 y = room.y; y < room.y + room.h; ++y)
    {
        for(u32 x = room.x; x < room.x + room.w; ++x)
        {
            u32 rand = rand_num(0, 100);
            if(rand <= 55)
            {
                v2u pos = {x, y};
                set_floor(pos);
            }
        }
    }
    
    tile_t buff_one[automaton_max_size * automaton_max_size] = {0};
    tile_t buff_two[automaton_max_size * automaton_max_size] = {0};
    
    automaton_t dungeon_data = {(tile_t *)dungeon.tiles, MAX_DUNGEON_WIDTH};
    automaton_t buff_one_data = {buff_one, automaton_max_size};
    automaton_t buff_two_data = {buff_two, automaton_max_size};
    
    apply_automaton(&dungeon_data, &buff_one_data, room);
    
    v4u buff_room = V4u(0, 0, room.w, room.h);
    apply_automaton(&buff_one_data, &buff_two_data, buff_room);
    apply_automaton(&buff_two_data, &buff_one_data, buff_room);
    apply_automaton(&buff_one_data, &buff_two_data, buff_room);
    apply_automaton(&buff_two_data, &buff_one_data, buff_room);
    
    place_automaton_room(&buff_one_data, &dungeon_data, room);
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
                set_automaton_room(room);
            }
            
            result.valid = true;
            result.room = room;
        }
    }
    
    return(result);
}

internal u32
set_dungeon_start(v4u *rooms, u32 room_count)
{
    u32 start_room_index = 0;
    v2u start_pos = {0};
    
    start_room_index = rand_num(0, room_count - 1);
    
    for(;;)
    {
        start_pos = get_rand_rect_pos(rooms[start_room_index]);
        if(is_traversable(start_pos))
        {
            set_tile(start_pos, tile_stone_path_up);
            set_dungeon_player_start(start_pos);
            break;
        }
    }
    
    return(start_room_index);
}

internal void
set_dungeon_end(v4u *rooms, u32 room_count, u32 start_room_index)
{
    v2u start_room_pos = {rooms[start_room_index].x, rooms[start_room_index].y};
    u32 end_room = 0;
    u32 best_dist = 0;
    
    for(u32 i = 0; i < room_count; ++i)
    {
        v2u current_room_pos = {rooms[i].x, rooms[i].y};
        
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
            set_tile(end_pos, tile_stone_path_down);
            break;
        }
    }
}

internal b32
scan_for_dungeon_corridor(v2u start, u32 direction)
{
    b32 scanning = false;
    i32 x_dir = 0;
    i32 y_dir = 0;
    
    switch(direction)
    {
        case up: y_dir = -1; break;
        case down: y_dir = 1; break;
        case left: x_dir = -1; break;
        case right: x_dir = 1; break;
    }
    
    for(;;)
    {
        v2u scan = {start.x, start.y};
        start.x += x_dir;
        start.y += y_dir;
        
        if(is_inside_dungeon(scan))
        {
            if(scanning)
            {
                if(is_traversable(scan))
                {
                    return(true);
                }
            }
            else
            {
                if(!is_traversable(scan))
                {
                    scanning = true;
                }
            }
        }
        else
        {
            return(false);
        }
    }
}

internal void
set_dungeon_corridor(v2u start, u32 direction)
{
    b32 setting_tiles = false;
    i32 x_dir = 0;
    i32 y_dir = 0;
    
    switch(direction)
    {
        case up: y_dir = -1; break;
        case down: y_dir = 1; break;
        case left: x_dir = -1; break;
        case right: x_dir = 1; break;
    }
    
    for(;;)
    {
        v2u scan = {start.x, start.y};
        start.x += x_dir;
        start.y += y_dir;
        
        if(setting_tiles)
        {
            if(is_traversable(scan))
            {
                return;
            }
            else
            {
                set_floor(scan);
            }
        }
        else
        {
            if(!is_traversable(scan))
            {
                setting_tiles = true;
                set_floor(scan);
            }
        }
    }
}

internal void
connect_dungeon_rooms(v4u *rooms, u32 room_count)
{
    for(u32 i = 0; i < room_count; ++i)
    {
        u32 corridor_count = 0;
        while(corridor_count < 2)
        {
            u32 direction = rand_num(up, right);
            v2u start = get_rand_rect_pos(rooms[i]);
            if(is_traversable(start))
            {
                if(scan_for_dungeon_corridor(start, direction))
                {
                    set_dungeon_corridor(start, direction);
                    ++corridor_count;
                }
            }
        }
    }
}

internal void
generate_dungeon()
{
    for(u32 y = 0; y < dungeon.h; ++y)
    {
        for(u32 x = 0; x < dungeon.w; ++x)
        {
            v2u pos = {x, y};
            set_seen(pos, false);
            set_occupied(pos, false);
            set_wall(pos);
        }
    }
    
    b32 rooms_done = false;
    v4u rooms[128] = {0};
    u32 room_count = 0;
    
    while(!rooms_done)
    {
        room_type type = rand_num(room_rectangle, room_automaton);
        room_result_t result = generate_room(type);
        if(result.valid)
        {
            rooms[room_count++] = result.room;
            if(room_count >= 32)
            {
                rooms_done = true;
            }
        }
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
    
    u32 start_room_index = set_dungeon_start(rooms, room_count);
    set_dungeon_end(rooms, room_count, start_room_index);
    
    connect_dungeon_rooms(rooms, room_count);
    //set_dungeon_monsters();
}