avion: avion.o main.o
	gcc avion.o main.o -o avion 
main.o: main.c
	gcc -Wall -c main.c 
avion.o:avion.c
	gcc -Wall -c avion.c 
clean:
	rm -f *.o avion
