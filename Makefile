all:
	gcc -g main.c -o main `sdl2-config --cflags --libs` -lGL -lm
