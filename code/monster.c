internal void
set_monster_spawn_chance(monster_type type, monster_tier tier)
{
    // NOTE(rami): Monster types start at 1 so we need to decrement by one
    // to get the correct array element.
    --type;
    
    // TODO(rami): Remove the things that get set to zero later.
    // TODO(rami): Medium and High tier monsters should start later,
    // Low tier monsters should have good chances at starting levels and slowly
    // fade as we go into higher levels.
    if(tier == monster_tier_low)
    {
        monster_spawn_chance[type][0] = 50;
        monster_spawn_chance[type][1] = 40;
        monster_spawn_chance[type][2] = 30;
        monster_spawn_chance[type][3] = 20;
        monster_spawn_chance[type][4] = 10;
        monster_spawn_chance[type][5] = 5;
        monster_spawn_chance[type][6] = 0;
        monster_spawn_chance[type][7] = 0;
        monster_spawn_chance[type][8] = 0;
        monster_spawn_chance[type][9] = 0;
    }
    else if(tier == monster_tier_medium)
    {
        monster_spawn_chance[type][0] = 0;
        monster_spawn_chance[type][1] = 0;
        monster_spawn_chance[type][2] = 5;
        monster_spawn_chance[type][3] = 10;
        monster_spawn_chance[type][4] = 20;
        monster_spawn_chance[type][5] = 50;
        monster_spawn_chance[type][6] = 40;
        monster_spawn_chance[type][7] = 30;
        monster_spawn_chance[type][8] = 20;
        monster_spawn_chance[type][9] = 10;
    }
    else
    {
        monster_spawn_chance[type][0] = 0;
        monster_spawn_chance[type][1] = 0;
        monster_spawn_chance[type][2] = 0;
        monster_spawn_chance[type][3] = 0;
        monster_spawn_chance[type][4] = 0;
        monster_spawn_chance[type][5] = 0;
        monster_spawn_chance[type][6] = 10;
        monster_spawn_chance[type][7] = 20;
        monster_spawn_chance[type][8] = 30;
        monster_spawn_chance[type][9] = 40;
    }
}

internal void
add_monster(monster_type type, v2u pos)
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(!monster->type)
        {
            monster->type = type;
            monster->ai = ai_wandering;
            
            monster->pos = pos;
            monster->new_pos = pos;
            set_occupied(monster->pos, true);
            
            switch(type)
            {
                case monster_slime:
                {
                    strcpy(monster->name, "Slime");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 3;
                    monster->hp = monster->max_hp;
                    monster->damage = 2;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(1, 0);
                } break;
                
                case monster_cave_bat:
                {
                    strcpy(monster->name, "Cave Bat");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 1;
                    monster->tile = V2u(5, 0);
                } break;
                
                case monster_python:
                {
                    strcpy(monster->name, "Python");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 2;
                    monster->hp = monster->max_hp;
                    monster->damage = 1;
                    monster->attack_speed = 2;
                    monster->move_speed = 1;
                    monster->level = 2;
                    monster->tile = V2u(6, 0);
                } break;
                
                case monster_skeleton:
                {
                    strcpy(monster->name, "Skeleton");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 5;
                    monster->hp = monster->max_hp;
                    monster->damage = 3;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 2;
                    monster->tile = V2u(2, 0);
                } break;
                
                case monster_armored_skeleton:
                {
                    strcpy(monster->name, "Armored Skeleton");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 5;
                    monster->hp = monster->max_hp;
                    monster->damage = 4;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 3;
                    monster->tile = V2u(3, 0);
                } break;
                
                case monster_orc_warrior:
                {
                    strcpy(monster->name, "Orc Warrior");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 6;
                    monster->hp = monster->max_hp;
                    monster->damage = 3;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 3;
                    monster->tile = V2u(4, 0);
                } break;
                
                case monster_kobold:
                {
                    strcpy(monster->name, "Kobold");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 6;
                    monster->hp = monster->max_hp;
                    monster->damage = 4;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 3;
                    monster->tile = V2u(7, 0);
                } break;
                
                case monster_ogre:
                {
                    strcpy(monster->name, "Ogre");
                    monster->w = 32;
                    monster->h = 32;
                    monster->max_hp = 8;
                    monster->hp = monster->max_hp;
                    monster->damage = 6;
                    monster->attack_speed = 1;
                    monster->move_speed = 1;
                    monster->level = 4;
                    monster->tile = V2u(8, 0);
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
}

internal void
get_monster_attack_message(monster_type type, char *message)
{
    switch(type)
    {
        case monster_slime:
        {
            strcpy(message, "The Slime bites you for");
        } break;
        
        case monster_skeleton:
        {
            u32 value = rand_num(1, 2);
            if(value == 1)
            {
                strcpy(message, "The Skeleton punches you for");
            }
            else
            {
                strcpy(message, "The Skeleton kicks you for");
            }
        } break;
        
        case monster_armored_skeleton:
        {
            strcpy(message, "The Skeleton slashes you for");
        } break;
        
        case monster_orc_warrior:
        {
            u32 value = rand_num(1, 2);
            if(value == 1)
            {
                strcpy(message, "The Orc Warrior punches you for");
            }
            else
            {
                strcpy(message, "The Orc Warrior kicks you for");
            }
        } break;
        
        case monster_cave_bat:
        {
            u32 value = rand_num(1, 2);
            if(value == 1)
            {
                strcpy(message, "The Cave Bat bites you for");
            }
            else
            {
                strcpy(message, "The Cave Bat slashes you for");
            }
        } break;
        
        case monster_python:
        {
            strcpy(message, "The Python bites you for");
        } break;
        
        case monster_kobold:
        {
            u32 value = rand_num(1, 2);
            if(value == 1)
            {
                strcpy(message, "The Kobold slashes you for");
            }
            else
            {
                strcpy(message, "The Kobold stabs you for");
            }
        } break;
        
        case monster_ogre:
        {
            strcpy(message, "The Ogre bashes you for");
        } break;
        
        invalid_default_case;
    }
}

internal void
monster_attack_player(monster_t *monster)
{
    player.hp -= monster->damage;
    if(player.hp > player.max_hp)
    {
        player.hp = 0;
    }
    
    char attack[64] = {0};
    get_monster_attack_message(monster->type, attack);
    
    add_console_text("%s %u damage.", color_white, attack, monster->damage);
    add_pop_text("%u", player.pos, text_normal_attack, monster->damage);
}

internal void
apply_monster_ai(monster_t *monster)
{
    if(monster->ai == ai_wandering)
    {
        u32 direction = rand_num(dir_up, dir_right);
        if(direction == dir_up)
        {
            --monster->new_pos.y;
        }
        else if(direction == dir_down)
        {
            ++monster->new_pos.y;
        }
        else if(direction == dir_left)
        {
            --monster->new_pos.x;
            monster->tile_flip = true;
        }
        else
        {
            ++monster->new_pos.x;
            monster->tile_flip = false;
        }
    }
}

internal void
monster_traverse_path(monster_t *monster, path_t *path)
{
    if(!monster->has_attacked && V2u_equal(path->list[0], player.pos))
    {
        for(u32 i = 0; i < monster->attack_speed; ++i)
        {
            monster_attack_player(monster);
        }
        
        monster->has_attacked = true;
    }
    else
    {
        if(is_occupied(path->list[0]))
        {
            u32 current_dist = tile_dist_cardinal(monster->pos, player.pos);
            v2u directions[dir_count] = {dir_none};
            
            directions[dir_up] = V2u(monster->pos.x, monster->pos.y - 1);
            directions[dir_down] = V2u(monster->pos.x, monster->pos.y + 1);
            directions[dir_left] = V2u(monster->pos.x - 1, monster->pos.y);
            directions[dir_right] = V2u(monster->pos.x + 1, monster->pos.y);
            
            directions[dir_top_left] = V2u(monster->pos.x - 1, monster->pos.y - 1);
            directions[dir_top_right] = V2u(monster->pos.x + 1, monster->pos.y - 1);
            directions[dir_bottom_left] = V2u(monster->pos.x - 1, monster->pos.y + 1);
            directions[dir_bottom_right] = V2u(monster->pos.x + 1, monster->pos.y + 1);
            
            for(u32 i = dir_up; i < dir_count; i++)
            {
                if(tile_dist_cardinal_and_ordinal(directions[i], player.pos) < current_dist)
                {
                    if(is_traversable(directions[i]) && !is_occupied(directions[i]))
                    {
                        monster->new_pos = directions[i];
                        break;
                    }
                }
            }
        }
        else
        {
            monster->new_pos = path->list[0];
        }
    }
}

internal void
update_monsters()
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(monster->type)
        {
            for(u32 speed_index = 0; speed_index < monster->move_speed; ++speed_index)
            {
                if(monster->in_combat)
                {
                    // NOTE(rami): Turn monster sprite towards target.
                    if(player.pos.x < monster->pos.x)
                    {
                        monster->tile_flip = true;
                    }
                    else
                    {
                        monster->tile_flip = false;
                    }
                    
                    path_t *path = pathfind(monster->pos, player.pos, pathfind_cardinal);
                    if(path->was_found)
                    {
                        monster_traverse_path(monster, path);
                    }
                    else
                    {
                        monster->in_combat = false;
                    }
                    
                    free(path);
                }
                else
                {
                    apply_monster_ai(monster);
                }
                
                if(is_traversable(monster->new_pos))
                {
                    if(!is_occupied(monster->new_pos))
                    {
                        set_occupied(monster->pos, false);
                        monster->pos = monster->new_pos;
                        set_occupied(monster->pos, true);
                    }
                }
                
                // NOTE(rami): This is to keep the new_pos locked.
                monster->new_pos = monster->pos;
            }
            
            monster->has_attacked = false;
        }
    }
}

internal void
remove_monster(monster_t *monster)
{
    set_occupied(monster->pos, false);
    memset(monster, 0, sizeof(monster_t));
}

internal void
render_monsters()
{
    for(u32 i = 0; i < array_count(monsters); ++i)
    {
        monster_t *monster = &monsters[i];
        if(monster->type &&
           is_seen(monster->pos))
        {
            v2u pos = get_game_pos(monster->pos);
            v4u src = {tile_mul(monster->tile.x), tile_mul(monster->tile.y), monster->w, monster->h};
            v4u dest = {pos.x, pos.y, monster->w, monster->h};
            
            SDL_RenderCopyEx(game.renderer, textures[tex_sprite_sheet].tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, monster->tile_flip);
            
            // Render Monster HP Bar
            if(monster->in_combat &&
               monster->hp)
            {
                // HP Bar Outside
                set_render_color(color_black);
                v4u hp_bar_outside = {pos.x, pos.y + 33, 32, 4};
                SDL_RenderDrawRect(game.renderer, (SDL_Rect *)&hp_bar_outside);
                
                // HP Bar Inside
                set_render_color(color_dark_red);
                u32 hp_bar_inside_w = get_ratio(monster->hp, monster->max_hp, 30);
                v4u hp_bar_inside = {pos.x + 1, pos.y + 34, hp_bar_inside_w, 2};
                SDL_RenderFillRect(game.renderer, (SDL_Rect *)&hp_bar_inside);
            }
        }
    }
}