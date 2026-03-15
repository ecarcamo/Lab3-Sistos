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

    char *map = mmap(NULL, 81, PROT_READ, MAP_PRIVATE, file_descriptor, 0);

    if (map == MAP_FAILED)
    {
        perror("Error en mmap");
        return 1;
    }

    printf("Archivo mapeado en memoria\n");

    return 0;
}