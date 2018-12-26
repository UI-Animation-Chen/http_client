main: main.o http.o
	gcc main.o http.o -o main
http.o: http.c http.h
	gcc -c http.c -o http.o
main.o: main.c http.h
	gcc -c main.c -o main.o
