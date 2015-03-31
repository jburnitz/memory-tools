CC=gcc

all: dumpmem patchmem

dumpmem: dumpmem.c
	$(CC) -o dumpmem dumpmem.c

patchmem: patchmem.c
	$(CC) -o patchmem patchmem.c -lm

clean:
	rm -f patchmem dumpmem
