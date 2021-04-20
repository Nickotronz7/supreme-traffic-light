#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>

#include "../headers/cJSON.h"

// l: largo del buffer
// n: nombre del buffer
// m: media de la distribucion
int main(int argc, char **argv)
{
    char *buffer_name = NULL;

    int buffer_len, dist_med;

    int opt = 0;

    while ((opt = getopt(argc, argv, "l:n:m:")) != -1)
    {
        if (opt == 'l')
            buffer_len = atoi(optarg);
        else if (opt == 'n')
            buffer_name = optarg;
        else if (opt == 'm')
            dist_med = atoi(optarg);
        else
            printf("Argumentos invalidos");
    }

    buffer_len = (buffer_len * 82) + 415;

    char *buffer_name_tmp = (char *)malloc((strlen(buffer_name) + 1) * sizeof(char));
    strcpy(buffer_name_tmp, "/");
    strcat(buffer_name_tmp, buffer_name);
    strcpy(buffer_name, buffer_name_tmp);
    free(buffer_name_tmp);

    int shm_fd;
    char *shm_base;

    shm_fd = shm_open(buffer_name, O_RDONLY, 0666);
    if (shm_fd == -1)
    {
        printf("Cons: Error en memoria compartida: %s\n", strerror(errno));
        exit(1);
    }

    shm_base = mmap(0, buffer_len, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
        printf("Cons: Fallo en el mapeo: %s\n", strerror(errno));
        exit(1);
    }

    printf("%s\n", shm_base);

    if (munmap(shm_base, buffer_len) == -1)
    {
        printf("cons: Unmap failed: %s\n", strerror(errno));
        exit(1);
    }

    /* close the shared memory segment as if it was a file */
    if (close(shm_fd) == -1)
    {
        printf("cons: Close failed: %s\n", strerror(errno));
        exit(1);
    }

    /* remove the shared memory segment from the file system */
    if (shm_unlink(buffer_name) == -1)
    {
        printf("cons: Error removing %s: %s\n", buffer_name, strerror(errno));
        exit(1);
    }

    return 0;
}