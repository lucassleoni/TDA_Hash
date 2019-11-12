CC = gcc
CFLAGS = -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0

compile: hash.c minipruebas_hash.c
	clear
	$(CC) $(CFLAGS) *.c -o hash -g 

hash: compile
	clear
	./hash

valgrind: compile
	clear
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./hash