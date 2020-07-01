internal b32
player_moved_while_confused(RandomState *random, Entity *player, Direction came_from_direction)
{
    b32 result = false;
    
    if(player->p.effects[EffectType_Confusion].is_enabled)
    {
        u32 chance = random_number(random, 1, 100);
        assert(player->p.effects[EffectType_Confusion].value);
        
        if(chance <= player->p.effects[EffectType_Confusion].value)
        {
            for(;;)
            {
                Direction direction = get_random_direction(random);
                if(direction != came_from_direction)
                {
                    switch(direction)
                    {
                        case Direction_Up: --player->new_pos.y; break;
                        case Direction_Down: ++player->new_pos.y; break;
                        case Direction_Left: --player->new_pos.x; break;
                        case Direction_Right: ++player->new_pos.x; break;
                        
                        case Direction_UpLeft: --player->new_pos.x; player->new_pos.y--; break;
                        case Direction_UpRight: ++player->new_pos.x; player->new_pos.y--; break;
                        case Direction_DownLeft: --player->new_pos.x; player->new_pos.y++; break;
                        case Direction_DownRight: ++player->new_pos.x; player->new_pos.y++; break;
                        
                        invalid_default_case;
                    }
                    
                    result = true;
                    break;
                }
            }
        }
    }
    
    return(result);
}

internal void
end_player_status_effect(StatusEffect *effect)
{
    memset(effect, 0, sizeof(StatusEffect));
}

internal void
start_player_status_effect(Entity *player, EffectType index, u32 value, u32 duration)
{
    player->p.effects[index].is_enabled = true;
    player->p.effects[index].duration = duration;
    player->p.effects[index].value = value;
    
    switch(index)
    {
        case EffectType_Resistance: break;
        case EffectType_Flight: break;
        case EffectType_Poison: break;
        case EffectType_Confusion: break;
        
        case EffectType_Might: player->strength += value; break;
        case EffectType_Wisdom: player->intelligence += value; break;
        case EffectType_Agility: player->dexterity += value; break;
        case EffectType_Fortitude: player->p.defence += value; break;
        case EffectType_Focus: player->evasion += value; break;
        
        case EffectType_Weakness:
        {
            if((s32)(player->p.defence - value) <= 0)
            {
                player->p.defence = 0;
            }
            else
            {
                player->p.defence -= value;
            }
        } break;
        
        case EffectType_Decay:
        {
            player->strength -= value;
            player->intelligence -= value;
            player->dexterity -= value;
        } break;
        
        invalid_default_case;
    }
}

internal b32
entity_will_hit(RandomState *random, u32 hit_chance, u32 evasion)
{
    b32 result = (random_number(random, 1, hit_chance) >= evasion);
    return(result);
}

internal void
move_entity(Dungeon *dungeon, v2u new_pos, Entity *entity)
{
    entity->new_pos = new_pos;
    set_tile_occupied(dungeon->tiles, entity->pos, false);
    entity->pos = new_pos;
    set_tile_occupied(dungeon->tiles, entity->pos, true);
}

internal b32
heal_entity(Entity *entity, u32 value)
{
    b32 result = false;
    
    if(entity->hp < entity->max_hp)
    {
        result = true;
        
        entity->hp += value;
        if(entity->hp > entity->max_hp)
        {
            entity->hp = entity->max_hp;
        }
    }
    
    return(result);
}

internal String128
entity_attack_message(GameState *game, Entity *attacker, Entity *defender, Inventory *inventory)
{
    String128 result = {0};
    
    if(attacker->type == EntityType_Player)
    {
        char *attack = 0;
        
        InventorySlot slot = equipped_inventory_slot_from_item_slot(ItemSlot_FirstHand, inventory);
        if(slot.item)
        {
            switch(slot.item->id)
            {
                case ItemID_Dagger:
                case ItemID_Sword:
                {
                    u32 roll = random_number(&game->random, 1, 6);
                    switch(roll)
                    {
                        case 1: attack = "stab"; break;
                        case 2: attack = "pierce"; break;
                        case 3: attack = "puncture"; break;
                        case 4: attack = "slash"; break;
                        case 5: attack = "lacerate"; break;
                        case 6: attack = "cleave"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Club:
                case ItemID_Warhammer:
                {
                    u32 roll = random_number(&game->random, 1, 6);
                    switch(roll)
                    {
                        case 1: attack = "smash"; break;
                        case 2: attack = "bash"; break;
                        case 3: attack = "strike"; break;
                        case 4: attack = "pummel"; break;
                        case 5: attack = "pound"; break;
                        case 6: attack = "crush"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Battleaxe:
                {
                    u32 roll = random_number(&game->random, 1, 6);
                    switch(roll)
                    {
                        case 1: attack = "hack"; break;
                        case 2: attack = "rend"; break;
                        case 3: attack = "chop"; break;
                        case 4: attack = "slash"; break;
                        case 5: attack = "lacerate"; break;
                        case 6: attack = "cleave"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                case ItemID_Spear:
                {
                    u32 roll = random_number(&game->random, 1, 4);
                    switch(roll)
                    {
                        case 1: attack = "stab"; break;
                        case 2: attack = "pierce"; break;
                        case 3: attack = "puncture"; break;
                        case 4: attack = "strike"; break;
                        
                        invalid_default_case;
                    }
                } break;
                
                invalid_default_case;
            }
        }
        else
        {
            u32 roll = random_number(&game->random, 1, 2);
            switch(roll)
            {
                case 1: attack = "punch"; break;
                case 2: attack = "kick"; break;
                
                invalid_default_case;
            }
        }
        
        snprintf(result.str, sizeof(result.str), "You %s the %s", attack, defender->name);
    }
    else if(attacker->type == EntityType_Enemy)
    {
        snprintf(result.str, sizeof(result.str), "The %s attacks you", attacker->name);
    }
    
    return(result);
}

internal void
remove_entity(Entity *entity)
{
    memset(entity, 0, sizeof(Entity));
}

internal void
kill_entity(GameState *game, Dungeon *dungeon, String128 *log, Entity *entity)
{
    if(entity->type == EntityType_Player)
    {
        // TODO(rami): Spawn blood on player on death.
        //log_text(log, "You are dead!", entity->name);
        
        // TODO(rami): Just so we don't have to look at the underflow,
        // decide what to do with this later.
        entity->hp = 0;
    }
    else
    {
        log_text(log, "%sThe %s dies!", start_color(Color_LightRed), entity->name);
        set_tile_occupied(dungeon->tiles, entity->pos, false);
        
        // TODO(rami): Amount of blood in the blood tiles needs to be adjusted,
        // would be nice to have maybe a size variable or something so we know
        // how much blood we want to put down.
        TileID remains = tile_none;
        if(entity->e.is_red_blooded)
        {
            remains = random_number(&game->random,
                                    Tile_RedBloodPuddle1,
                                    Tile_RedBloodSplatter3);
        }
        else if(entity->e.is_green_blooded)
        {
            remains = random_number(&game->random,
                                    Tile_GreenBloodPuddle1,
                                    Tile_GreenBloodSplatter3);
        }
        
        set_tile_remains_value(dungeon->tiles, entity->pos, remains);
        remove_entity(entity);
    }
}

internal b32
entity_is_dead(Entity *entity)
{
    b32 result = false;
    
    if((s32)entity->hp <= 0)
    {
        result = true;
    }
    
    return(result);
}

internal void
attack_entity(GameState *game,
              Dungeon *dungeon,
              String128 *log,
              Inventory *inventory,
              Entity *attacker,
              Entity *defender)
{
    // TODO(rami): Need to apply player defence.
    // Attacker damage is reduced by a certain amount,
    // maybe a number between 0 and defence, maybe something else.
    String128 attack = entity_attack_message(game, attacker, defender, inventory);
    log_text(log, "%s for %u damage.", attack.str, attacker->damage);
    
    defender->hp -= attacker->damage;
    if(entity_is_dead(defender))
    {
        kill_entity(game, dungeon, log, defender);
    }
}

internal void
update_enemy_ai(GameState *game, Entity *enemy)
{
    enemy->new_pos = enemy->pos;
    
    Direction direction = get_random_direction(&game->random);
    switch(direction)
    {
        case Direction_Up:
        {
            --enemy->new_pos.y;
        } break;
        
        case Direction_Down:
        {
            ++enemy->new_pos.y;
        } break;
        
        case Direction_Left:
        {
            enemy->e.is_flipped = true;
            --enemy->new_pos.x;
        } break;
        
        case Direction_Right:
        {
            enemy->e.is_flipped = false;
            ++enemy->new_pos.x;
        } break;
        
        case Direction_UpLeft:
        {
            enemy->e.is_flipped = true;
            --enemy->new_pos.x;
            --enemy->new_pos.y;
        } break;
        
        case Direction_UpRight:
        {
            enemy->e.is_flipped = false;
            ++enemy->new_pos.x;
            --enemy->new_pos.y;
        } break;
        
        case Direction_DownLeft:
        {
            enemy->e.is_flipped = true;
            --enemy->new_pos.x;
            ++enemy->new_pos.y;
        } break;
        
        case Direction_DownRight:
        {
            enemy->e.is_flipped = false;
            ++enemy->new_pos.x;
            ++enemy->new_pos.y;
        } break;
        
        invalid_default_case;
    }
}

internal void
update_entities(GameState *game,
                GameInput *input,
                Entity *player,
                Entity *entities,
                Dungeon *dungeon,
                Item *items,
                ConsumableData *consumable_data,
                String128 *log,
                Inventory *inventory,
                u32 *enemy_levels)
{
    // Update Player
    b32 should_update_player = false;
    player->action_speed = 0.0f;
    
    if(inventory->is_asking_player)
    {
        if(is_input_valid(&input->Key_Yes))
        {
            log_text(log, "The scroll turns illegible, you discard it.");
            inventory->is_asking_player = false;
            
            InventorySlot slot = {inventory->use_item_src_index, inventory->slots[slot.index]};
            remove_item_from_inventory_and_game(slot, player, log, inventory);
            reset_inventory_item_use(inventory);
        }
        else if(is_input_valid(&input->Key_No))
        {
            inventory->is_asking_player = false;
        }
    }
    else
    {
#if MOONBREATH_SLOW
        if(is_input_valid(&input->Key_ToggleFov))
        {
            debug_fov = !debug_fov;
            update_fov(dungeon, player);
        }
        else if(is_input_valid(&input->Key_ToggleTraversable))
        {
            should_update_player = true;
            debug_traversable = !debug_traversable;
        }
        // We need to check this manually
        // so that it works as an expected toggle.
        else if(input->Key_ToggleHasBeenUp.is_down &&
                input->Key_ToggleHasBeenUp.was_up)
        {
            should_update_player = true;
            input->Key_ToggleHasBeenUp.was_up = false;
            debug_has_been_up = !debug_has_been_up;
        }
        else if(is_input_valid(&input->Key_ToggleIdentify))
        {
            if(inventory->is_open)
            {
                Item *item = get_inventory_slot_item(inventory, inventory->pos);
                if(item)
                {
                    item->is_identified = !item->is_identified;
                }
            }
        }
        else
#endif
        
        if(is_input_valid(&input->Key_Up))
        {
            if(inventory->is_open)
            {
                if(inventory->pos.y > 0)
                {
                    --inventory->pos.y;
                }
                else
                {
                    inventory->pos.y = INVENTORY_HEIGHT - 1;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_Up))
                {
                    --player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(is_input_valid(&input->Key_Down))
        {
            if(inventory->is_open)
            {
                if((inventory->pos.y + 1) < INVENTORY_HEIGHT)
                {
                    ++inventory->pos.y;
                }
                else
                {
                    inventory->pos.y = 0;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_Down))
                {
                    ++player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(is_input_valid(&input->Key_Left))
        {
            if(inventory->is_open)
            {
                if(inventory->pos.x > 0)
                {
                    --inventory->pos.x;
                }
                else
                {
                    inventory->pos.x = INVENTORY_WIDTH - 1;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_Left))
                {
                    --player->new_pos.x;
                }
                
                should_update_player = true;
            }
        }
        else if(is_input_valid(&input->Key_Right))
        {
            if(inventory->is_open)
            {
                if((inventory->pos.x + 1) < INVENTORY_WIDTH)
                {
                    ++inventory->pos.x;
                }
                else
                {
                    inventory->pos.x = 0;
                }
            }
            else
            {
                if(!player_moved_while_confused(&game->random, player, Direction_Right))
                {
                    ++player->new_pos.x;
                }
                
                should_update_player = true;
            }
        }
        else if(is_input_valid(&input->Key_UpLeft))
        {
            if(!inventory->is_open)
            {
                if(!player_moved_while_confused(&game->random, player, Direction_UpLeft))
                {
                    --player->new_pos.x;
                    --player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(is_input_valid(&input->Key_UpRight))
        {
            if(!inventory->is_open)
            {
                if(!player_moved_while_confused(&game->random, player, Direction_UpRight))
                {
                    ++player->new_pos.x;
                    --player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(is_input_valid(&input->Key_DownLeft))
        {
            if(!inventory->is_open)
            {
                if(!player_moved_while_confused(&game->random, player, Direction_DownLeft))
                {
                    --player->new_pos.x;
                    ++player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(is_input_valid(&input->Key_DownRight))
        {
            if(!inventory->is_open)
            {
                if(!player_moved_while_confused(&game->random, player, Direction_DownRight))
                {
                    ++player->new_pos.x;
                    ++player->new_pos.y;
                }
                
                should_update_player = true;
            }
        }
        else if(is_input_valid(&input->Key_Inventory))
        {
            if(inventory->item_use_type == ItemUseType_Identify ||
               player_is_enchanting(inventory->item_use_type))
            {
                if(!inventory->is_asking_player)
                {
                    ask_for_item_cancel(game, log, inventory);
                }
            }
            else
            {
                inventory->is_open = !inventory->is_open;
                inventory->is_asking_player = false;
                inventory->pos = V2u(0, 0);
                
                reset_inventory_item_use(inventory);
            }
        }
        else if(is_input_valid(&input->Key_EquipOrConsumeItem))
        {
            Item *item = get_inventory_slot_item(inventory, inventory->pos);
            if(item)
            {
                if(is_item_consumable(item->type))
                {
                    InventorySlot slot = get_slot_from_pos(inventory, inventory->pos);
                    
                    if(!item->is_identified)
                    {
                        set_consumable_as_known(item->id, items, consumable_data);
                    }
                    
                    switch(item->id)
                    {
                        case ItemID_MightPotion:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You drink the potion.. you feel powerful.");
                                start_player_status_effect(player, EffectType_Might, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_WisdomPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel knowledgeable.");
                                start_player_status_effect(player, EffectType_Wisdom, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_AgilityPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. your body feels nimble.");
                                start_player_status_effect(player, EffectType_Agility, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_FortitudePotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. your body feels stronger.");
                                start_player_status_effect(player, EffectType_Fortitude, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_ResistancePotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                // TODO(rami): Implement resistances.
                                log_text(log, "You drink the potion.. your body feels resistive.");
                                start_player_status_effect(player, EffectType_Resistance, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_HealingPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                if(heal_entity(player, item->c.value))
                                {
                                    log_text(log, "You drink the potion.. you feel slightly better.");
                                }
                                else
                                {
                                    log_text(log, "You drink the potion.. you feel the same as before.");
                                }
                                
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_FocusPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel very attentive.");
                                start_player_status_effect(player, EffectType_Focus, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_CuringPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                if(player->p.effects[EffectType_Poison].is_enabled)
                                {
                                    log_text(log, "You drink the potion.. you feel much better.");
                                    end_player_status_effect(&player->p.effects[EffectType_Poison]);
                                }
                                else
                                {
                                    log_text(log, "You drink the potion.. you feel the same as before.");
                                }
                                
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_FlightPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                // TODO(rami): The only thing we would want to really fly over
                                // right now is just walls, if we have water, lava, whatever in
                                // the future then this becomes more relevant.
                                // Flying and being on the ground would obviously be separate states.
                                
                                log_text(log, "You drink the potion.. you feel much lighter.");
                                start_player_status_effect(player, EffectType_Flight, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_DecayPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel impaired.");
                                start_player_status_effect(player, EffectType_Decay, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_WeaknessPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel weaker.");
                                start_player_status_effect(player, EffectType_Weakness, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_WoundingPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. painful wounds appear on your body.");
                                
                                player->hp -= item->c.value;
                                if(entity_is_dead(player))
                                {
                                    kill_entity(game, dungeon, log, player);
                                }
                                
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_VenomPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel very sick.");
                                start_player_status_effect(player, EffectType_Poison, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_ConfusionPotion:
                        {
                            if(!player_is_enchanting(inventory->item_use_type))
                            {
                                log_text(log, "You drink the potion.. you feel confused.");
                                start_player_status_effect(player, EffectType_Confusion, item->c.value, item->c.duration);
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                            }
                        } break;
                        
                        case ItemID_IdentifyScroll:
                        {
                            u32 slot_index = get_inventory_slot_index(inventory->pos);
                            if(item_use_is_active(ItemUseType_Identify, slot_index, inventory))
                            {
                                if(!inventory->is_asking_player)
                                {
                                    ask_for_item_cancel(game, log, inventory);
                                }
                            }
                            else if(item_use_is_none(slot_index, inventory))
                            {
                                log_text(log, "You read the scroll.. choose an item to identify.");
                                inventory->item_use_type = ItemUseType_Identify;
                                inventory->use_item_src_index = get_inventory_slot_index(inventory->pos);
                            }
                        } break;
                        
#if 0
                        case ItemID_InfuseWeaponScroll:
                        {
                            log_text(log, "You read the scroll.. choose an item to infuse.");
                            
                            // TODO(rami): Implement infuse weapon.
                            //inventory->item_use_type = use_type_infuse_weapon;
                            inventory->use_item_src_index = get_inventory_slot_index(inventory->pos);
                        } break;
#endif
                        
                        case ItemID_EnchantWeaponScroll:
                        {
                            u32 slot_index = get_inventory_slot_index(inventory->pos);
                            if(item_use_is_active(ItemUseType_EnchantWeapon, slot_index, inventory))
                            {
                                if(!inventory->is_asking_player)
                                {
                                    ask_for_item_cancel(game, log, inventory);
                                }
                            }
                            else if(item_use_is_none(slot_index, inventory))
                            {
                                log_text(log, "You read the scroll.. choose a weapon to enchant.");
                                inventory->item_use_type = ItemUseType_EnchantWeapon;
                                inventory->use_item_src_index = get_inventory_slot_index(inventory->pos);
                            }
                        } break;
                        
                        case ItemID_EnchantArmourScroll:
                        {
                            u32 slot_index = get_inventory_slot_index(inventory->pos);
                            if(item_use_is_active(ItemUseType_EnchantArmour, slot_index, inventory))
                            {
                                if(!inventory->is_asking_player)
                                {
                                    ask_for_item_cancel(game, log, inventory);
                                }
                            }
                            else if(item_use_is_none(slot_index, inventory))
                            {
                                log_text(log, "You read the scroll.. choose an armor to enchant.");
                                inventory->item_use_type = ItemUseType_EnchantArmour;
                                inventory->use_item_src_index = get_inventory_slot_index(inventory->pos);
                            }
                        } break;
                        
                        case ItemID_MagicMappingScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You read the scroll.. your surroundings become clear to you.");
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                                
                                for(u32 y = 0; y < MAX_DUNGEON_SIZE; ++y)
                                {
                                    for(u32 x = 0; x < MAX_DUNGEON_SIZE; ++x)
                                    {
                                        set_tile_has_been_seen(dungeon->tiles, V2u(x, y), true);
                                    }
                                }
                            }
                        } break;
                        
                        case ItemID_TeleportationScroll:
                        {
                            if(!inventory->item_use_type)
                            {
                                log_text(log, "You read the scroll.. you find yourself in a different place.");
                                remove_item_from_inventory_and_game(slot, player, log, inventory);
                                
                                for(;;)
                                {
                                    v2u tele_pos = random_dungeon_pos(game, dungeon);
                                    if(is_tile_traversable_and_not_occupied(dungeon->tiles, tele_pos))
                                    {
                                        move_entity(dungeon, tele_pos, player);
                                        break;
                                    }
                                }
                                
                                update_fov(dungeon, player);
                            }
                        } break;
                        
                        invalid_default_case;
                    }
                }
                else
                {
                    if(!inventory->item_use_type)
                    {
                        if(item->is_equipped)
                        {
                            unequip_item(item, player);
                        }
                        else
                        {
                            InventorySlot slot = equipped_inventory_slot_from_item_slot(item->slot, inventory);
                            if(slot.item)
                            {
                                unequip_item(slot.item, player);
                            }
                            
                            item->is_identified = true;
                            equip_item(item, player);
                        }
                    }
                }
            }
        }
        else if(is_input_valid(&input->Key_PickupOrDropItem))
        {
            if(inventory->is_open)
            {
                if(!inventory->item_use_type)
                {
                    InventorySlot slot = get_slot_from_pos(inventory, inventory->pos);
                    if(slot.item)
                    {
                        remove_item_from_inventory(slot, player, log, inventory);
                        
                        if(slot.item->is_identified)
                        {
                            String128 item_name = full_item_name(slot.item);
                            log_text(log, "You drop the %s%s%s.",
                                     item_rarity_color_code(slot.item->rarity),
                                     item_name.str,
                                     end_color());
                        }
                        else
                        {
                            log_text(log, "You drop the %s%s%s.",
                                     item_rarity_color_code(slot.item->rarity),
                                     item_id_text(slot.item->id),
                                     end_color());
                        }
                    }
                }
            }
            else
            {
                pick_up_item(items, inventory, player, log);
            }
        }
        else if(is_input_valid(&input->Key_IdentifyOrEnchantItem))
        {
            Item *item = get_inventory_slot_item(inventory, inventory->pos);
            if(item)
            {
                if(inventory->item_use_type == ItemUseType_Identify)
                {
                    if(!item->is_identified)
                    {
                        item->is_identified = true;
                        
                        InventorySlot slot = {inventory->use_item_src_index, inventory->slots[slot.index]};
                        remove_item_from_inventory_and_game(slot, player, log, inventory);
                        reset_inventory_item_use(inventory);
                    }
                }
                else if(inventory->item_use_type == ItemUseType_EnchantWeapon)
                {
                    if(item->type == ItemType_Weapon)
                    {
                        u32 chance = random_number(&game->random, 1, 4);
                        switch(chance)
                        {
                            case 1: log_text(log, "The %s glows blue for a moment..", item_id_text(item->id)); break;
                            case 2: log_text(log, "The %s seems sharper than before..", item_id_text(item->id)); break;
                            case 3: log_text(log, "The %s vibrates slightly..", item_id_text(item->id)); break;
                            case 4: log_text(log, "The %s starts shimmering..", item_id_text(item->id)); break;
                            
                            invalid_default_case;
                        }
                        
                        ++item->enchantment_level;
                        
                        InventorySlot slot = {inventory->use_item_src_index, inventory->slots[slot.index]};
                        remove_item_from_inventory_and_game(slot, player, log, inventory);
                        reset_inventory_item_use(inventory);
                    }
                }
                else if(inventory->item_use_type == ItemUseType_EnchantArmour)
                {
                    if(item->type == ItemType_Armour)
                    {
                        u32 chance = random_number(&game->random, 1, 3);
                        switch(chance)
                        {
                            case 1: log_text(log, "The %s glows white for a moment..", item_id_text(item->id)); break;
                            case 2: log_text(log, "The %s looks sturdier than before..", item_id_text(item->id)); break;
                            case 3: log_text(log, "The %s feels warm for a moment..", item_id_text(item->id)); break;
                            case 4: log_text(log, "The %s feels heavier than before..", item_id_text(item->id)); break;
                            
                            invalid_default_case;
                        }
                        
                        ++item->enchantment_level;
                        
                        InventorySlot slot = {inventory->use_item_src_index, inventory->slots[slot.index]};
                        remove_item_from_inventory_and_game(slot, player, log, inventory);
                        reset_inventory_item_use(inventory);
                    }
                }
            }
        }
        else if(is_input_valid(&input->Key_MoveItem))
        {
            if(inventory->is_open &&
               (!inventory->item_use_type || inventory->item_use_type == ItemUseType_Move))
            {
                if(inventory->item_use_type == ItemUseType_Move)
                {
                    // We are moving the item so the current inventory
                    // pos is assumed to be the destination.
                    inventory->use_item_dest_index = get_inventory_slot_index(inventory->pos);
                    
                    if(inventory->use_item_src_index != inventory->use_item_dest_index)
                    {
                        if(inventory->slots[inventory->use_item_dest_index])
                        {
                            // Swap the item that we're moving and the item
                            // at the destination of the move.
                            Item *temp = inventory->slots[inventory->use_item_dest_index];
                            
                            inventory->slots[inventory->use_item_dest_index] = inventory->slots[inventory->use_item_src_index];
                            inventory->slots[inventory->use_item_src_index] = temp;
                        }
                        else
                        {
                            // Nothing to swap, so just move the item.
                            inventory->slots[inventory->use_item_dest_index] = inventory->slots[inventory->use_item_src_index];
                            inventory->slots[inventory->use_item_src_index] = 0;
                        }
                    }
                    
                    reset_inventory_item_use(inventory);
                }
                else
                {
                    // If there is an item then start moving it.
                    u32 slot_index = get_inventory_slot_index(inventory->pos);
                    if(inventory->slots[slot_index])
                    {
                        inventory->item_use_type = ItemUseType_Move;
                        inventory->use_item_src_index = slot_index;
                    }
                }
            }
        }
        else if(is_input_valid(&input->Key_AscendOrDescend))
        {
            if(!inventory->is_open)
            {
                if(is_tile_id(dungeon->tiles, player->pos, Tile_StonePathUp) ||
                   is_tile_id(dungeon->tiles, player->pos, Tile_Escape))
                {
                    game->mode = GameMode_Quit;
                }
                else if(is_tile_id(dungeon->tiles, player->pos, Tile_StonePathDown))
                {
                    if(dungeon->level < MAX_DUNGEON_LEVEL)
                    {
                        create_dungeon(game, dungeon, player, log, entities, items, consumable_data, enemy_levels);
                        log_text(log, "You descend further.. Level %u.", dungeon->level);
                        update_fov(dungeon, player);
                    }
                    else
                    {
                        game->mode = GameMode_Quit;
                    }
                }
                else
                {
                    log_text(log, "You don't see a path here.");
                }
            }
        }
        else if(is_input_valid(&input->Key_Wait))
        {
            if(!inventory->is_open)
            {
                should_update_player = true;
                player->action_speed = 1.0f;
            }
        }
        
        if(should_update_player)
        {
#if MOONBREATH_SLOW
            if(debug_traversable)
            {
                if(pos_in_dungeon(dungeon, player->new_pos))
                {
                    move_entity(dungeon, player->new_pos, player);
                }
            }
            else
#endif
            
            if(!V2u_equal(player->pos, player->new_pos) &&
               is_tile_occupied(dungeon->tiles, player->new_pos))
            {
                for(u32 entity_index = 1; entity_index < MAX_ENTITIES; ++entity_index)
                {
                    Entity *enemy = &entities[entity_index];
                    if(V2u_equal(player->new_pos, enemy->pos))
                    {
                        u32 player_hit_chance = 15 + (player->dexterity / 2);
                        player_hit_chance += player->p.accuracy;
                        
#if 0
                        // Player Hit Test
                        printf("\nPlayer Hit Chance: %u\n", player_hit_chance);
                        printf("Target Entity Evasion: %u\n", enemy->evasion);
                        
                        u32 hit_count = 0;
                        u32 miss_count = 0;
                        
                        for(u32 index = 0; index < 100; ++index)
                        {
                            u32 roll = random_number(&game->random, 0, player_hit_chance);
                            if(entity_will_hit(&game->random, player_hit_chance, enemy->evasion))
                            {
                                ++hit_count;
                            }
                            else
                            {
                                ++miss_count;
                            }
                        }
                        
                        printf("Hit Count: %u (%.01f%%)\n", hit_count, (f32)hit_count / 100);
                        printf("Miss Count: %u (%.01f%%)\n\n", miss_count, (f32)miss_count / 100);
#else
                        if(entity_will_hit(&game->random, player_hit_chance, enemy->evasion))
                        {
                            printf("player->damage: %u\n", player->damage);
                            attack_entity(game, dungeon, log, inventory, player, enemy);
                        }
                        else
                        {
                            log_text(log, "%sYour attack misses.", start_color(Color_LightGray));
                        }
                        
                        enemy->e.in_combat = true;
#endif
                        
                        player->action_speed = player->p.attack_speed;
                        break;
                    }
                }
            }
            else
            {
                if(is_tile_id(dungeon->tiles, player->new_pos, Tile_StoneDoorClosed))
                {
                    set_tile_id(dungeon->tiles, player->new_pos, Tile_StoneDoorOpen);
                    player->action_speed = 1.0f;
                }
                else if(is_tile_traversable(dungeon->tiles, player->new_pos))
                {
                    move_entity(dungeon, player->new_pos, player);
                    player->action_speed = 1.0f;
                }
            }
            
            // Changing the new position must be based on the current position.
            player->new_pos = player->pos;
            game->time += player->action_speed;
        }
    }
    
    if(player->action_speed)
    {
        update_pathfind_map(dungeon, player);
        update_fov(dungeon, player);
        
        // Update Enemies
        for(u32 entity_index = 1; entity_index < MAX_ENTITIES; ++entity_index)
        {
            Entity *enemy = &entities[entity_index];
            if(enemy->type == EntityType_Enemy)
            {
                enemy->e.wait_timer += player->action_speed;
                u32 action_count = (u32)(enemy->e.wait_timer / enemy->action_speed);
#if 0
                printf("player->action_speed: %.1f\n", player->action_speed);
                printf("wait_timer: %.1f\n", enemy->e.wait_timer);
                printf("action_count: %u\n\n", action_count);
#endif
                
                if(action_count)
                {
                    enemy->e.wait_timer = 0.0f;
                    
                    while(action_count--)
                    {
                        if(enemy->e.in_combat)
                        {
                            if(player->pos.x < enemy->pos.x)
                            {
                                enemy->e.is_flipped = true;
                            }
                            else
                            {
                                enemy->e.is_flipped = false;
                            }
                            
                            v2u next_pos = next_pathfind_pos(dungeon, player, enemy);
                            if(V2u_equal(next_pos, player->pos))
                            {
                                if(entity_will_hit(&game->random, 40, player->evasion))
                                {
                                    attack_entity(game, dungeon, log, inventory, enemy, player);
                                }
                                else
                                {
                                    log_text(log, "%sYou dodge the attack.", start_color(Color_LightGray));
                                }
                            }
                            else
                            {
                                enemy->new_pos = next_pos;
                            }
                        }
                        else
                        {
#if MOONBREATH_SLOW
                            if(!debug_fov && tile_is_seen(dungeon->tiles, enemy->pos))
#else
                            if(tile_is_seen(dungeon->tiles, enemy->pos))
#endif
                            {
                                //enemy->e.in_combat = true;
                                update_enemy_ai(game, enemy);
                            }
                            else
                            {
                                update_enemy_ai(game, enemy);
                            }
                        }
                        
                        // Calling move_entity() will set the pos of the entity to new_pos.
                        // Before that happens we save the pos into pos_save_for_ghost
                        // because the code that renders the enemy ghosts needs it.
                        enemy->e.pos_save_for_ghost = enemy->pos;
                        
                        if(is_tile_traversable_and_not_occupied(dungeon->tiles, enemy->new_pos))
                        {
                            move_entity(dungeon, enemy->new_pos, enemy);
                        }
                    }
                }
            }
        }
        
        // Update Player Effects
        for(u32 index = 0; index < EffectType_Count; ++index)
        {
            StatusEffect *effect = &player->p.effects[index];
            if(effect->is_enabled)
            {
                --effect->duration;
                
                if(effect->duration)
                {
                    if(index == EffectType_Poison)
                    {
                        if((s32)(player->hp - effect->value) <= 0)
                        {
                            player->hp = 0;
                        }
                        else
                        {
                            player->hp -= effect->value;
                        }
                    }
                }
                else
                {
                    switch(index)
                    {
                        case EffectType_Might:
                        {
                            log_text(log, "You don't feel as powerful anymore..");
                            player->strength -= effect->value;
                        } break;
                        
                        case EffectType_Wisdom:
                        {
                            log_text(log, "You don't feel as knowledgeable anymore..");
                            player->intelligence -= effect->value;
                        } break;
                        
                        case EffectType_Agility:
                        {
                            log_text(log, "Your body feels less nimble..");
                            player->dexterity -= effect->value;
                        } break;
                        
                        case EffectType_Fortitude:
                        {
                            log_text(log, "You don't feel as strong anymore..");
                            player->p.defence -= effect->value;
                        } break;
                        
                        case EffectType_Resistance:
                        {
                            log_text(log, "You don't feel as resistive anymore..");
                        } break;
                        
                        case EffectType_Focus:
                        {
                            log_text(log, "You don't feel as attentive anymore..");
                            player->evasion -= effect->value;
                        } break;
                        
                        case EffectType_Flight:
                        {
                            log_text(log, "You don't feel light anymore..");
                        } break;
                        
                        case EffectType_Decay:
                        {
                            log_text(log, "You don't feel impaired anymore..");
                            player->strength += effect->value;
                            player->intelligence += effect->value;
                            player->dexterity += effect->value;
                        } break;
                        
                        case EffectType_Weakness:
                        {
                            log_text(log, "You don't feel weak anymore..");
                            player->p.defence += effect->value;
                        } break;
                        
                        case EffectType_Poison:
                        {
                            log_text(log, "You don't feel sick anymore..");
                        } break;
                        
                        case EffectType_Confusion:
                        {
                            log_text(log, "You don't feel confused anymore..");
                        } break;
                        
                        invalid_default_case;
                    }
                    
                    end_player_status_effect(effect);
                }
            }
        }
    }
}

internal void
render_entities(GameState *game,
                Dungeon *dungeon,
                Entity *player,
                Entity *entities,
                Inventory *inventory,
                Assets *assets)
{
    // Render Player
    v4u src = tile_rect(player->tile);
    v4u dest = game_dest(game, player->pos);
    SDL_RenderCopy(game->renderer, assets->sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
    
    // Render Player Items
    for(u32 index = 1; index < ItemSlot_Count; ++index)
    {
        for(u32 inventory_slot_index = 0; inventory_slot_index < array_count(inventory->slots); ++inventory_slot_index)
        {
            Item *item = inventory->slots[inventory_slot_index];
            if(item &&
               item->is_equipped &&
               (item->slot == index))
            {
                v4u src = tile_rect(item->tile);
                SDL_RenderCopy(game->renderer, assets->wearable_item_tileset.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest);
                break;
            }
        }
    }
    
    // Render Enemies
    for(u32 entity_index = 1; entity_index < MAX_ENTITIES; ++entity_index)
    {
        Entity *enemy = &entities[entity_index];
        if(enemy->type == EntityType_Enemy)
        {
            if(tile_is_seen(dungeon->tiles, enemy->pos))
            {
                enemy->e.has_been_seen = true;
                enemy->e.is_ghost_enabled = false;
                
                v4u src = tile_rect(enemy->tile);
                v4u dest = game_dest(game, enemy->pos);
                SDL_RenderCopyEx(game->renderer, assets->sprite_sheet.tex, (SDL_Rect *)&src, (SDL_Rect *)&dest, 0, 0, enemy->e.is_flipped);
                
                // Render Enemy HP Bar
                if(enemy->e.in_combat &&
                   enemy->hp != enemy->max_hp)
                {
                    // HP Bar Outside
                    set_render_color(game, Color_Black);
                    v4u hp_bar_outside = {dest.x, dest.y + 33, 32, 4};
                    SDL_RenderDrawRect(game->renderer, (SDL_Rect *)&hp_bar_outside);
                    
                    // HP Bar Inside
                    set_render_color(game, Color_DarkRed);
                    u32 hp_bar_inside_w = ratio(enemy->hp, enemy->max_hp, 30);
                    v4u hp_bar_inside = {dest.x + 1, dest.y + 34, hp_bar_inside_w, 2};
                    SDL_RenderFillRect(game->renderer, (SDL_Rect *)&hp_bar_inside);
                }
            }
            else
            {
                if(enemy->e.has_been_seen)
                {
                    if(enemy->e.is_ghost_enabled)
                    {
                        if(tile_is_seen(dungeon->tiles, enemy->e.ghost_pos))
                        {
                            enemy->e.has_been_seen = false;
                            enemy->e.is_ghost_enabled = false;
                        }
                        else
                        {
                            v4u src = tile_rect(enemy->tile);
                            v4u dest = game_dest(game, enemy->e.ghost_pos);
                            render_texture_half_color(game->renderer, assets->sprite_sheet.tex, src, dest);
                        }
                    }
                    else
                    {
                        // If enemy pos is seen then enemy ghost is placed on new enemy pos.
                        // This means that the enemy moved.
                        
                        // else enemy pos is not seen so enemy ghost is placed on enemy pos.
                        // This means that the player moved.
                        if(tile_is_seen(dungeon->tiles, enemy->e.pos_save_for_ghost))
                        {
                            enemy->e.ghost_pos = enemy->new_pos;
                        }
                        else
                        {
                            enemy->e.ghost_pos = enemy->e.pos_save_for_ghost;
                        }
                        
                        enemy->e.is_ghost_enabled = true;
                        enemy->e.is_ghost_flipped = enemy->e.is_flipped;
                    }
                }
            }
        }
    }
}

internal void
add_player_entity(GameState *game, Entity *player, Item *items, Inventory *inventory)
{
    player->type = EntityType_Player;
    
    strcpy(player->name, "Name");
    player->max_hp = 20;
    player->hp = 10;
    player->w = player->h = 32;
    
    player->strength = 10;
    player->intelligence = 10;
    player->dexterity = 10;
    
    player->damage = 1;
    player->evasion = 10;
    player->p.accuracy = 2;
    player->p.attack_speed = 1.0f;
    
    player->p.fov = 6;
    
#if 1
    { // Give the player their starting item.
        add_weapon_item(game, items, ItemID_Sword, ItemRarity_Common, player->pos.x, player->pos.y);
        
        Item *item = get_item_on_pos(player->pos, items);
        item->enchantment_level = 0;
        item->is_identified = true;
        item->is_cursed = false;
        
        add_item_to_inventory(item, inventory);
        equip_item(item, player);
    }
#endif
}

internal void
add_enemy_entity(Entity *entities, Dungeon *dungeon, u32 *enemy_levels, EntityID id, u32 x, u32 y)
{
    assert(id);
    
    for(u32 entity_index = 1; entity_index < MAX_ENTITIES; ++entity_index)
    {
        Entity *enemy = &entities[entity_index];
        if(!enemy->type)
        {
            enemy->id = id;
            enemy->type = EntityType_Enemy;
            set_tile_occupied(dungeon->tiles, V2u(x, y), true);
            
            switch(id)
            {
                case EntityID_Rat:
                {
                    strcpy(enemy->name, "Rat");
                    enemy->max_hp = enemy->hp = 15;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(1, 1);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Snail:
                {
                    strcpy(enemy->name, "Snail");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(0, 1);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Slime:
                {
                    strcpy(enemy->name, "Slime");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(1, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_green_blooded = true;
                } break;
                
                case EntityID_GiantSlime:
                {
                    strcpy(enemy->name, "Giant Slime");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(2, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_green_blooded = true;
                } break;
                
                case EntityID_Skeleton:
                {
                    strcpy(enemy->name, "Skeleton");
                    enemy->max_hp = enemy->hp = 25;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(3, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case EntityID_SkeletonWarrior:
                {
                    strcpy(enemy->name, "Skeleton Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(4, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case EntityID_OrcWarrior:
                {
                    strcpy(enemy->name, "Orc Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(5, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_CaveBat:
                {
                    strcpy(enemy->name, "Cave Bat");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(6, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Python:
                {
                    strcpy(enemy->name, "Python");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(7, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Kobold:
                {
                    strcpy(enemy->name, "Kobold");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(8, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Ogre:
                {
                    strcpy(enemy->name, "Ogre");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(9, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Tormentor:
                {
                    strcpy(enemy->name, "Tormentor");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(10, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case EntityID_Imp:
                {
                    strcpy(enemy->name, "Imp");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(11, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_GiantDemon:
                {
                    strcpy(enemy->name, "Giant Demon");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(12, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Hellhound:
                {
                    strcpy(enemy->name, "Hellhound");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(13, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_UndeadElfWarrior:
                {
                    strcpy(enemy->name, "Undead Elf Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(14, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case EntityID_Assassin:
                {
                    strcpy(enemy->name, "Assassin");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(15, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Goblin:
                {
                    strcpy(enemy->name, "Goblin");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(16, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_GoblinWarrior:
                {
                    strcpy(enemy->name, "Goblin Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(17, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Viper:
                {
                    strcpy(enemy->name, "Viper");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(18, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_ScarletKingsnake:
                {
                    strcpy(enemy->name, "Scarlet Kingsnake");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(19, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Dog:
                {
                    strcpy(enemy->name, "Dog");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(20, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Wolf:
                {
                    strcpy(enemy->name, "Wolf");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(21, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_GreenMamba:
                {
                    strcpy(enemy->name, "Green Mamba");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(22, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_FloatingEye:
                {
                    strcpy(enemy->name, "Floating Eye");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(23, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Devourer:
                {
                    strcpy(enemy->name, "Devourer");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(24, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Ghoul:
                {
                    strcpy(enemy->name, "Ghoul");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(25, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_made_of_bone = true;
                } break;
                
                case EntityID_Cyclops:
                {
                    strcpy(enemy->name, "Cyclops");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(26, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_DwarwenWarrior:
                {
                    strcpy(enemy->name, "Dwarwen Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(27, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_BlackKnight:
                {
                    strcpy(enemy->name, "Black Knight");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(28, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                } break;
                
                case EntityID_CursedBlackKnight:
                {
                    strcpy(enemy->name, "Cursed Black Knight");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(29, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                } break;
                
                case EntityID_Treant:
                {
                    strcpy(enemy->name, "Treant");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(30, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                } break;
                
                case EntityID_Minotaur:
                {
                    strcpy(enemy->name, "Minotaur");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(31, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_CentaurWarrior:
                {
                    strcpy(enemy->name, "Centaur Warrior");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(32, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Centaur:
                {
                    strcpy(enemy->name, "Centaur");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(33, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_FrostShards:
                {
                    strcpy(enemy->name, "Frost Shards");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(34, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                } break;
                
                case EntityID_FrostWalker:
                {
                    strcpy(enemy->name, "Frost Walker");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(35, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                } break;
                
                case EntityID_Griffin:
                {
                    strcpy(enemy->name, "Griffin");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(36, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                case EntityID_Spectre:
                {
                    strcpy(enemy->name, "Spectre");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(37, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                } break;
                
                case EntityID_FlyingSkull:
                {
                    strcpy(enemy->name, "Flying Skull");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(38, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                } break;
                
                case EntityID_BrimstoneImp:
                {
                    strcpy(enemy->name, "Brimstone Imp");
                    enemy->max_hp = enemy->hp = 4;
                    enemy->new_pos = enemy->pos = V2u(x, y);
                    enemy->w = enemy->h = 32;
                    enemy->tile = V2u(39, 0);
                    
                    enemy->damage = 2;
                    enemy->evasion = 4;
                    enemy->action_speed = 1.0f;
                    
                    enemy->e.level = enemy_levels[id];
                    enemy->e.is_red_blooded = true;
                } break;
                
                invalid_default_case;
            }
            
            return;
        }
    }
    
    assert(false);
}