#define RGBA_COLOR_WHITE_S (v4_t){{255, 255, 240, 255}}
#define RGBA_COLOR_RED_S (v4_t){{178, 34, 34, 255}}
#define RGBA_COLOR_BLUE_S (v4_t){{0, 128, 255, 255}}
#define RGBA_COLOR_GREEN_S (v4_t){{0, 179, 0, 255}}
#define RGBA_COLOR_YELLOW_S (v4_t){{207, 175, 0, 255}}
#define RGBA_COLOR_ORANGE_S (v4_t){{255, 165, 0, 255}}
#define RGBA_COLOR_BROWN_S (v4_t){{231, 165, 106, 255}}
#define RGBA_COLOR_BLACK_S (v4_t){{0, 0, 0, 0}}

#define RGBA_COLOR_WHITE_P 255, 255, 240, 255
#define RGBA_COLOR_RED_P 178, 34, 34, 255
#define RGBA_COLOR_BLUE_P 0, 128, 255, 255
#define RGBA_COLOR_GREEN_P 0, 179, 0, 255
#define RGBA_COLOR_YELLOW_P 207, 175, 0, 255
#define RGBA_COLOR_ORANGE_P 255, 165, 0, 255
#define RGBA_COLOR_BROWN_P 231, 165, 106, 255
#define RGBA_COLOR_BLACK_P 0, 0, 0, 0

// NOTE(Rami): I'm sure we can do the above color stuff better??

internal void
render_tilemap()
{
  SDL_SetRenderTarget(game.renderer, assets.textures[tex_tilemap]);
  SDL_RenderClear(game.renderer);

  for(i32 x = tile_div(game.camera.x); x < tile_div(game.camera.x + game.camera.w); x++)
  {
    for(i32 y = tile_div(game.camera.y); y < tile_div(game.camera.y + game.camera.h); y++)
    {
      SDL_Rect src = {tile_mul(level.tiles[(y * LEVEL_WIDTH_IN_TILES) + x]), 0, TILE_SIZE, TILE_SIZE};
      SDL_Rect dst = {tile_mul(x), tile_mul(y), TILE_SIZE, TILE_SIZE};

      v2_t current = v2(x, y);
      if(is_lit(current))
      {
        v4_t color = get_color_for_lighting_value(current);
        SDL_SetTextureColorMod(assets.textures[tex_game_tileset], color.r, color.g, color.b);
        SDL_RenderCopy(game.renderer, assets.textures[tex_game_tileset], &src, &dst);
      }
      else if(is_seen(current))
      {
        SDL_SetTextureColorMod(assets.textures[tex_game_tileset], lighting_seen, lighting_seen, lighting_seen);
        SDL_RenderCopy(game.renderer, assets.textures[tex_game_tileset], &src, &dst);
      }
    }
  }

  SDL_SetRenderTarget(game.renderer, NULL);

  SDL_Rect dst = {0, 0, game.camera.w, game.camera.h};
  SDL_RenderCopy(game.renderer, assets.textures[tex_tilemap], &game.camera, &dst);
}

internal void
render_text(char *str, v2_t pos, v4_t color, font_t font, ...)
{
  char txt_final[256];

  va_list arg_list;
  va_start(arg_list, font);
  vsnprintf(txt_final, sizeof(txt_final), str, arg_list);
  va_end(arg_list);

  i32 origin_x = pos.x;
  char *at = txt_final;

  while(*at)
  {
    i32 array_index = *at - START_ASCII_CHAR;

    if(*at == ' ')
    {
      at++;
      pos.x += font.space_size;
      continue;
    }
    else if(*at == '\n')
    {
      at++;
      pos.x = origin_x;
      pos.y += 16;
      continue;
    }
    else if(*at == '\\' && at[1] == 'n')
    {
      at += 2;
      pos.x = origin_x;
      pos.y += 16;
      continue;
    }
    else if(array_index < 0)
    {
      at++;
      debug("'%c': Character does not exist in metrics array\n", array_index + START_ASCII_CHAR);
      continue;
    }

    SDL_SetTextureColorMod(font.atlas, color.r, color.g, color.b);

    SDL_Rect src = {font.metrics[array_index].x, font.metrics[array_index].y, font.metrics[array_index].w, font.metrics[array_index].h};
    SDL_Rect dst = {pos.x, pos.y, font.metrics[array_index].w, font.metrics[array_index].h};

    SDL_RenderCopy(game.renderer, font.atlas, &src, &dst);

    if(!font.shared_advance_in_px)
    {
      pos.x += font.metrics[array_index].unique_advance_in_px;
    }
    else
    {
      pos.x += font.shared_advance_in_px;
    }

    at++;
  }
}