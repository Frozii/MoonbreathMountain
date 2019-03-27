bool is_inside_level(iv2_t p)
{
  if(p.x >= 0 && p.x < LEVEL_WIDTH_IN_TILES && p.y >= 0 && p.y < LEVEL_HEIGHT_IN_TILES)
  {
    return true;
  }

  return false;
}

bool occupied(iv2_t pos)
{
  if(level.occupied[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x])
  {
    return true;
  }

  return false;
}

void set_occupied(iv2_t pos, bool val)
{
  level.occupied[(pos.y * LEVEL_WIDTH_IN_TILES) + pos.x] = val;
}

// NOTE(Rami): This is supposed to house all of our traversable tiles so we can check against them.
bool traversable(iv2_t p)
{
  if(level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_none ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_floor_stone ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_floor_grass ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_door_open ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_path_up ||
     level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile_path_down)
  {
    return true;
  }

  return false;
}

char* read_file(char *path, char *mode)
{
  FILE *file = fopen(path, mode);
  if(!file)
  {
    debug("Could not read file %s", path);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *buff = malloc(size + 1);
  fread(buff, size, 1, file);

  buff[size] = '\0';
  fclose(file);

  return buff;
}

SDL_Texture* load_texture(char *path, SDL_Color *color_key)
{
  SDL_Surface *loaded_surf = IMG_Load(path);
  if(!loaded_surf)
  {
    debug("SDL could not load image %s: %s", path, IMG_GetError());
    return NULL;
  }

  if(color_key)
  {
    // store the rgb color into color_key in the color format of the surface
    // all pixels with the color of color_key will be transparent
    i32 formatted_key = SDL_MapRGB(loaded_surf->format, color_key->r, 0, 0);
    SDL_SetColorKey(loaded_surf, SDL_TRUE, formatted_key);
  }

  SDL_Texture *new_tex = SDL_CreateTextureFromSurface(game_state.renderer, loaded_surf);
  if(!new_tex)
  {
    debug("SDL could not create a texture from surface: %s", SDL_GetError());
    SDL_FreeSurface(loaded_surf);
    return NULL;
  }

  SDL_FreeSurface(loaded_surf);
  return new_tex;
}

bool attack_entity(entity_t *attacker, entity_t *defender)
{
  defender->hp -= attacker->damage;
  if(defender->hp <= 0)
  {
    return true;
  }

  return false;
}

// NOTE(Rami): Do we need this?
// bool inside_level(iv2_t p)
// {
//   if(p.x < 0 || p.y < 0 || p.x >= LEVEL_WIDTH_IN_TILES || p.y >= LEVEL_HEIGHT_IN_TILES)
//   {
//     return false;
//   }

//   return true;
// }

bool iv2_equal(iv2_t a, iv2_t b)
{
  if(a.x == b.x && a.y == b.y)
  {
    return true;
  }

  return false;
}

SDL_Color hex_to_rgba(i32 hex)
{
  SDL_Color rgb_color = {(hex >> 24) & 0xFF, (hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF};
  return rgb_color;
}

i32 str_to_i(char *str)
{
  i32 result = 0;

  while(*str >= '0' && *str <= '9')
  {
    result *= 10;
    result += *str - '0';
    str++;
  }

  return result;
}

i32 rnum(i32 min, i32 max)
{
  return min + rand() % (max - min + 1);
}

bool str_cmp(char *a, char *b)
{
  while(*a && *b && *a++ == *b++)
  {
    if(*a == '\0' && *b == '\0')
    {
      return true;
    }
  }

  return false;
}

bool is_tile(iv2_t p, i32 tile)
{
  if(level.tiles[(p.y * LEVEL_WIDTH_IN_TILES) + p.x] == tile)
  {
    return true;
  }

  return false;
}

// NOTE(Rami): Does not consider diagonal movement.
i32 tile_dist(iv2_t a, iv2_t b)
{
  return abs(a.x - b.x) + abs(a.y - b.y);
}

i32 tile_div(i32 n)
{
  return n / TILE_SIZE;
}

i32 tile_mul(i32 n)
{
  return n * TILE_SIZE;
}