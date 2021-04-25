#include "../headers/cJSON.h"
#include "../headers/cons.h"

// l: largo del buffer
// n: nombre del buffer
// m: media de la distribucion
int main(int argc, char **argv)
{
    char *buffer_name = NULL;

    int buffer_len, dist_med,buffer_len_sem;

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
    buffer_len_sem=buffer_len;
    buffer_len = (buffer_len * 82) + 415;

    char *buffer_name_tmp = (char *)malloc((strlen(buffer_name) + 1) * sizeof(char));
    strcpy(buffer_name_tmp, "/");
    strcat(buffer_name_tmp, buffer_name);
    strcpy(buffer_name, buffer_name_tmp);
    free(buffer_name_tmp);

    int shm_fd;
    char *shm_base;
    //semaforo open 
    if ((sem = sem_open ("/SemafCons", O_CREAT, 0660, buffer_len_sem)) == SEM_FAILED) {
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
    if (sem_post (sem) == -1) {
	    perror ("sem_post: sem"); exit (1);}
    return 0;
}

char *read_buffer(char *sh_json)
{
    cJSON *json = cJSON_Parse(sh_json);
    int index, prod_id, timestamp, rnd_key, prod_viv, cons_viv;
    char *msg_read;

    /* -- Meter cosa de semaforo :v -- */

    index = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "nxt_read"));
    prod_id = cJSON_GetNumberValue(
        cJSON_GetObjectItem(cJSON_GetArrayItem(
                                cJSON_GetObjectItem(json,
                                                    "buffer"),
                                index),
                            "id_prod"));
    timestamp = cJSON_GetNumberValue(
        cJSON_GetObjectItem(cJSON_GetArrayItem(
                                cJSON_GetObjectItem(json,
                                                    "buffer"),
                                index),
                            "timestamp"));

    rnd_key = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "cons_key"));

    prod_viv = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "prod_viv"));

    cons_viv = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "cons_viv"));

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
    printf("    Identificador del Productor: %i,\n", prod_id);
    printf("    Timestamp: %i,\n", timestamp);
    printf("    Llave random: %i,\n", rnd_key);
    printf("    Productores vivos: %i,\n", prod_viv);
    printf("    Consumidores vivos: %i\n", cons_viv);
    printf("}\n");
    return cJSON_Print(json);
}