CC     = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-2.0 libglade-2.0`
LIBS   = `pkg-config --libs gtk+-2.0 libglade-2.0` -export-dynamic
OBJ    = main.o login.o interface_admin.o interface_utilisateur.o outils.o personnel.o historique.o

# Application complète (interfaces Glade 2 : administrateur + utilisateur)
gestion_personnel: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS)

# Tests des modules SANS interface : on lit les résultats dans les fichiers .txt
test: test_personnel test_historique
	@./test_personnel; echo "personnel     : code retour = $$? (0 = tout est correct)"; cat rapport_personnel.txt
	@./test_historique; echo "historique : code retour = $$? (0 = tout est correct)"; cat rapport_historique.txt

test_personnel: test_personnel.o personnel.o
	$(CC) -o $@ test_personnel.o personnel.o

test_historique: test_historique.o historique.o
	$(CC) -o $@ test_historique.o historique.o

%.o: %.c app.h personnel.h historique.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o gestion_personnel test_personnel test_historique test_personnel.txt tri_personnel.txt rapport_personnel.txt export_personnel.csv \
	      test_historique.txt rapport_historique.txt export_historique.csv
