#include "../headers/cJSON.h"
#include "../headers/prod.h"
#include "../headers/expo.h"

// l: largo del buffer
// n: nombre del buffer
// m: media de la distribucion
int main(int argc, char **argv)
{
    char *buffer_name = NULL;

    int buffer_len, dist_med, buffer_len_sem, mode;

    int opt = 0;

    sem_t *sem_c; //semaforo
    sem_t *sem_p; //semaforo

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

    char *buffer_name_tmp = (char *)malloc((strlen(buffer_name) + 1) *
                                           sizeof(char));
    strcpy(buffer_name_tmp, "/");
    strcat(buffer_name_tmp, buffer_name);
    strcpy(buffer_name, buffer_name_tmp);
    free(buffer_name_tmp);

    if (mode)
    {
        auto_mode(sem_p, sem_c, buffer_len_sem, buffer_name, buffer_len, dist_med);
    }
    else
    {
        manual_mode(sem_p, sem_c, buffer_len_sem, buffer_name, buffer_len);
    }

    return 0;
}

char *write_buffer(char *sh_json)
{

    cJSON *json = cJSON_Parse(sh_json);
    if (!included)
    {
        cJSON_SetNumberValue(cJSON_GetObjectItem(json, "prod_tot"),
                             cJSON_GetNumberValue(cJSON_GetObjectItem(
                                 json, "prod_tot")) +
                                 1);

        cJSON_SetNumberValue(cJSON_GetObjectItem(json, "prod_viv"),
                             cJSON_GetNumberValue(cJSON_GetObjectItem(
                                 json, "prod_viv")) +
                                 1);

        included = true;
    }

    int index, id_prod, num_msg, msg_tot, nxt_write, prod_viv, cons_viv;
    index = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "nxt_write"));
    msg_tot = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "msg_tot"));
    id_prod = (int)getpid();
    time_t timestamp = time(NULL);
    num_msg = id_prod % 7;
    char *str_msg = "Holiguis";
    char *timestamp_string = ctime(&timestamp);

    prod_viv = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "prod_viv"));

    cons_viv = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "cons_viv"));

    cJSON_SetNumberValue(cJSON_GetObjectItem(cJSON_GetArrayItem(
                                                 cJSON_GetObjectItem(json,
                                                                     "buffer"),
                                                 index),
                                             "id_prod"),
                         id_prod);

    cJSON_SetValuestring(cJSON_GetObjectItem(cJSON_GetArrayItem(
                                                 cJSON_GetObjectItem(json,
                                                                     "buffer"),
                                                 index),
                                             "timestamp"),
                         timestamp_string);

    cJSON_SetNumberValue(cJSON_GetObjectItem(cJSON_GetArrayItem(
                                                 cJSON_GetObjectItem(json,
                                                                     "buffer"),
                                                 index),
                                             "num_msg"),
                         num_msg);

    cJSON_SetValuestring(cJSON_GetObjectItem(cJSON_GetArrayItem(
                                                 cJSON_GetObjectItem(json,
                                                                     "buffer"),
                                                 index),
                                             "str_msg"),
                         str_msg);

    cJSON_SetNumberValue(cJSON_GetObjectItem(json, "msg_tot"), msg_tot + 1);

    if (index + 1 == cJSON_GetNumberValue(cJSON_GetObjectItem(json,
                                                              "buffer_size")))
    {
        cJSON_SetNumberValue(cJSON_GetObjectItem(json, "nxt_write"), 0);
    }
    else
    {
        cJSON_SetNumberValue(cJSON_GetObjectItem(json, "nxt_write"), index + 1);
    }

    // imprimir mensaje
    printf("%s    +---------------------------------+\n",KGRN);
    printf("%s    Se a logrado una escritura en buffer!!\n",KGRN);
    printf("%s    Escrito en: %s%i,\n",KGRN,KWHT, index);
    printf("%s    Productores vivos: %s%i,\n",KGRN,KWHT, prod_viv);
    printf("%s    Consumidores vivos: %s%i\n",KGRN,KWHT, cons_viv);
    printf("%s    +---------------------------------+\n",KGRN);

    return cJSON_Print(json);
}

void auto_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
               char *buffer_name, int buffer_len, int dist_med)
{
    double tSleep;
    while (alive)
    {
        /* meta la probabilidad aqui */
        tSleep = ran_expo(dist_med);
        sleep(tSleep);
        printf("El proceso durmio %f segundos", tSleep);
        //semaforo open
        int shm_fd;
        char *shm_base;
        if ((sem_p = sem_open(
                 "/SemafProd", O_CREAT, 0660, buffer_len_sem)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        if ((sem_c = sem_open(
                 "/SemafCons", O_CREAT, 0660, 0)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        //-------
        shm_fd = shm_open(buffer_name, O_RDWR, 0666);
        if (shm_fd == -1)
        {
            printf("Prod: Error en memoria compartida: %s\n", strerror(errno));
            exit(1);
        }

        shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                        shm_fd, 0);
        if (shm_base == MAP_FAILED)
        {
            printf("Prod: Fallo en el mapeo: %s\n", strerror(errno));
            exit(1);
        }
        //Semaforo espera o --
        if (sem_wait(sem_p) == -1)
        {
            perror("sem_wait: sem");
            exit(1);
        }
        //zona critica
        char *tmp_json, *key;
        cJSON *json = cJSON_Parse(shm_base);

        if (cJSON_GetObjectItem(json, "covid"))
            key = "end";

        if (!strcmp(key, "end"))
        {
            alive = !alive;
            cJSON_SetNumberValue(cJSON_GetObjectItem(json, "cons_viv"),
                                 (cJSON_GetNumberValue(
                                     cJSON_GetObjectItem(json, "cons_viv"))) -
                                     1);
            tmp_json = cJSON_Print(json);
            free(json);
            buffer_len = (int)(strlen(tmp_json));
        }
        else
        {
            free(json);
            tmp_json = write_buffer(shm_base);
            buffer_len = (int)(strlen(tmp_json));
        }

        shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                        shm_fd, 0);
        if (shm_base == MAP_FAILED)
        {
            printf("Prod: Fallo en el mapeo: %s\n", strerror(errno));
            exit(1);
        }

        for (size_t i = 0; i < buffer_len; i++)
        {
            *(shm_base + i) = *(tmp_json + i);
        }
        //zona critica
        //semafaro ++
        if (sem_post(sem_c) == -1)
        {
            perror("sem_post: sem");
            exit(1);
        }
    }
}

void manual_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
                 char *buffer_name, int buffer_len)
{
    char key[3];
    while (alive)
    {
        printf("%sPresione 'ENTER' para generar un mensaje \n",KCYN);
        scanf("%s", key);

        //semaforo open
        int shm_fd;
        char *shm_base;
        if ((sem_p = sem_open(
                 "/SemafProd", O_CREAT, 0660, buffer_len_sem)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        if ((sem_c = sem_open(
                 "/SemafCons", O_CREAT, 0660, 0)) == SEM_FAILED)
        {
            perror("sem_open");
            exit(1);
        }
        //-------
        shm_fd = shm_open(buffer_name, O_RDWR, 0666);
        if (shm_fd == -1)
        {
            printf("Prod: Error en memoria compartida: %s\n", strerror(errno));
            exit(1);
        }

        shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                        shm_fd, 0);
        if (shm_base == MAP_FAILED)
        {
            printf("Prod: Fallo en el mapeo: %s\n", strerror(errno));
            exit(1);
        }
        //Semaforo espera o --
        if (sem_wait(sem_p) == -1)
        {
            perror("sem_wait: sem");
            exit(1);
        }
        //zona critica

        char *tmp_json;
        cJSON *json = cJSON_Parse(shm_base);

        if (cJSON_GetObjectItem(json, "covid"))
        {
            // key = "end";
            key[0] = 'e';
            key[1] = 'n';
            key[2] = 'd';
        }

        if (!strcmp(key, "end"))
        {
            alive = !alive;
            cJSON_SetNumberValue(cJSON_GetObjectItem(json, "cons_viv"),
                                 (cJSON_GetNumberValue(
                                     cJSON_GetObjectItem(json, "cons_viv"))) -
                                     1);
            tmp_json = cJSON_Print(json);
            free(json);
            buffer_len = (int)(strlen(tmp_json));
        }
        else
        {
            free(json);
            tmp_json = write_buffer(shm_base);
            buffer_len = (int)(strlen(tmp_json));
        }

        shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED,
                        shm_fd, 0);
        if (shm_base == MAP_FAILED)
        {
            printf("Prod: Fallo en el mapeo: %s\n", strerror(errno));
            exit(1);
        }

        for (size_t i = 0; i < buffer_len; i++)
        {
            *(shm_base + i) = *(tmp_json + i);
        }
        //zona critica
        //semafaro ++
        if (sem_post(sem_c) == -1)
        {
            perror("sem_post: sem");
            exit(1);
        }
    }
    if (!alive)
        kill();
}

void kill()
{
    printf("%sMe dio %sCOVID%s, me mori y esto fue lo que hice:\n\n",KGRN,KRED,KGRN);
    printf("%smensajes producidos: %s%i\n",KGRN,KWHT ,msg_prod);
    printf("%sAcumulado de tiempo esperado: %s%i\n",KGRN,KWHT, ac_wait_time);
    printf("%sAcumulado de tiempo bloqueado por semaforos: %s%i\n",KGRN,KWHT, ac_wait_time_sem);
    exit(0);
}
