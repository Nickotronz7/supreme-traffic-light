#include "../headers/cJSON.h"
#include "../headers/cons.h"

// l: largo del buffer
// n: nombre del buffer
// m: media de la distribucion
int main(int argc, char **argv)
{
    char *buffer_name = NULL;

    int buffer_len, dist_med, buffer_len_sem, mode;

    int opt = 0;

    sem_t *sem_p, *sem_c; //semaforo

    while ((opt = getopt(argc, argv, "l:n:m:j:")) != -1)
    {
        switch (opt)
        {
        case 'l':
            buffer_len = atoi(optarg);
            break;

        case 'n':
            buffer_name = optarg;
            break;

        case 'm':
            dist_med = atoi(optarg);
        case 'j':
            mode = atoi(optarg);

        default:
            break;
        }
    }

    buffer_len_sem = buffer_len;
    buffer_len = (buffer_len * 82) + 415;

    char *buffer_name_tmp = (char *)malloc((strlen(buffer_name) + 1) * sizeof(char));
    strcpy(buffer_name_tmp, "/");
    strcat(buffer_name_tmp, buffer_name);
    strcpy(buffer_name, buffer_name_tmp);
    free(buffer_name_tmp);

    if (mode)
    {
        auto_mode(sem_p, sem_c, buffer_len_sem, buffer_name, buffer_len);
    }
    else
    {
        manual_mode(sem_p, sem_c, buffer_len_sem, buffer_name, buffer_len);
    }

    return 0;
}

char *read_buffer(char *sh_json)
{
    cJSON *json = cJSON_Parse(sh_json);
    if (!included)
    {
        cJSON_SetNumberValue(cJSON_GetObjectItem(json, "cons_tot"),
                             cJSON_GetNumberValue(cJSON_GetObjectItem(
                                 json, "cons_tot")) +
                                 1);

        cJSON_SetNumberValue(cJSON_GetObjectItem(json, "cons_viv"),
                             cJSON_GetNumberValue(cJSON_GetObjectItem(
                                 json, "cons_viv")) +
                                 1);

        included = true;
    }

    int index, prod_id, rnd_key, prod_viv, cons_viv;
    char *msg_read, *timestamp;

    index = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "nxt_read"));
    prod_id = cJSON_GetNumberValue(
        cJSON_GetObjectItem(cJSON_GetArrayItem(
                                cJSON_GetObjectItem(json,
                                                    "buffer"),
                                index),
                            "id_prod"));

    timestamp = cJSON_GetStringValue(
        cJSON_GetObjectItem(cJSON_GetArrayItem(
                                cJSON_GetObjectItem(json, "buffer"),
                                index),
                            "timestamp"));

    rnd_key = cJSON_GetNumberValue(
        cJSON_GetObjectItem(cJSON_GetArrayItem(
                                cJSON_GetObjectItem(json,
                                                    "buffer"),
                                index),
                            "num_mag"));

    prod_viv = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "prod_viv"));

    cons_viv = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "cons_viv"));

    msg_read = cJSON_GetStringValue(
        cJSON_GetObjectItem(cJSON_GetArrayItem(
                                cJSON_GetObjectItem(json, "buffer"),
                                index),
                            "str_msg"));

    if (index + 1 == cJSON_GetNumberValue(cJSON_GetObjectItem(json,
                                                              "buffer_size")))
    {
        cJSON_SetNumberValue(cJSON_GetObjectItem(json, "nxt_read"), 0);
    }
    else
    {
        cJSON_SetNumberValue(cJSON_GetObjectItem(json, "nxt_read"), index + 1);
    }

    printf("{\n");
    printf("    Leido de: %i,\n", index);
    printf("    Productores vivos: %i,\n", prod_viv);
    printf("    Consumidores vivos: %i\n", cons_viv);
    printf("    Mensaje leido: %s,\n", msg_read);
    printf("    Magic Number leido %i\n", rnd_key);
    printf("    Timestamp %s\n", timestamp);
    printf("}\n");
    return cJSON_Print(json);
}

void auto_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
               char *buffer_name, int buffer_len)
{
    while (alive)
    {
        /* meta la probabilidad aqui */

        int shm_fd;
        char *shm_base;
        //semaforo open
        if ((sem_c = sem_open("/SemafCons", O_CREAT, 0660, 0)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        if ((sem_p = sem_open("/SemafProd", O_CREAT, 0660, buffer_len_sem)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        //-------
        shm_fd = shm_open(buffer_name, O_RDWR, 0666);
        if (shm_fd == -1)
        {
            printf("Cons: Error en memoria compartida: %s\n", strerror(errno));
            exit(1);
        }

        shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                        shm_fd, 0);
        if (shm_base == MAP_FAILED)
        {
            printf("Cons: Fallo en el mapeo: %s\n", strerror(errno));
            exit(1);
        }

        //Semaforo espera o --
        if (sem_wait(sem_c) == -1)
        {
            perror("sem_wait: sem");
            exit(1);
        }
        //zona critica
        char *tmp_json = read_buffer(shm_base);
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

        //zona critica
        //semafaro ++
        if (sem_post(sem_p) == -1)
        {
            perror("sem_post: sem");
            exit(1);
        }
    }
}

void manual_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
                 char *buffer_name, int buffer_len)
{
    char key;
    while (alive)
    {
        printf("Presione 'ENTER' para consumir un mensaje \n");
        scanf("%c", &key);

        int shm_fd;
        char *shm_base;
        //semaforo open
        if ((sem_c = sem_open("/SemafCons", O_CREAT, 0660, 0)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        if ((sem_p = sem_open("/SemafProd", O_CREAT, 0660, buffer_len_sem)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        //-------
        shm_fd = shm_open(buffer_name, O_RDWR, 0666);
        if (shm_fd == -1)
        {
            printf("Cons: Error en memoria compartida: %s\n", strerror(errno));
            exit(1);
        }

        shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                        shm_fd, 0);
        if (shm_base == MAP_FAILED)
        {
            printf("Cons: Fallo en el mapeo: %s\n", strerror(errno));
            exit(1);
        }

        //Semaforo espera o --
        if (sem_wait(sem_c) == -1)
        {
            perror("sem_wait: sem");
            exit(1);
        }
        //zona critica
        char *tmp_json = read_buffer(shm_base);
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

        //zona critica
        //semafaro ++
        if (sem_post(sem_p) == -1)
        {
            perror("sem_post: sem");
            exit(1);
        }
    }
}
