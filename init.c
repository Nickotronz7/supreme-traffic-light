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

int main(int argc, char **argv)
{
    char *buffer_name = NULL;
    int buffer_len;

    int opt = 0;

    while ((opt = getopt(argc, argv, "l:n:")) != -1)
    {
        if (opt == 'l')
            buffer_len = atoi(optarg);
        else if (opt == 'n')
            buffer_name = optarg;
        else
            printf("Argumentos invalidos");
    }

    int shm_fd;
    char *shm_base;
    char *ptr;

    shm_fd = shm_open(buffer_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        printf("prod: Shared memory failed: %s\n", strerror(errno));
        exit(1);
    }

    ftruncate(shm_fd, buffer_len);

    shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
        printf("prod: Map failed: %s\n", strerror(errno));
        // close and shm_unlink?
        exit(1);
    }

    return 0;
}
