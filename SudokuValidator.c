#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <omp.h>

int sudoku[9][9];

int validate_line(int index, int check_rows)
{
    int seen[10] = {0};

    for (int i = 0; i < 9; i++)
    {
        int value = check_rows ? sudoku[index][i] : sudoku[i][index];

        if (value < 1 || value > 9 || seen[value])
            return 0;

        seen[value] = 1;
    }

    return 1;
}

int validate_rows()
{
    for (int i = 0; i < 9; i++)
        if (!validate_line(i, 1))
            return 0;

    return 1;
}

int validate_columns()
{
    for (int i = 0; i < 9; i++)
        if (!validate_line(i, 0))
            return 0;

    return 1;
}

int validate_subgrid(int start_row, int start_col)
{
    int seen[10] = {0};

    for (int r = start_row; r < start_row + 3; r++)
    {
        for (int c = start_col; c < start_col + 3; c++)
        {
            int value = sudoku[r][c];

            if (value < 1 || value > 9 || seen[value])
                return 0;

            seen[value] = 1;
        }
    }

    return 1;
}

void* column_thread(void* arg)
{
    printf("Column thread TID: %ld\n", syscall(SYS_gettid));

    validate_columns();

    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    omp_set_num_threads(1);
    if (argc != 2)
    {
        printf("Uso: %s <archivo_sudoku>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    char *map = mmap(NULL, 81, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < 81; i++)
    {
        int row = i / 9;
        int col = i % 9;
        sudoku[row][col] = map[i] - '0';
    }

    int sudoku_valid = 1;

    if (!validate_rows())
        sudoku_valid = 0;

    if (!validate_columns())
        sudoku_valid = 0;

    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int r = 0; r < 9; r += 3)
        for (int c = 0; c < 9; c += 3)
            if (!validate_subgrid(r, c))
                sudoku_valid = 0;

    printf("Sudoku %s\n", sudoku_valid ? "valido" : "invalido");

    pid_t parent_pid = getpid();
    pid_t child_pid = fork();

    if (child_pid == 0)
    {
        char pid_str[20];
        sprintf(pid_str, "%d", parent_pid);

        execlp("ps", "ps", "-p", pid_str, "-lLf", NULL);

        exit(1);
    }
    else
    {
        wait(NULL);

        pthread_t thread_id;

        pthread_create(&thread_id, NULL, column_thread, NULL);
        pthread_join(thread_id, NULL);

        printf("Main thread TID: %ld\n", syscall(SYS_gettid));
    }

    return 0;
}