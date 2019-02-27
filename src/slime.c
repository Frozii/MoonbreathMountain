slime_t *slimes[SLIME_COUNT];

// NOTE(Rami): Creation of multiple slimes?
void create_slimes(i32 x, i32 y)
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(!slimes[i])
    {
      slimes[i] = malloc(sizeof(slime_t));
      slimes[i]->entity = malloc(sizeof(entity_t));

      slimes[i]->in_combat = false;
      slimes[i]->entity->hp = 4;
      slimes[i]->entity->damage = 1;
      slimes[i]->entity->armor = 0;
      slimes[i]->entity->fov = 4;
      slimes[i]->entity->pos.x = x;
      slimes[i]->entity->pos.y = y;
      slimes[i]->entity->new_pos.x = x;
      slimes[i]->entity->new_pos.y = y;
      slimes[i]->entity->aspect.w = TILE_SIZE;
      slimes[i]->entity->aspect.h = TILE_SIZE;
      slimes[i]->entity->anim.frame_num = 0;
      slimes[i]->entity->anim.frame_count = 4;
      slimes[i]->entity->anim.frame_delay = 400;
      slimes[i]->entity->anim.frame_last_changed = 0;
      return;
    }
  }

  // NOTE(Rami): 
  debug("Slime array is already full\n");
}
void update_slimes()
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i])
    {
      if(slimes[i]->entity->hp <= 0)
      {
        free_slimes(i, 1);
        continue;
      }

      b32 can_move = true;

  //     // NOTE(Rami): Enable SOON!
  //     // if(slimes[i]->in_combat)
  //     // {
  //     //   if(line_of_sight(slimes[i]->entity->pos, player->entity->pos))
  //     //   {
  //     //     if(distance(slimes[i]->entity->pos, player->entity->pos) == 1)
  //     //     {
  //     //       attack_entity(slimes[i]->entity, player->entity);
  //     //       add_console_msg("Slime attacks you for %d damage", RGBA_COLOR_WHITE_S, slimes[i]->entity->damage);
  //     //     }
  //     //     else
  //     //     {
  //     //       i32 sx = slimes[i]->entity->pos.x < player->entity->pos.x ? 1 : -1;
  //     //       i32 sy = slimes[i]->entity->pos.y < player->entity->pos.y ? 1 : -1;

  //     //       if(slimes[i]->entity->pos.x != player->entity->pos.x)
  //     //       {
  //     //         slimes[i]->entity->pos.x += sx;
  //     //       }

  //     //       if(slimes[i]->entity->pos.y != player->entity->pos.y)
  //     //       {
  //     //         slimes[i]->entity->pos.y += sy;
  //     //       }
  //     //     }
  //     //   }

  //     //   continue;
  //     // }

  //     // NOTE(Rami): Have some kinda AI function so we can have different
  //     // out of combat behaviours like the one below.


      int rand_dir = rnum(dir_left, dir_right);
      if(rand_dir == dir_left)
      {
        slimes[i]->entity->new_pos.x = slimes[i]->entity->pos.x - 1;
      }
      else
      {
        slimes[i]->entity->new_pos.x = slimes[i]->entity->pos.x + 1;
      }

      rand_dir = rnum(dir_up, dir_down);
      if(rand_dir == dir_up)
      {
        slimes[i]->entity->new_pos.y = slimes[i]->entity->pos.y - 1;
      }
      else
      {
        slimes[i]->entity->new_pos.y = slimes[i]->entity->pos.y + 1;
      }

      if(!is_pos_traversable(slimes[i]->entity->new_pos) ||
         iv2_is_equal(slimes[i]->entity->new_pos, player->entity->pos))
      {
        can_move = false;
      }

      if(can_move)
      {
        slimes[i]->entity->pos.x = slimes[i]->entity->new_pos.x;
        slimes[i]->entity->pos.y = slimes[i]->entity->new_pos.y;
      }
    }
    else
    {
      break;
    }
  }
}

void render_slimes()
{
  for(i32 i = 0; i < SLIME_COUNT; i++)
  {
    if(slimes[i])
    {
      if(global_state.time_elapsed > slimes[i]->entity->anim.frame_last_changed + slimes[i]->entity->anim.frame_delay)
      {
        slimes[i]->entity->anim.frame_num = (slimes[i]->entity->anim.frame_num < (slimes[i]->entity->anim.frame_count - 1)) ? slimes[i]->entity->anim.frame_num + 1 : 0;
        slimes[i]->entity->anim.frame_last_changed = global_state.time_elapsed;
      }

      SDL_Rect src = {tile_mul(slimes[i]->entity->anim.frame_num), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(slimes[i]->entity->pos.x) - global_state.camera.x, tile_mul(slimes[i]->entity->pos.y) - global_state.camera.y, TILE_SIZE, TILE_SIZE};
      SDL_RenderCopy(global_state.renderer, global_state.assets.textures[tex_monster_sprite_sheet], &src, &dst);
    }
    else
    {
      break;
    }
  }
}

void free_slimes(i32 start, i32 count)
{
  for(i32 i = start; i < start + count; i++)
  {
    if(slimes[i])
    {
      if(slimes[i]->entity)
      {
        free(slimes[i]->entity);
      }

      free(slimes[i]);
      slimes[i] = NULL;
    }
  }
}