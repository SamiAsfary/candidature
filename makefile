CC=gcc
FLAGS=-Wall -Wextra -g 

all	:	application

application:	main.o application.o
		$(CC) main.o application.o -o application -ljansson

application.o:	application.c application.h 
		$(CC) -c application.c $(FLAGS) -o application.o -ljansson

main.o	:	main.c application.h
		$(CC) -c main.c $(FLAGS) -o main.o

clean	:
		rm -rf *.o
		rm -rf application
