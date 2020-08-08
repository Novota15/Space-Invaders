CFLAGS = -lglfw -lGLU -lGL -lGLEW
LFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

invaders: main.o
	g++ main.o $(LFLAGS) -o invaders

main.o: main.cpp
	g++ -c main.cpp $(CFLAGS)

clean:
	rm -f main
	rm -f *.o
	rm -f *~