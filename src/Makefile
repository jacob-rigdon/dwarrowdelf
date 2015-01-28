all: dungeon_generation.o dwarrowdelf.o
	gcc dungeon_generation.o dwarrowdelf.o -o dwarrowdelf
dungeon_generation.o: dungeon_generation.c
	gcc -c dungeon_generation.c

dwarrowdelf.o: dwarrowdelf.c
	gcc -c dwarrowdelf.c
