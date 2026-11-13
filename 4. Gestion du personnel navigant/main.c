/* main.c - Gestion du personnel navigant - Carthage Airways
 * Compilation : make        Exécution : ./gestion_personnel        Tests sans interface : make test */
#include "app.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    ouvrir_login();
    gtk_main();
    return 0;
}
