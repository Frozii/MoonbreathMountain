internal void
update_input()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      game.state = state_quit;
    }
  #if MOONBREATH_DEBUG
    else if(event.type == SDL_KEYDOWN)
  #else
    else if(event.type == SDL_KEYDOWN && !event.key.repeat)
  #endif
    {
      keyboard.keys[event.key.keysym.scancode] = 1;
      player_keypress(event.key.keysym.scancode);
    }
    else if(event.type == SDL_KEYUP)
    {
      keyboard.keys[event.key.keysym.scancode] = 0;
    }
  }
}

internal void
update_camera()
{
  game.camera.x = tile_mul(player.x) - (game.camera.w / 2);
  game.camera.y = (tile_mul(player.y) + (player.h / 2)) - (game.camera.h / 2);

  if(game.camera.x < 0)
  {
    game.camera.x = 0;
  }

  if(game.camera.y < 0)
  {
    game.camera.y = 0;
  }

  if(game.camera.x >= LEVEL_PIXEL_WIDTH - game.camera.w)
  {
    game.camera.x = LEVEL_PIXEL_WIDTH - game.camera.w;
  }

  if(game.camera.y >= LEVEL_PIXEL_HEIGHT - game.camera.h)
  {
    game.camera.y = LEVEL_PIXEL_HEIGHT - game.camera.h;
  }
}

internal i32
init_game()
{
  i32 result = 0;

  if(!SDL_Init(SDL_INIT_VIDEO))
  {
    game.window = SDL_CreateWindow("Moonbreath", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                 WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if(game.window)
    {
      printf("Monitor refresh rate is %d HZ\n", get_window_refresh_rate(game.window));

      u32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
      game.renderer = SDL_CreateRenderer(game.window, -1, render_flags);
      if(game.renderer)
      {
        i32 img_flags = IMG_INIT_PNG;
        if(IMG_Init(img_flags) & img_flags)
        {
          if(!TTF_Init())
          {
            b32 font_ok = 1;

            font[font_clean] = create_ttf_font("../data/fonts/dos_vga_437.ttf", 16, 4);
            font[font_retro] = create_ttf_font("../data/fonts/slkscr.ttf", 16, 4);
            font[font_cursive] = create_ttf_font("../data/fonts/alkhemikal.ttf", 16, 4);

            for(i32 i = 0; i < font_total; ++i)
            {
              if(!font[i].success)
              {
                font_ok = 0;
                debug("Font atlas %d failed\n", i);
              }
            }

            if(font_ok)
            {
              b32 texture_ok = 1;

              texture[tex_tilemap] = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LEVEL_PIXEL_WIDTH, LEVEL_PIXEL_HEIGHT);
              texture[tex_game_tileset] = load_texture("../data/images/game_tileset.png", 0);
              texture[tex_item_tileset] = load_texture("../data/images/item_tileset.png", 0);
              texture[tex_sprite_sheet] = load_texture("../data/images/sprite_sheet.png", 0);
              texture[tex_inventory_win] = load_texture("../data/images/inventory_win.png", 0);
              texture[tex_inventory_item_win] = load_texture("../data/images/inventory_item_win.png", 0);
              texture[tex_inventory_selected_item] = load_texture("../data/images/inventory_selected_item.png", 0);
              texture[tex_interface_bottom_win] = load_texture("../data/images/interface_bottom_win.png", 0);
              texture[tex_health_bar_outside] = load_texture("../data/images/health_bar_outside.png", 0);
              texture[tex_health_bar_inside] = load_texture("../data/images/health_bar_inside.png", 0);

              for(i32 i = 0; i < tex_total; ++i)
              {
                if(!texture[i])
                {
                  texture_ok = 0;
                  debug("Texture %d failed", i);
                }
              }

              if(texture_ok)
              {
                // NOTE(rami):
                // srand(time(0));
                srand(1553293671);
                printf("SEED: %lu\n\n", time(0));

                game.state = state_running;
                game.camera = v4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - CONSOLE_HEIGHT);
                game.turn_changed = 1;
                game.perf_count_frequency = SDL_GetPerformanceFrequency();

                for(i32 i = 0; i < ITEM_COUNT; ++i)
                {
                  item[i].unique_id = i + 1;
                }

                for(i32 i = 0; i < CONSOLE_MESSAGE_COUNT; ++i)
                {
                  strcpy(console_message[i].msg, CONSOLE_MESSAGE_EMPTY);
                  console_message[i].color = color_black;
                }

                // NOTE(rami): Since we know the item we are setting the information for,
                // we could skip all the things that item doesn't care about because
                // the item array is initialized to zero

                item_info[0].id = 1;
                strcpy(item_info[0].name, "Lesser Health Potion");
                item_info[0].category = category_consumable;
                item_info[0].tile = 1;
                strcpy(item_info[0].use, "Restores 2 health");
                item_info[0].heal_amount = 2;
                item_info[0].damage = 0;
                item_info[0].armor = 0;
                strcpy(item_info[0].description, "A magical red liquid created with \nan unknown formula. Consuming \nthem is said to heal simple cuts \nand even grievous wounds.");

                item_info[1].id = 2;
                strcpy(item_info[1].name, "Iron Sword");
                item_info[1].category = category_weapon;
                item_info[1].tile = 2;
                item_info[1].use[0] = 0;
                item_info[1].heal_amount = 0;
                item_info[1].damage = 2;
                item_info[1].armor = 0;
                strcpy(item_info[1].description, "A well-built straight sword with a\nsymbol of an unknown blacksmith\ningrained onto it.");

                item_info[2].id = 3;
                strcpy(item_info[2].name, "Rune Helmet");
                item_info[2].category = category_armor;
                item_info[2].tile = 3;
                item_info[2].use[0] = 0;
                item_info[2].heal_amount = 0;
                item_info[2].damage = 0;
                item_info[2].armor = 1;
                strcpy(item_info[2].description, "A rune helmet.");

                item_info[3].id = 4;
                strcpy(item_info[3].name, "Rune Chestplate");
                item_info[3].category = category_armor;
                item_info[3].tile = 4;
                item_info[3].use[0] = 0;
                item_info[3].heal_amount = 0;
                item_info[3].damage = 0;
                item_info[3].armor = 1;
                strcpy(item_info[3].description, "A rune chestplate.");

                item_info[4].id = 5;
                strcpy(item_info[4].name, "Rune Platelegs");
                item_info[4].category = category_armor;
                item_info[4].tile = 5;
                item_info[4].use[0] = 0;
                item_info[4].heal_amount = 0;
                item_info[4].damage = 0;
                item_info[4].armor = 1;
                strcpy(item_info[4].description, "A pair of rune platelegs.");

                item_info[5].id = 6;
                strcpy(item_info[5].name, "Rune Boots");
                item_info[5].category = category_armor;
                item_info[5].tile = 6;
                item_info[5].use[0] = 0;
                item_info[5].heal_amount = 0;
                item_info[5].damage = 0;
                item_info[5].armor = 1;
                strcpy(item_info[5].description, "A pair of rune boots.");

                item_info[6].id = 7;
                strcpy(item_info[6].name, "Rune Shoulders");
                item_info[6].category = category_armor;
                item_info[6].tile = 7;
                item_info[6].use[0] = 0;
                item_info[6].heal_amount = 0;
                item_info[6].damage = 0;
                item_info[6].armor = 1;
                strcpy(item_info[6].description, "A pair of rune shoulders.");

                item_info[7].id = 8;
                strcpy(item_info[7].name, "Rune Gloves");
                item_info[7].category = category_armor;
                item_info[7].tile = 8;
                item_info[7].use[0] = 0;
                item_info[7].heal_amount = 0;
                item_info[7].damage = 0;
                item_info[7].armor = 1;
                strcpy(item_info[7].description, "A pair of rune gloves.");

                result = 1;
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
            debug("SDL TTF library could not initialize: %s\n", SDL_GetError());
          }
        }
        else
        {
          debug("SLD image library could not initialize: %s\n", SDL_GetError());
        }
      }
      else
      {
        debug("SDL could not create a renderer: %s\n", SDL_GetError());
      }
    }
    else
    {
      debug("SDL could not create window: %s\n", SDL_GetError());
    }
  }
  else
  {
    debug("SDL could not initialize: %s\n", SDL_GetError());
  }

  return(result);
}

internal void
run_game()
{
  add_player();

  generate_level();

  add_monster(monster_slime, 16, 54);
  add_monster(monster_skeleton, 17, 54);

  add_item(id_iron_sword, 12, 57);
  add_item(id_rune_helmet, 13, 57);
  add_item(id_lesser_health_potion, 14, 57);
  add_item(id_lesser_health_potion, 15, 57);
  add_item(id_lesser_health_potion, 16, 57);
  add_item(id_lesser_health_potion, 17, 57);

  // add_item(id_rune_chestplate, 15, 57);
  // add_item(id_rune_platelegs, 16, 57);
  // add_item(id_rune_boots, 17, 57);
  // add_item(id_rune_shoulders, 18, 57);
  // add_item(id_rune_gloves, 19, 57);

  u32 frames_per_second = 60;
  r32 target_seconds_per_frame = 1.0f / (r32)frames_per_second;
  u64 old_counter = SDL_GetPerformanceCounter();
  r32 old_dt = SDL_GetPerformanceCounter();

  while(game.state)
  {
    SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(game.renderer);

    r32 new_dt = SDL_GetPerformanceCounter();
    game.dt = (r32)(new_dt - old_dt) / (r32)game.perf_count_frequency;
    old_dt = new_dt;
    // printf("\ndt: %f\n", game.dt);

    // NOTE(rami): Pop up text
  #if 0
    for(i32 i = POP_UP_TEXT_COUNT - 1; i > -1; --i)
    {
      if(pop_up_text[i].active)
      {
        printf("\npop_up_text[%d]\n", i);
        printf("str: %s\n", pop_up_text[i].str);
        printf("x: %.02f, y: %.02f\n", pop_up_text[i].x, pop_up_text[i].y);
        printf("change: %.02f\n", pop_up_text[i].change);
        printf("speed: %.02f\n", pop_up_text[i].speed);
        printf("duration_time: %dms\n", pop_up_text[i].duration_time);
        printf("start_time: %dms\n", pop_up_text[i].start_time);
      }
    }
  #endif

    // NOTE(rami): Inventory
  #if 0
    for(i32 i = INVENTORY_SLOT_COUNT - 1; i > -1; --i)
    {
      if(inventory.slot[i].id)
      {
        printf("\nInventory.slots[%d]\n", i);
        printf("id %d\n", inventory.slot[i].id);
        printf("unique_id %d\n", inventory.slot[i].unique_id);
        printf("in_inventory %d\n", inventory.slot[i].in_inventory);
        printf("equipped %d\n", inventory.slot[i].equipped);
        printf("x %d\n", inventory.slot[i].x);
        printf("y %d\n", inventory.slot[i].y);
      }
    }
  #endif

    // NOTE(rami): Item
  #if 0
    for(i32 i = ITEM_COUNT - 1; i > -1; --i)
    {
      if(items[i].id)
      {
        printf("\nitems[%d]\n", i);
        printf("id %d\n", items[i].id);
        printf("unique_id %d\n", items[i].unique_id);
        printf("in_inventory %d\n", items[i].in_inventory);
        printf("is_equipped %d\n", items[i].is_equipped);
        printf("x %d\n", items[i].x);
        printf("y %d\n", items[i].y);
      }
    }
  #endif

    // NOTE(rami): Player
  #if 0
    printf("\nPlayer\n");
    printf("frame_start.x, y: %d, %d\n", player.sprite.frame_start.x,
                                         player.sprite.frame_start.y);
    printf("frame_current.x, y: %d, %d\n", player.sprite.frame_current.x,
                                           player.sprite.frame_current.y);
    printf("frame_count: %d\n", player.sprite.frame_count);
    printf("frame_delay: %d\n", player.sprite.frame_delay);
    printf("frame_last_changed: %d\n", player.sprite.frame_last_changed);
    printf("new_x, new_y: %d, %d\n", player.new_x, player.new_y);
    printf("x, y: %d, %d\n", player.x, player.y);
    printf("w, h: %d, %d\n", player.w, player.h);
    printf("name: %s\n", player.name);
    printf("max_hp: %d\n", player.max_hp);
    printf("hp: %d\n", player.hp);
    printf("damage: %d\n", player.damage);
    printf("armor: %d\n", player.armor);
    printf("speed: %d\n", player.speed);
    printf("level: %d\n", player.level);
    printf("money: %d\n", player.money);
    printf("fov: %d\n", player.fov);
    printf("brightness: %d\n", player.brightness);
  #endif

    // NOTE(rami): Monster
  #if 0
    for(i32 i = MONSTER_COUNT - 1; i > -1; --i)
    {
      if(monster[i].type)
      {
        printf("\nmonster[%d]\n", i);
        printf("type: %d\n", monster[i].type);
        printf("ai: %d\n", monster[i].ai);

        printf("frame_start.x, y: %d, %d\n", monster[i].sprite.frame_start.x,
                                             monster[i].sprite.frame_start.y);
        printf("frame_current.x, y: %d, %d\n", monster[i].sprite.frame_current.x,
                                               monster[i].sprite.frame_current.y);
        printf("frame_count : %d\n", monster[i].sprite.frame_count);
        printf("frame_delay: %d\n", monster[i].sprite.frame_delay);
        printf("frame_last_changed: %d\n", monster[i].sprite.frame_last_changed);

        printf("x, y: %d, %d\n", monster[i].x, monster[i].y);
        printf("w, h: %d, %d\n", monster[i].w, monster[i].h);
        printf("in_combat: %d\n", monster[i].in_combat);
        printf("max_hp: %d\n", monster[i].max_hp);
        printf("hp: %d\n", monster[i].hp);
        printf("damage: %d\n", monster[i].damage);
        printf("armor: %d\n", monster[i].armor);
        printf("speed: %d\n", monster[i].speed);
        printf("level: %d\n", monster[i].level);
      }
    }
  #endif

    update_input();

    if(game.turn_changed)
    {
      update_player();
      update_monster();
      update_lighting();
      update_camera();

      game.turn_changed = 0;
    }

    update_pop_up_text();

    render_tilemap();
    render_item();
    render_player();
    render_monster();
    render_ui();
    render_pop_up_text();

    if(inventory.open)
    {
      render_inventory();
    }

    u64 work_counter_elapsed = SDL_GetPerformanceCounter() - old_counter;
    r32 ms_for_work = (1000.0f * (r32)work_counter_elapsed) / (r32)game.perf_count_frequency;
    // printf("ms_for_work: %.02f\n", ms_for_work);

    if(get_seconds_elapsed(old_counter, SDL_GetPerformanceCounter()) < target_seconds_per_frame)
    {
      u32 time_to_delay = ((target_seconds_per_frame - get_seconds_elapsed(old_counter,
                          SDL_GetPerformanceCounter())) * 1000) - 1;
      if(time_to_delay > 0)
      {
        SDL_Delay(time_to_delay);
      }

      while(get_seconds_elapsed(old_counter, SDL_GetPerformanceCounter())
            < target_seconds_per_frame)
      {
      }
    }
    else
    {
      // NOTE(rami): We're right on the schedule or late
      // NOTE(rami): Valgrind will trigger this!
      // return;
    }

    u64 new_counter = SDL_GetPerformanceCounter();
    u64 elapsed_counter = new_counter - old_counter;
    SDL_RenderPresent(game.renderer);

    r32 ms_per_frame = (1000.0f * (r32)elapsed_counter) / (r32)game.perf_count_frequency;
    r32 frames_per_second = (r32)game.perf_count_frequency / (r32)elapsed_counter;
    old_counter = new_counter;
    // printf("ms_per_frame: %.02f\n", ms_per_frame);
    // printf("frames_per_second: %.02f\n", frames_per_second);
    // printf("time_elapsed: %d\n", game.time_elapsed);
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