LFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

invaders: main.o
	g++ main.o -o invaders

main.o: main.cpp
	g++ -c main.cpp $(LFLAGS)

clean:
	rm -f main
	rm -f *.o
	rm -f *~