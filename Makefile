CC = g++
CFLAGS = -lglfw -lGLU -lGL -lGLEW
LFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

main: main.o game-objects.o
	$(CC) main.o game-objects.o $(LFLAGS) -o invaders

main.o: main.cpp main.h
	$(CC) -c main.cpp main.h $(CFLAGS)

game-objects.o: game-objects.cpp game-objects.h
	$(CC) -c game-objects.cpp game-objects.h $(CFLAGS)

clean:
	rm -f invaders
	rm -f *.o
	rm -f *~