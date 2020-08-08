CC = gcc
CFLAGS = -c -g -lglfw -lGLU -lGL -lGLEW
LFLAGS = -g -lglfw -lGLU -lGL -lGLEW

all: main

main: main.o
	$(CC) $(LFLAGS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f main
	rm -f *.o
	rm -f *~