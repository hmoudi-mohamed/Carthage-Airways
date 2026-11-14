CC     = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-2.0 libglade-2.0`
LIBS   = `pkg-config --libs gtk+-2.0 libglade-2.0` -export-dynamic
OBJ    = main.o login.o principal.o gestion_personnel.o espace.o outils.o personnel.o avion.o

# Application complète (interface Glade 2)
gestion_personnel: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

# Test du module personnel SANS interface : on lit les résultats dans les fichiers .txt
test: test_personnel
	./test_personnel; echo "code retour = $$? (0 = tout est correct)"; cat rapport_personnel.txt

test_personnel: test_personnel.o personnel.o
	$(CC) -o $@ test_personnel.o personnel.o

%.o: %.c app.h personnel.h avion.h date.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) test_personnel.o gestion_personnel test_personnel test_personnel.txt tri_personnel.txt rapport_personnel.txt
