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
    int true_buffer_len = buffer_len;

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

    shm_base = mmap(0, buffer_len_init, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
        printf("Terr: Mapeo fallido: %s\n", strerror(errno));
        exit(1);
    }

    

    cJSON *json = NULL;
    json = cJSON_Parse(shm_base);

    cJSON_SetNumberValue(cJSON_GetObjectItem(json, "covid"), 1);

    char *tmp_json = cJSON_Print(json);
    buffer_len = (int)(strlen(tmp_json));

    shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                    shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
        printf("Cons: Fallo en el mapeo: %s\n", strerror(errno));
        exit(1);
    }

    for (size_t i = 0; i < buffer_len; i++)
    {
        *(shm_base + i) = *(tmp_json + i);
    }

    cons = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "cons_viv"));
    prod = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "prod_viv"));

    while ((cons + prod) > 0)
    {
        printf("cons %i, prod %i\n", cons, prod);
        // printf("%s\n", cJSON_Print(json));
        if (sem_post(sem_p) == -1)
        {
            perror("sem_post: sem");
            exit(1);
        }
         if (sem_post(sem_c) == -1)
        {
            perror("sem_post: sem");
            exit(1);
        }
        json = cJSON_Parse(shm_base);

        cons = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "cons_viv"));
        prod = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "prod_viv"));

        sleep(1);
    }

    printf("%sMensaje totales %s%f\n",KRED,KWHT, cJSON_GetNumberValue(
                                       cJSON_GetObjectItem(json, "msg_tot")));
    printf("%sMensaje en el buffer\n",KRED);

    for (size_t i = 0; i < true_buffer_len; i++)
    {
        printf("    %s\n", cJSON_Print(cJSON_GetArrayItem(
                               cJSON_GetObjectItem(json, "buffer"), i)));
    }

    printf("%sTotal de productores %s%f\n",KRED,KWHT,
           cJSON_GetNumberValue(
               cJSON_GetObjectItem(json, "prod_tot")));
    printf("%sTotal de consumidores %s%f\n",KRED,KWHT,
           cJSON_GetNumberValue(
               cJSON_GetObjectItem(json, "cons_tot")));
    printf("%sConsumidores eliminados por llave %s%f\n",KRED,KWHT,
           cJSON_GetNumberValue(
               cJSON_GetObjectItem(json, "cons_key")));
    printf("%sTiempo esperando total %s%f\n",KRED,KWHT,
           cJSON_GetNumberValue(
               cJSON_GetObjectItem(json, "wait_t")));
    printf("%sTiempo bloqueado total %s%f\n",KRED,KWHT,
           cJSON_GetNumberValue(
               cJSON_GetObjectItem(json, "block_t")));
    printf("%sTiempo de usuario total %s%f\n",KRED,KWHT,
           cJSON_GetNumberValue(
               cJSON_GetObjectItem(json, "user_t")));
    printf("%sTiempo de kernel total %s%f\n",KRED,KWHT,
           cJSON_GetNumberValue(
               cJSON_GetObjectItem(json, "kernel_t")));

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

    //Mensajes totales, mensajes en el buffer, total de pro-ductores,
    //total  de  consumidores,  consumidores  eliminados  por  llave,
    //Tiempo  esperando  total,tiempo bloqueado total, tiempo de usuario total,
    //tiempo de kernel total.

    return 0;
}