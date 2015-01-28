#include <stdio.h>
#include <stdlib.h>
#include "dungeon_generation.h"


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
