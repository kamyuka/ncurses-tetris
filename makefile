CC = gcc

tetris: tetris.c
	$(CC) -o tetris tetris.c -lncurses

test:
	./tetris
clean:
	rm -f tetris
