#include "../headers/terminator.h"

int main(int argc, char **argv)
{
    char *buffer_name = NULL;

    int buffer_len, buffer_len_init;

    int opt = 0;

    while ((opt = getopt(argc, argv, "l:n:")) != -1)
    {
        switch (opt)
        {
        case 'l':
            buffer_len = atoi(optarg);
            break;

        case 'n':
            buffer_name = optarg;
            break;

        default:
            break;
        }
    }

    sem_t *sem_p, *sem_c; //semaforo

    // len y=82x+415
    buffer_len_init = (82 * buffer_len) + 415;

    char *buffer_name_tmp = (char *)malloc((strlen(buffer_name) + 1) * sizeof(char));
    strcpy(buffer_name_tmp, "/");
    strcat(buffer_name_tmp, buffer_name);
    strcpy(buffer_name, buffer_name_tmp);
    free(buffer_name_tmp);

    int shm_fd;
    char *shm_base;

    if ((sem_c = sem_open("/SemafCons", O_CREAT, 0660, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }
    if ((sem_p = sem_open("/SemafProd", O_CREAT, 0660, buffer_len)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    shm_fd = shm_open(buffer_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        printf("Terr: Fallo en memoria compartida: %s\n", strerror(errno));
        exit(1);
    }

    ftruncate(shm_fd, buffer_len_init);

    shm_base = mmap(0, buffer_len_init, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
        printf("Terr: Mapeo fallido: %s\n", strerror(errno));
        exit(1);
    }

    if (munmap(shm_base, buffer_len) == -1)
    {
        printf("Terr: Fallo el unmap: %s\n", strerror(errno));
        exit(1);
    }

    if (close(shm_fd) == -1)
    {
        printf("Terr: Fallo el cierre: %s\n", strerror(errno));
        exit(1);
    }

    if (shm_unlink(buffer_name) == -1)
    {
        printf("Terr: Error al remover %s: %s\n", buffer_name, strerror(errno));
        exit(1);
    }

    if (sem_unlink("/SemafProd") == -1)
    {
        printf("Terr: Error al remover '/SemafProd': %s\n", strerror(errno));
        exit(1);
    }

    if (sem_unlink("/SemafCons") == -1)
    {
        printf("Terr: Error al remover '/SemafCons': %s\n", strerror(errno));
        exit(1);
    }

    return 0;
}