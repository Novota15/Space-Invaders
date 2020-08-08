CFLAGS = -lglfw -lGLU -lGL -lGLEW
LFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

invaders: main.o
	g++ main.o -lglfw -lGLU -lGL -lGLEW -o invaders

main.o: main.cpp
	g++ -c main.cpp -lglfw -lGLU -lGL -lGLEW

clean:
	rm -f invaders
	rm -f *.o
	rm -f *~