#ifndef DUNGEON_GENERATION_H
#define DUNGEON_GENERATION_H

#include "tiles.h"
#include "structs.h"

void place_spawns(entity_t *player, char *map, int map_pitch, int room_count, room_t *rooms);

void generate_dungeon(char *map, int map_pitch, int map_width, int map_height, int room_count, entity_t* player);

void initialize_map(char *map, int map_pitch, int map_width, int map_height);

void initialize_and_place_rooms(char *map, int map_pitch, int map_width, int map_height, int room_count, room_t *rooms);

void connect_rooms(char *map, int map_pitch, int room_count, room_t *rooms);

void place_corridors(char *map, int map_pitch, room_t room_a, room_t room_b, int direction);

int is_room_valid(char *map, int map_pitch, room_t room);

int random_int(int from, int to);

cell_t random_cell_in_rect(room_t room);

#endif // DUNGEON_GENERATION_H