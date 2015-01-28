#include <stdio.h>
#include <stdlib.h>

#define DUNGEON_X 160
#define DUNGEON_Y 96

#define MIN_HAUSDORFF_DIST 3
#define MAX_ROOMS 20
#define MIN_ROOMS 12

#define MIN_ROOM_X 8
#define MAX_ROOM_X 30

#define MIN_ROOM_Y 5
#define MAX_ROOM_Y 30

#define MAX_CONSECUTIVE_FAILS 2000


/*
 * An enumerated type for determining what kind of space is at a coordinate 
 */
typedef enum{ open, corridor, impasse, rock,  monster, avatar }coord_type;



/*
 * A struct for holding elements of a room
 */
typedef struct room{
	int center_index_x;
	int center_index_y;

	int top_left_index_x;
	int top_left_index_y;

	int dimension_x;
	int dimension_y;

	int area;
} room_t;



/*
 * A struct for holding elements of a dungeon level
 */
typedef struct level{
	int num_rooms;
	room_t rooms[MAX_ROOMS];
	coord_type map[DUNGEON_X][DUNGEON_Y];
} level_t;


void init_level(level_t *level);

int generate_room(level_t *level, int* consecutive_fails);

int sign(int x);

int generate_corridor(level_t *level, int src_index, int dest_index);
