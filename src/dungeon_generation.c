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

/*
 * Sets outer edge coordinates to impasse, inner corrdinates to rock,
 * generates rooms, then corridors
 */
void init_level(level_t *level){
	int i, j;

	//initialze num rooms
	level->num_rooms = 0;
	

	//sets all initial coordinates to proper state
	for(i = 0; i < DUNGEON_X; i++){
		for(j = 0; j < DUNGEON_Y; j++){
			if(i == 0 || i == (DUNGEON_X - 1) || j == 0 || j == (DUNGEON_Y - 1) ){
				level->map[i][j] = impasse;
			} else{
				level->map[i][j] = rock;
			}
		}
	}

	int fails = 0;
	//while room generation conditions are not met, generate rooms
	while(  level->num_rooms < MIN_ROOMS  || 
		level->num_rooms > MIN_ROOMS && level->num_rooms < MAX_ROOMS && fails < MAX_CONSECUTIVE_FAILS  ){
		
		generate_room(level, &fails);
		fails++;
		
	}

	//for each room, generate a corridor to the next room in the sequence
	for(i = 0; i < (level->num_rooms - 1); i++){
		generate_corridor(level, i, i+1);
	}
}

/*
 * Function called by init_level to generate one room in the level with a random location,
 * size, and at least MIN_HAUSDORFF_DIST away from all other rooms
 */
int generate_room(level_t *level, int* consecutive_fails){
	int i, j, k;
	//determine random center of room
	int x_loc = (rand() % DUNGEON_X) + 1;		//nonzero random x value
	int y_loc = (rand() % DUNGEON_Y) + 1;		//nonzero random y value

	//check if valid center; not on an edge or in an open area
	//return if not valid
	if( level->map[x_loc][y_loc] == impasse || level->map[x_loc][y_loc] == open)
		return 1;

	//determine random room dimenions
	int center_to_edge_x = ( (rand() % MAX_ROOM_X) + MIN_ROOM_X ) / 2;	// 1/2 random number between max/min room x
	int center_to_edge_y = ( (rand() % MAX_ROOM_Y) + MIN_ROOM_Y ) / 2;	// 1/2 random number between max/min room y

	//check if room, about center x_loc, y_loc would be a valid room location
	//that is, within the bounds of the level - return if not valid
	if(x_loc - center_to_edge_x <= 0 || x_loc + center_to_edge_x >= DUNGEON_X - 1 ||
			y_loc - center_to_edge_y <= 0 || y_loc + center_to_edge_y >= DUNGEON_Y - 1)
		return 2;

	int x_dim = 2 * center_to_edge_x + 1;
	int y_dim = 2 * center_to_edge_y + 1;

	int x_top_left = x_loc - center_to_edge_x;
	int y_top_left = y_loc - center_to_edge_y;

	//checking for rooms within rooms
	//return if found
	for(i = x_top_left; i < x_top_left + x_dim; i++){
		for(j = y_top_left; j < y_top_left + y_dim; j++){
			if( level->map[i][j] ==  open )
				return 3;
		}
	}		
	
	//checking for rooms within MIN_HAUSDORFF_DIST blocks of each other
	//return if found
	for(i = x_top_left; i < x_top_left + x_dim; i++){
		for(j = y_top_left; j < y_top_left + y_dim; j++){
			for(k = 1; k <= MIN_HAUSDORFF_DIST; k++){ 

				if( i == x_top_left) {		// top row case
					if( level->map[i-k][j] == open )
						return 4;
				} 
				if(j == y_top_left){		// left edge case
					if( level->map[i][j-k] == open )
						return 4;
				}
				if( j == (y_top_left + y_dim - 1) ){		//right edge case
					if( level->map[i][j+k] == open )
						return 4;
				}
				if( i == (x_top_left + x_dim - 1) ){		//bottom edge case
					if( level->map[i+k][j] == open )
						return 4;
				}
			}
		}
	}


	//otherwise, room is in a valid location, so set location in room struct
	level->rooms[level->num_rooms].center_index_x = x_loc;
	level->rooms[level->num_rooms].center_index_y = y_loc;

	level->rooms[level->num_rooms].dimension_x = x_dim;
	level->rooms[level->num_rooms].dimension_y = y_dim;

	level->rooms[level->num_rooms].top_left_index_x = x_top_left;
	level->rooms[level->num_rooms].top_left_index_y = y_top_left;

	level->rooms[level->num_rooms].area = x_dim * y_dim; 

	//draw room to level
	for(i = x_top_left; i < x_top_left + x_dim; i++){
		for(j = y_top_left; j < y_top_left + y_dim; j++){
			
			level->map[i][j] = open;
		}
	}

	//increment num_rooms
	level->num_rooms++;

	//reset fail count
	*consecutive_fails = 0;
}

/*
 * Utility function called by generate_corridor
 * returns 1 if arg is positive, -1 if arg is negative
 */
int sign(int x){
	return (x > 0) - (x < 0);
}

/*
 * Function called by init_level that draws a corridor between two rooms
 * passed as arguments
 */
int generate_corridor(level_t *level, int src_index, int dest_index){
	int i;
	//randomly determine how to leave room (vertical or horizontal)
	int dir = rand() % 2;

	int src_x = level->rooms[src_index].center_index_x;
	int src_y = level->rooms[src_index].center_index_y;

	int dest_x = level->rooms[dest_index].center_index_x;
	int dest_y = level->rooms[dest_index].center_index_y;

	int x_sign = sign(dest_x - src_x);
	int y_sign = sign(dest_y - src_y);

	if(!dir){	//leave horizontal
		for(i = src_y; i !=  dest_y; i += y_sign)
			level->map[src_x][i] = corridor;

		for(i = src_x; i != dest_x; i += x_sign)
			level->map[i][dest_y] = corridor;
			
	} else {	//leave vertical
		for ( i = src_x; i != dest_x; i += x_sign)
			level->map[i][src_y] = corridor;
		
		for( i = src_y; i != dest_y; i += y_sign)
			level->map[dest_x][i] = corridor;
	}
}

void main(){
	int i,j;
	time_t t;
	level_t level;

	//initialize room
	srand( time(&t) );
	init_level(&level);

	//print each line
	for(i = 0; i < DUNGEON_X; i++){
		for(j = 0; j < DUNGEON_Y; j++){
			if(level.map[i][j] == rock || level.map[i][j] == impasse){
				printf("#");
			}
			else if(level.map[i][j] == open || level.map[i][j] == corridor){
				printf(".");
			}
		}
		printf("\n");
	}
}
