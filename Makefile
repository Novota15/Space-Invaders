CC = g++
CFLAGS = -lglfw -lGLU -lGL -lGLEW
LFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

main:  game-objects.o controls.o main.o
	$(CC) game-objects.o controls.o main.o $(LFLAGS) -o invaders

main.o: main.cpp main.h
	$(CC) -c main.cpp main.h $(CFLAGS)

game-objects.o: game-objects.cpp game-objects.h
	$(CC) -c game-objects.cpp game-objects.h $(CFLAGS)

controls.o: controls.cpp controls.h
	$(CC) -c controls.cpp controls.h $(CFLAGS)

clean:
	rm -f invaders
	rm -f *.o
	rm -f *.h.gch
	rm -f *~