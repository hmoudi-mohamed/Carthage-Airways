prog:aeroport.o main.o
	gcc aeroport.o main.o -o prog -g
main.o:main.c
	gcc -c main.c -g
aeroport.o:aeroport.c
	gcc -c aeroport.c -g
prog_avance:aeroport.o test_avance.o
	gcc aeroport.o test_avance.o -o prog_avance -g
test_avance.o:test_avance.c
	gcc -c test_avance.c -g
prog_menu:aeroport.o main_menu.o
	gcc aeroport.o main_menu.o -o prog_menu -g
main_menu.o:main_menu.c
	gcc -c main_menu.c -g
