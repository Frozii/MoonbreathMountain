#include <text_render.h>
#include <util_io.h>

font_t* create_ttf_font_atlas(TTF_Font *font, int space_in_px)
{
  // we render all glyphs to this atlas
  SDL_Texture *new_atlas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1376, 32);

  // remove black glyph background
  SDL_SetTextureBlendMode(new_atlas, SDL_BLENDMODE_BLEND);

  // malloc the font  
  font_t *font_struct = malloc(sizeof(font_t));
  font_struct->atlas = NULL;
  font_struct->space_in_px = space_in_px;

  // we don't set the advance for .ttf's so set this to zero
  font_struct->shared_advance_in_px = 0;

  // surface and a texture used by each glyph
  SDL_Surface *glyph_surf = NULL;
  SDL_Texture *glyph_tex = NULL;

  // position of the glyph
  int x = 0;
  int y = 0;

  for(int i = 0; i < FONT_METRICS_COUNT; i++)
  {
    // calculate wanted char
    char ch = i + START_ASCII_CHAR;

    // create the glyph texture
    SDL_Color color = {255, 255, 255, 255};
    glyph_surf = TTF_RenderGlyph_Solid(font, ch, color);
    glyph_tex = SDL_CreateTextureFromSurface(renderer, glyph_surf);

    // set glyph rendering to the atlas
    SDL_SetRenderTarget(renderer, new_atlas);

    // store the distance from the glyph to the next one
    int advance;
    TTF_GlyphMetrics(font, ch, NULL, NULL, NULL, NULL, &advance);

    // we will render on the x and y set earlier,
    // the width and height of the glyph we take from the surface
    SDL_Rect atlas_rect = {x, y, glyph_surf->w, glyph_surf->h};

    // store the glyph metrics for later use
    font_struct->metrics[i] = (glyph_metrics_t){atlas_rect.x, atlas_rect.y, atlas_rect.w, atlas_rect.h, advance};

    // render the glyph to the atlas
    SDL_RenderCopy(renderer, glyph_tex, NULL, &atlas_rect);

    // move where we render the glyph
    x += glyph_surf->w;

    // free the current glyph resources
    SDL_FreeSurface(glyph_surf);
    glyph_surf = NULL;

    SDL_DestroyTexture(glyph_tex);
    glyph_tex = NULL;
  }

  // unset rendering from the atlas
  SDL_SetRenderTarget(renderer, NULL);

  // point at our new atlas
  font_struct->atlas = new_atlas;

  // return the font struct
  return font_struct;
}

font_t* create_bmp_font_atlas(char *path, int glyph_w, int glyph_h, int glyphs_per_row, int space_in_px, int shared_advance_in_px)
{
  // load texture
  SDL_Texture *bmp_atlas = load_texture(path, &(SDL_Color){0, 0, 0, 0}); // NOTE(Rami): check the asm for this at some point vs the usual way to do it
  if(!bmp_atlas)
  {
    printf("Could not load file %s\n", path);
    return NULL;
  }

  // malloc a struct where we can hold the atlas pointer and the metrics
  // set the atlas to the texture we just loaded
  font_t *bmp_font = malloc(sizeof(font_t));
  bmp_font->atlas = bmp_atlas;
  bmp_font->space_in_px = space_in_px;
  bmp_font->shared_advance_in_px = shared_advance_in_px;

  // start at 1, 1 to bypass padding
  // 
  // glyph_num is for tracking how many glyphs
  // the atlas has in a single row
  int x = 1;
  int y = 1;
  int glyph_num = 0;

  // for however many glyphs we want to store
  for(int i = 0; i < FONT_METRICS_COUNT; i++)
  {
    // move to next row if needed
    if(glyph_num > glyphs_per_row)
    {
      x = 1;
      y += 17;
      glyph_num = 0;
    }

    // store the x, y, width and height of the glyph,
    // so that we can reference back to it
    bmp_font->metrics[i] = (glyph_metrics_t){x, y, glyph_w, glyph_h, 0};

    // move rendering position forward,
    // move to next glyph
    x += 17;
    glyph_num++;
  }

  // return the malloc'd struct
  return bmp_font;
}

void render_text(char *str, int text_x, int text_y, int text_color, font_t *font, ...)
{
  // holds the final string
  char str_final[256];

  // start a vararg list
  va_list arg_list;
  va_start(arg_list, font);

  // fill str_final with the final string that includes specifiers
  vsnprintf(str_final, sizeof(str_final), str, arg_list);
  va_end(arg_list);

  // points at the current character we want to render
  char *at = str_final;

  // holds the original starting x position of the text for wrapping
  int initial_x = text_x;

  // if the shared_advance variable is zero it means
  // we want to use the unique_advance value for each glyph.
  // 
  // otherwise each glyph will use the shared_advance value
  int share_advance;
  if(!font->shared_advance_in_px)
  {
    share_advance = 0;
  }
  else
  {
    share_advance = 1;
  }

  // while not a null-terminator
  while(at[0])
  {
    // calculate array index
    int array_index = *at - START_ASCII_CHAR;

    // if space
    if(at[0] == ' ')
    {
      at++;

      text_x += font->space_in_px;
      continue;
    }
    // if newline
    else if(at[0] == '\n')
    {
      at++;

      text_x = initial_x;
      text_y += 16;
      continue;
    }
    else if(at[0] == '\\' && at[1] == 'n')
    {
      at += 2;

      text_x = initial_x;
      text_y += 16;
      continue;
    }
    // if character is not stored
    else if(array_index < 0)
    {
      at++;

      printf("'%c': Character does not exist in metrics array\n", array_index + START_ASCII_CHAR);
      continue;
    }

    // apply color to glyph
    SDL_Color color = hex_to_rgba(text_color);
    SDL_SetTextureColorMod(font->atlas, color.r, color.g, color.b);

    // set the src and dst structs with the information from the metrics array
    glyph_metrics_t *glyph_metrics = &font->metrics[array_index];
    SDL_Rect src = {glyph_metrics->x, glyph_metrics->y, glyph_metrics->w, glyph_metrics->h};
    SDL_Rect dst = {text_x, text_y, font->metrics[array_index].w, font->metrics[array_index].h};

    // render glyph
    SDL_RenderCopy(renderer, font->atlas, &src, &dst);

    // apply advance
    if(!share_advance)
    {
      text_x += font->metrics[array_index].unique_advance_in_px;
    }
    else
    {
      text_x += font->shared_advance_in_px;
    }

    at++;
  }
}