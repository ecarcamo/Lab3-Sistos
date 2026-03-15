#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int sudoku[9][9];   // matriz global del sudoku

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Uso: %s <archivo_sudoku>\n", argv[0]);
        return 1;
    }

    printf("Archivo recibido: %s\n", argv[1]);

    int file_descriptor = open(argv[1], O_RDONLY);

    if (file_descriptor < 0)
    {
        perror("Error abriendo archivo");
        return 1;
    }

    printf("Archivo abierto correctamente\n");

    return 0;
}