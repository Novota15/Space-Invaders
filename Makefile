CC = gcc
CFLAGS = -lglfw -lGLU -lGL -lGLEW

all: main

main: main.o
	$(CC) $(LFLAGS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f main
	rm -f *.o
	rm -f *~