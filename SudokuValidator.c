#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int sudoku[9][9];   // matriz global del sudoku

int validate_line(int index, int check_rows)
{
    int seen[10] = {0};

    for (int i = 0; i < 9; i++)
    {
        int value;

        if (check_rows)
        {
            value = sudoku[index][i];
        }
        else
        {
            value = sudoku[i][index];
        }

        if (value < 1 || value > 9 || seen[value])
        {
            return 0;
        }

        seen[value] = 1;
    }

    return 1;
}

int validate_rows()
{
    for (int i = 0; i < 9; i++)
    {
        if (!validate_line(i, 1))
        {
            return 0;
        }
    }

    return 1;
}

int validate_columns()
{
    for (int i = 0; i < 9; i++)
    {
        if (!validate_line(i, 0))
        {
            return 0;
        }
    }

    return 1;
}

int validate_subgrid(int start_row, int start_col)
{
    int seen[10] = {0};

    for (int row = start_row; row < start_row + 3; row++)
    {
        for (int col = start_col; col < start_col + 3; col++)
        {
            int value = sudoku[row][col];

            if (value < 1 || value > 9 || seen[value])
            {
                return 0;
            }

            seen[value] = 1;
        }
    }

    return 1;
}

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

    for (int i = 0; i < 81; i++)
    {
        int row = i / 9;
        int col = i % 9;

        sudoku[row][col] = map[i] - '0';
    }

    printf("\nSudoku cargado:\n");

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }

    if (validate_rows())
    {
        printf("\nFilas válidas\n");
    }
    else
    {
        printf("\nError en filas\n");
    }

    if (validate_columns())
    {
        printf("Columnas válidas\n");
    }
    else
    {
        printf("Error en columnas\n");
    }

    int subgrid_valid = 1;

    for (int row = 0; row < 9; row += 3)
    {
        for (int col = 0; col < 9; col += 3)
        {
            if (!validate_subgrid(row, col))
            {
                subgrid_valid = 0;
            }
        }
    }

    if (subgrid_valid)
    {
        printf("Subcuadros válidos\n");
    }
    else
    {
        printf("Error en subcuadros\n");
    }

    return 0;
}