CC = g++
CFLAGS = -lglfw -lGLU -lGL -lGLEW
LFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

main:  main.o validators.o math-ops.o graphics.o game-objects.o controls.o 
	$(CC) $^ $(LFLAGS) -o invaders

main.o: main.cpp main.h
	$(CC) -c $< $(CFLAGS)

validators.o: validators.cpp validators.h
	$(CC) -c $< $(CFLAGS)

math-ops.o: math-ops.cpp math-ops.h
	$(CC) -c $< $(CFLAGS)

graphics.o: graphics.cpp graphics.h
	$(CC) -c $< $(CFLAGS)

game-objects.o: game-objects.cpp game-objects.h
	$(CC) -c $< $(CFLAGS)

controls.o: controls.cpp controls.h
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f invaders
	rm -f *.o
	rm -f *.h.gch
	rm -f *~