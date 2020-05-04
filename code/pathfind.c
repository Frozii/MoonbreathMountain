#if 0
internal void
print_pathfind_map(u32 *map, u32 width, u32 height)
{
    for(u32 y = 0; y < height; ++y)
    {
        for(u32 x = 0; x < width; ++x)
        {
            v2u current = {x, y};
            if(get_pathfind_value(map, width, current) != 1024)
            {
                printf("%u ", get_pathfind_value(map, width, current));
            }
        }
        
        printf("\n");
    }
}
#endif

internal u32
get_pathfind_value(u32 *map, u32 width, v2u pos)
{
    u32 result = map[(pos.y * width) + pos.x];
    return(result);
}

internal void
set_pathfind_value(u32 *map, u32 width, v2u pos, u32 value)
{
    map[(pos.y * width) + pos.x] = value;
}

internal v2u
next_pathfind_pos(dungeon_t *dungeon, u32 *map, u32 width, entity_t *enemy, entity_t *player)
{
    v2u result = {0};
    
    u32 lowest_distance = get_pathfind_value(map, width, enemy->pos);
    v2u pos = {0, 0};
    
    for(u32 dir = dir_up; dir <= dir_down_right; ++dir)
    {
        switch(dir)
        {
            case dir_up: pos = V2u(enemy->pos.x, enemy->pos.y - 1); break;
            case dir_down: pos = V2u(enemy->pos.x, enemy->pos.y + 1); break;
            case dir_left: pos = V2u(enemy->pos.x - 1, enemy->pos.y); break;
            case dir_right: pos = V2u(enemy->pos.x + 1, enemy->pos.y); break;
            
            case dir_up_left: pos = V2u(enemy->pos.x - 1, enemy->pos.y - 1); break;
            case dir_up_right: pos = V2u(enemy->pos.x + 1, enemy->pos.y - 1); break;
            case dir_down_left: pos = V2u(enemy->pos.x - 1, enemy->pos.y + 1); break;
            case dir_down_right: pos = V2u(enemy->pos.x + 1, enemy->pos.y + 1); break;
            
            invalid_default_case;
        }
        
        u32 pos_distance = get_pathfind_value(map, width, pos);
        if(pos_distance <= lowest_distance && (!is_tile_occupied(dungeon->tiles, pos) || V2u_equal(pos, player->pos)))
        {
            lowest_distance = pos_distance;
            result = pos;
        }
    }
    
    return(result);
}

internal void
update_pathfind_map(dungeon_t *dungeon, entity_t *player)
{
    if(is_tile_traversable(dungeon->tiles, player->pos))
    {
        // NOTE(rami): Initialize to a high value.
        u32 map_default_value = 1024;
        for(u32 y = 0; y < dungeon->height; ++y)
        {
            for(u32 x = 0; x < dungeon->width; ++x)
            {
                set_pathfind_value(dungeon->pathfind_map, dungeon->width, V2u(x, y), map_default_value);
            }
        }
        
        // NOTE(rami): This is the lowest number, the goal.
        set_pathfind_value(dungeon->pathfind_map, dungeon->width, player->pos, 0);
        
        for(;;)
        {
            next_iteration:
            
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
                {
                    v2u current = {x, y};
                    
                    // NOTE(rami): We need to be able to go through closed doors
                    // with this so we don't infinite loop. If we were to have
                    // different doors in the future, we would need something like
                    // a is_door() function to be used here instead.
                    if(is_tile_traversable(dungeon->tiles, current) ||
                       is_tile_value(dungeon->tiles, current, tile_stone_door_closed))
                    {
                        u32 lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, current);
                        
                        // TODO(Rami): This needs to take care of diagonals, so it doesn't
                        // infinite loop.
                        
                        if(is_inside_rectangle(current, V4u(0, 0, dungeon->width, dungeon->height)))
                        {
                            // NOTE(rami): Up
                            v2u up = {x, y - 1};
                            if(get_pathfind_value(dungeon->pathfind_map, dungeon->width, up) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, up);
                            }
                            
                            // NOTE(rami): Down
                            v2u down = {x, y + 1};
                            if(get_pathfind_value(dungeon->pathfind_map, dungeon->width, down) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, down);
                            }
                            
                            // NOTE(rami): Left
                            v2u left = {x - 1, y};
                            if(get_pathfind_value(dungeon->pathfind_map, dungeon->width, left) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, left);
                            }
                            
                            // NOTE(rami): Right
                            v2u right = {x + 1, y};
                            if(get_pathfind_value(dungeon->pathfind_map, dungeon->width, right) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, right);
                            }
                            
                            // NOTE(Rami): Top Left
                            v2u top_left = {x - 1, y - 1};
                            if(get_pathfind_value(dungeon->pathfind_map, dungeon->width, top_left) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, top_left);
                            }
                            
                            // NOTE(Rami): Top Right
                            v2u top_right = {x + 1, y - 1};
                            if(get_pathfind_value(dungeon->pathfind_map, dungeon->width, top_right) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, top_right);
                            }
                            
                            // NOTE(Rami): Bottom Left
                            v2u bottom_left = {x - 1, y + 1};
                            if(get_pathfind_value(dungeon->pathfind_map, dungeon->width, bottom_left) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, bottom_left);
                            }
                            
                            // NOTE(Rami): Bottom Right
                            v2u bottom_right = {x + 1, y + 1};
                            if(get_pathfind_value(dungeon->pathfind_map, dungeon->width, bottom_right) < lowest_neighbour)
                            {
                                lowest_neighbour = get_pathfind_value(dungeon->pathfind_map, dungeon->width, bottom_right);
                            }
                            
                            if(lowest_neighbour < get_pathfind_value(dungeon->pathfind_map, dungeon->width, current))
                            {
                                set_pathfind_value(dungeon->pathfind_map, dungeon->width, current, lowest_neighbour + 1);
                            }
                        }
                    }
                }
            }
            
#if 0
            printf("\n\nResult Map\n");
            print_map(map, width, height);
#endif
            
            for(u32 y = 0; y < dungeon->height; ++y)
            {
                for(u32 x = 0; x < dungeon->width; ++x)
                {
                    v2u current = {x, y};
                    if(is_tile_traversable(dungeon->tiles, current) &
                       get_pathfind_value(dungeon->pathfind_map, dungeon->width, current) == map_default_value)
                    {
                        goto next_iteration;
                    }
                }
            }
            
            break;
        }
    }
}