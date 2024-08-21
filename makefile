CC=gcc
FLAGS=-Wall -Wextra -g 

all	:	candidature

candidature:	main.o candidature.o
		$(CC) main.o candidature.o -o candidature -ljansson

candidature.o:	candidature.c candidature.h 
		$(CC) -c candidature.c $(FLAGS) -o candidature.o -ljansson

main.o	:	main.c candidature.h
		$(CC) -c main.c $(FLAGS) -o main.o

clean	:
		rm -rf *.o
		rm -rf candidature
