// NOTE(Rami): WORKS, REMOVE THIS NOTE LATER
internal void
render_items()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].item_id && !items[i].in_inventory)
    {
      SDL_Rect src = {tile_mul(items_info[items[i].item_id - 1].tile), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dest = {tile_mul(items[i].x) - game.camera.x, tile_mul(items[i].y) - game.camera.y, TILE_SIZE, TILE_SIZE};

      if(is_lit(v2(items[i].x, items[i].y)))
      {
        v4_t color = get_color_for_lighting_value(v2(items[i].x, items[i].y));
        SDL_SetTextureColorMod(assets.textures[item_tileset_tex], color.r, color.g, color.b);
        SDL_RenderCopy(game.renderer, assets.textures[item_tileset_tex], &src, &dest);
      }
    }
  }
}

// NOTE(Rami): WORKS, REMOVE THIS NOTE LATER
// NOTE(Rami): When you drop multiple items they need to be picked up in the reverse order
internal void
drop_item()
{
  if(player.inventory.item_count)
  {
    for(i32 i = 0; i < ITEM_COUNT; i++)
    {
      if(items[i].in_inventory)
      {
        if(items[i].unique_id ==
           player.inventory.slots[player.inventory.item_selected - 1].unique_id)
        {
          items[i].in_inventory = false;
          items[i].is_equipped = false;
          items[i].x = player.entity.x;
          items[i].y = player.entity.y;

          player.inventory.slots[player.inventory.item_selected - 1] =
          (item_t){id_none, 0, false, false, 0, 0};

          for(i32 i = 1; i < INVENTORY_SLOT_COUNT; i++)
          {
            if(player.inventory.slots[i].item_id &&
              !player.inventory.slots[i - 1].item_id)
            {
              player.inventory.slots[i - 1] = player.inventory.slots[i];
              player.inventory.slots[i] = (item_t){id_none, 0, false, false, 0, 0};
            }
          }

          player.inventory.item_count--;
          add_console_message("You drop the %s", RGBA_COLOR_WHITE_S,
                              items_info[items[i].item_id - 1].name);
          break;
        }
      }
    }
  }
  else
  {
    add_console_message("You find nothing in your inventory to drop", RGBA_COLOR_WHITE_S);
  }
}

internal void
remove_item()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the correct item from the items array,
    // its .in_inventory value needs to be zero
    if(player.inventory.slots[player.inventory.item_selected].unique_id == items[i].unique_id && !items[i].in_inventory)
    {
      // remove the item data from inventory
      // memset(&player.inventory.slots[player.inventory.item_selected], 0, sizeof(item_t));
      // NOTE(Rami): REMOVE AFTER VERIFYING THE MEMSET WORKS
      player.inventory.slots[player.inventory.item_selected].item_id = id_none;
      player.inventory.slots[player.inventory.item_selected].unique_id = 0;
      player.inventory.slots[player.inventory.item_selected].in_inventory = false;
      player.inventory.slots[player.inventory.item_selected].is_equipped = false;
      player.inventory.slots[player.inventory.item_selected].x = 0;
      player.inventory.slots[player.inventory.item_selected].y = 0;
      break;
    }
  }

  // count holds how many items we have to move item data
  i32 count = INVENTORY_SLOT_COUNT - player.inventory.item_selected - 1;

  // if count is over the amount of items we have then clamp it
  if(count > player.inventory.item_count)
  {
    count = player.inventory.item_count - player.inventory.item_selected - 1;
  }

  // move the item data according to the value of count
  for(i32 i = 0; i != count; i++)
  {
    player.inventory.slots[player.inventory.item_selected + i] = player.inventory.slots[player.inventory.item_selected + i + 1];
  }

  // after moving the last item remove the data from the slot you moved it from
  // memset(&player.inventory.slots[player.inventory.item_selected + count], 0, sizeof(item_t));
  // NOTE(Rami): REMOVE AFTER VERIFYING THE MEMSET WORKS
  player.inventory.slots[player.inventory.item_selected + count].item_id = id_none;
  player.inventory.slots[player.inventory.item_selected + count].unique_id = 0;
  player.inventory.slots[player.inventory.item_selected + count].in_inventory = false;
  player.inventory.slots[player.inventory.item_selected + count].is_equipped = false;
  player.inventory.slots[player.inventory.item_selected + count].x = 0;
  player.inventory.slots[player.inventory.item_selected + count].y = 0;
}

internal void
consume_item()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    // find the item with the same unique id as the item were on in the inventory
    if(items[i].unique_id == player.inventory.slots[player.inventory.item_selected].unique_id)
    {
      // only proceed if the item is consumable
      if(items_info[items[i].item_id - 1].item_type == type_consume)
      {
        // if the player is already at max hp
        if(player.entity.hp >= player.max_hp)
        {
          // NOTE(Rami): or alternatively "You drink the potion and feel no difference" + the item is gone
          add_console_message("You do not feel like drinking this right now", RGBA_COLOR_WHITE_S);
          break;
        }

        // NOTE(Rami): add other potion types like MEDIUM_HEATH_POTION, GREATER HEALTH_POTION...
        // increase hp amount depending on the potion
        if(items[i].item_id == id_lesser_health_potion)
        {
          // apply hp increase
          player.entity.hp += items_info[items[i].item_id - 1].hp_healed;

          // if it goes over the players maximum hp then clamp it
          if(player.entity.hp >= player.max_hp)
          {
            player.entity.hp = player.max_hp;
          }

          add_console_message("You drink the potion and feel slighty better", RGBA_COLOR_BLUE_S);
          remove_item();
          break;
        }
      }
    }
  }
}

// NOTE(Rami):
internal void
toggle_equipped_item()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {



    // if(player.inventory.slots[i].is_equipped)
    // {
    //   player.inventory.slots[i].is_equipped = false;
    // }
    // else
    // {
    //   player.inventory.slots[i].is_equipped = true;
    // }
  }

  // for(i32 i = 0; i < ITEM_COUNT; i++)
  // {
  //   // find the item with the same unique id as the item were on in the inventory
  //   if(items[i].unique_id == player.inventory.slots[player.inventory.item_selected].unique_id)
  //   {
  //     // only proceed if the item is equippable
  //     if(items_info[items[i].item_id - 1].item_type == type_equip)
  //     {
  //       // if equipped
  //       if(items[i].is_equipped)
  //       {
  //         // unequip it
  //         items[i].is_equipped = false;
  //         add_console_message("You unequip the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id - 1].name);
  //       }
  //       // if unequipped
  //       else
  //       {
  //         // equip it
  //         items[i].is_equipped = true;
  //         add_console_message("You equip the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id - 1].name);
  //       }
  //       break;
  //     }
  //   }
  // }
}

// NOTE(Rami): WORKS, REMOVE THIS NOTE LATER
internal void
add_game_item(item_id_e item_id, v2_t pos)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].item_id == id_none)
    {
      debug("Item added\n");

      items[i].item_id = item_id;
      items[i].in_inventory = false;
      items[i].is_equipped = false;
      items[i].x = pos.x;
      items[i].y = pos.y;
      return;
    }
  }

  debug("No free item slots");
}

// NOTE(Rami): WORKS, REMOVE THIS NOTE LATER
internal void
pickup_item()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(!items[i].in_inventory)
    {
      if(v2_equal(v2(items[i].x, items[i].y), v2(player.entity.x, player.entity.y)))
      {
        for(i32 inventory_i = 0; inventory_i < INVENTORY_SLOT_COUNT; inventory_i++)
        {
          if(!player.inventory.slots[inventory_i].item_id)
          {
            items[i].in_inventory = true;
            player.inventory.slots[inventory_i] = items[i];
            add_console_message("You pickup the %s", RGBA_COLOR_WHITE_S, items_info[items[i].item_id - 1].name);

            return;
          }
        }

        add_console_message("Your inventory is full right now", RGBA_COLOR_WHITE_S);
      }
    }
  }

  add_console_message("You find nothing to pick up", RGBA_COLOR_WHITE_S);
}