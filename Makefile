CFLAGS = -c -lglfw -lGLU -lGL -lGLEW
LFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

invaders: main.o
	g++ main.o -o $(LFLAGS) invaders

main.o: main.cpp
	g++ main.cpp $(CFLAGS)

clean:
	rm -f main
	rm -f *.o
	rm -f *~