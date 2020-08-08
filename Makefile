CC = g++
CFLAGS = -lglfw -lGLU -lGL -lGLEW
LFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

main: main.o
	$(CC) main.o $(LFLAGS) -o invaders

main.o: main.cpp
	$(CC) -c main.cpp $(CFLAGS)

clean:
	rm -f invaders
	rm -f *.o
	rm -f *~