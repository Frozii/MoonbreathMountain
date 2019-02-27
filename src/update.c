i32 attack_entity(entity_t *attacker, entity_t *target)
{
  target->hp -= attacker->damage;
  if(target->hp <= 0)
  {
    return 1;
  }

  return 0;
}

void update_input()
{
  if(global_state.key_pressed == SDLK_ESCAPE)
  {
    // NOTE(Rami): Delete later.
    debug("SDLK_ESCAPE\n");
    global_state.game_is_running = false;
  }

  /* - IN INVENTORY - */

  else if(player->inventory_display)
  {
    switch(global_state.key_pressed)
    {
      case SDLK_k:
      {
        // if the highlight index can't go any lower
        // meaning that this is the top most item we can be on
        if(player->inventory_item_selected - 1 < 0)
        {
          // then if we have more than one item in the inventory
          if(player->inventory_item_count > 0)
          {
            // set the highlight index to be the last item
            player->inventory_item_selected = player->inventory_item_count - 1;
          }
        }
        // else we can substract because we're not on the top most item
        else
        {
          (player->inventory_item_selected)--;
        }
      } break;

      case SDLK_j:
      {
        // if the highlight index can't go any higher
        // meaning that this is the bottom item we can be on
        if(player->inventory_item_selected + 1 > player->inventory_item_count - 1)
        {
          // set the highlight index to the first item
          player->inventory_item_selected = player->inventory_item_count = 0;
        }
        // else we can add because we're not on the most bottom item
        else
        {
          (player->inventory_item_selected)++;
        }
      } break;

      case SDLK_i:
      {
        // do not display inventory anymore
        // reset highlight index
        player->inventory_display = false;
        player->inventory_item_selected = 0;
      } break;

      case SDLK_d:
      {
        drop_item();

        // if the bottom item of the inventory got dropped, make the highlighter go up by one
        if(player->inventory_item_selected + 1 == player->inventory_item_count)
        {
          if(player->inventory_item_selected - 1 >= 0)
          {
            player->inventory_item_selected--;
          }
        }
      } break;

      case SDLK_e:
      {
        equip_or_unequip_item(player);
      } break;

      case SDLK_c:
      {
        consume_item(player);

        // if the bottom item of the inventory got dropped, make the highlighter go up by one
        if(player->inventory_item_selected + 1 == player->inventory_item_count)
        {
          if(player->inventory_item_selected - 1 >= 0)
          {
            player->inventory_item_selected--;
          }
        }
      } break;
    }
  }

  /* - NOT IN INVENTORY - */

  else if(!player->inventory_display)
  {
    switch(global_state.key_pressed)
    {
      // Up
      case SDLK_k:
      {
        player->entity->new_pos.x = player->entity->pos.x;
        player->entity->new_pos.y = player->entity->pos.y - 1;
        global_state.turn_changed = true;
      } break;

      // Down
      case SDLK_j:
      {
        player->entity->new_pos.x = player->entity->pos.x;
        player->entity->new_pos.y = player->entity->pos.y + 1;
        global_state.turn_changed = true;
      } break;

      // Left
      case SDLK_h:
      {
        player->entity->new_pos.x = player->entity->pos.x - 1;
        player->entity->new_pos.y = player->entity->pos.y;
        global_state.turn_changed = true;
      } break;

      // Right
      case SDLK_l:
      {
        player->entity->new_pos.x = player->entity->pos.x + 1;
        player->entity->new_pos.y = player->entity->pos.y;
        global_state.turn_changed = true;
      } break;

      // Left Up
      case SDLK_y:
      {
        player->entity->new_pos.x = player->entity->pos.x - 1;
        player->entity->new_pos.y = player->entity->pos.y - 1;
        global_state.turn_changed = true;
      } break;

      // Right Up
      case SDLK_u:
      {
        player->entity->new_pos.x = player->entity->pos.x + 1;
        player->entity->new_pos.y = player->entity->pos.y - 1;
        global_state.turn_changed = true;
      } break;

      // Left Down
      case SDLK_b:
      {
        player->entity->new_pos.x = player->entity->pos.x - 1;
        player->entity->new_pos.y = player->entity->pos.y + 1;
        global_state.turn_changed = true;
      } break;

      // Right Down
      case SDLK_n:
      {
        player->entity->new_pos.x = player->entity->pos.x + 1;
        player->entity->new_pos.y = player->entity->pos.y + 1;
        global_state.turn_changed = true;
      } break;

      // NOTE(Rami): advance one turn, for testing
      case SDLK_t:
      {
        global_state.turn_changed = true;
      } break;

      case SDLK_i:
      {
        player->inventory_display = true;
      } break;

      case SDLK_COMMA:
      {
        add_inventory_item(player);
      } break;

      case SDLK_d:
      {
        if(is_tile_close((iv2_t){player->entity->pos.x, player->entity->pos.y}, tile_path_down))
        {
          // NOTE(Rami): Enable this later.
          // add_console_msg("You travel deeper into the mountain..", HEX_COLOR_WHITE);
          // generate_level(level, LEVEL_SIZE, LEVEL_SIZE, LEVEL_SIZE, 2);
          // global_state.turn_changed = true;
        }
      } break;

      case SDLK_a:
      {
        if(is_tile_close((iv2_t){player->entity->pos.x, player->entity->pos.y}, tile_path_up))
        {
          debug("You flee from the mountain..\n");
          global_state.game_is_running = false;
        }
      } break;
    }
  }

  global_state.key_pressed = 0;
}

void update_events()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      // NOTE(Rami):
      debug("SDL_QUIT\n");
      global_state.game_is_running = false;
    }
    // NOTE(Rami): 
    // else if(event.type == SDL_KEYDOWN && !event.key.repeat)
    else if(event.type == SDL_KEYDOWN)
    {
      global_state.key_pressed = event.key.keysym.sym;
    }
  }
}

void update_camera()
{
  global_state.camera.x = tile_mul(player->entity->pos.x) - (global_state.camera.w / 2);
  global_state.camera.y = (tile_mul(player->entity->pos.y) + (player->entity->aspect.h / 2)) - (global_state.camera.h / 2);

  if(global_state.camera.x < 0)
  {
    global_state.camera.x = 0;
  }

  if(global_state.camera.y < 0)
  {
    global_state.camera.y = 0;
  }

  if(global_state.camera.x >= LEVEL_WIDTH_IN_PIXELS - global_state.camera.w)
  {
    global_state.camera.x = LEVEL_WIDTH_IN_PIXELS - global_state.camera.w;
  }

  if(global_state.camera.y >= LEVEL_HEIGHT_IN_PIXELS - global_state.camera.h)
  {
    global_state.camera.y = LEVEL_HEIGHT_IN_PIXELS - global_state.camera.h;
  }
}

// void update_lighting()
// {
//   // set all elements as not visible
//   for(int y = 0; y < LEVEL_SIZE; y++)
//   {
//     for(int x = 0; x < LEVEL_SIZE; x++)
//     {
//       fov[y * LEVEL_SIZE + x] = 0;
//     }
//   }

//   // hardcoded lighting
//   #if 0
//   // set the elements inside the players field of view visible
//   for(int y = (player.y / TILE_SIZE) - player.fov; y < (player.y / TILE_SIZE) + player.fov; y++)
//   {
//     for(int x = (player.x / TILE_SIZE) - player.fov; x < (player.x / TILE_SIZE) + player.fov; x++)
//     {
//       fov[y * LEVEL_SIZE + x] = 255;
//     }
//   }
//   #endif

//   // raycasted lighting
//   #if 0
//   for(int angle = 0; angle < 360; angle++)
//   {
//     // calculate the amount for the ray to progress
//     float dx = 0.1 * cos(angle);
//     float dy = 0.1 * sin(angle);

//     // set the ray to begin from the players location
//     float fx = player.x;
//     float fy = player.y;

//     for(;;)
//     {
//       // add to the rays location the amount we calculated
//       fx += dx;
//       fy += dy;

//       float dist = distance(player.x + 16, player.y + 16, fx, fy);
//       //int idist = dist / 32;

//       //debug("dist_between: %d\n", idist);

//       // if the ray is over the players view distance then stop the ray
//       if(dist > to_pixels(player->fov))
//       {
//         break;
//       }

//       // convert to array valid values
//       int ifx = fx / 32;
//       int ify = fy / 32;

//       // make sure the ray isn't going off the level
//       if(ifx >= 0 && ifx <= LEVEL_SIZE && ify >= 0 && ify <= LEVEL_SIZE)
//       {
//         //fov[ify * LEVEL_SIZE + ifx] = 255 * ((6 - idist) / 6);
//         fov[ify * LEVEL_SIZE + ifx] = 255;


//         // if we hit something we can't see through then stop the ray
//         if(level[ify * LEVEL_SIZE + ifx] == TILE_WALL_STONE || level[ify * LEVEL_SIZE + ifx] == TILE_DOOR_CLOSED)
//         {
//           break;
//         }
//       }
//     }

//     fov[(player.y / TILE_SIZE) * LEVEL_SIZE + ((player.x / TILE_SIZE) - 1)] = 40;
//   }
//   #endif
// }