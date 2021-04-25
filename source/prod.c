#include "../headers/cJSON.h"
#include "../headers/prod.h"

// l: largo del buffer
// n: nombre del buffer
// m: media de la distribucion
int main(int argc, char **argv)
{
    char *buffer_name = NULL;

    int buffer_len, dist_med;

    int opt = 0;

    sem_t *sem;//semaforo

    while ((opt = getopt(argc, argv, "l:n:m:")) != -1)
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

        default:
            break;
        }
    }

    buffer_len = (buffer_len * 82) + 415;

    char *buffer_name_tmp = (char *)malloc((strlen(buffer_name) + 1) *
                                           sizeof(char));
    strcpy(buffer_name_tmp, "/");
    strcat(buffer_name_tmp, buffer_name);
    strcpy(buffer_name, buffer_name_tmp);
    free(buffer_name_tmp);

    int shm_fd;
    char *shm_base;
    //semaforo open 
    if ((sem = sem_open ("/SemafProd", O_CREAT, 0660, 1)) == SEM_FAILED) {
        perror ("sem_open"); exit (1);
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
    if(sem_wait(sem) == -1){
        perror("sem_wait: sem");exit(1);
    }   
//zona critica
    char *tmp_json = write_buffer(shm_base);
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
    if (sem_post (sem) == -1) {
	    perror ("sem_post: sem"); exit (1);}
    return 0;
}

char *write_buffer(char *sh_json)
{
    cJSON *json = cJSON_Parse(sh_json);
    int index, id_prod, timestamp, num_msg, msg_tot, nxt_write;
    index = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "nxt_write"));
    msg_tot = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "msg_tot"));
    id_prod = (int)getpid();
    timestamp = time(NULL);
    num_msg = id_prod % 7;
    char *str_msg = "Holiguis";

    /* -- Meter cosa de semaforo :v -- */

    cJSON_SetNumberValue(cJSON_GetObjectItem(cJSON_GetArrayItem(
                                                 cJSON_GetObjectItem(json,
                                                                     "buffer"),
                                                 index),
                                             "id_prod"),
                         id_prod);

    cJSON_SetNumberValue(cJSON_GetObjectItem(cJSON_GetArrayItem(
                                                 cJSON_GetObjectItem(json,
                                                                     "buffer"),
                                                 index),
                                             "timestamp"),
                         timestamp);

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

    return cJSON_Print(json);
}
