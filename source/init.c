#include "../headers/cJSON.h"
#include "../headers/init.h"

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

    // len y=82x+415
    buffer_len_init = (82 * buffer_len) + 415;

    char *buffer_name_tmp = (char *)malloc((strlen(buffer_name) + 1) * sizeof(char));
    strcpy(buffer_name_tmp, "/");
    strcat(buffer_name_tmp, buffer_name);
    strcpy(buffer_name, buffer_name_tmp);
    free(buffer_name_tmp);

    int shm_fd;
    char *shm_base;

    shm_fd = shm_open(buffer_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        printf("Init: Fallo en memoria compartida: %s\n", strerror(errno));
        exit(1);
    }

    ftruncate(shm_fd, buffer_len_init);

    shm_base = mmap(0, buffer_len_init, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
        printf("Init: Mapeo fallido: %s\n", strerror(errno));
        exit(1);
    }

    char *json = shm_base;
    json += sprintf(json, "%s", makeJson(buffer_len));

    return 0;
}

char *makeJson(int buffer_size)
{
    cJSON *jsonRoot = NULL;
    cJSON *buffer = NULL;
    jsonRoot = cJSON_CreateObject();

    if (NULL == jsonRoot)
    {
        printf("Error al crear el root del json");
    }

    cJSON_AddNumberToObject(jsonRoot, "msg_tot", 0);
    cJSON_AddNumberToObject(jsonRoot, "nxt_write", 0);
    cJSON_AddNumberToObject(jsonRoot, "nxt_read", 0);
    cJSON_AddNumberToObject(jsonRoot, "buffer_size", buffer_size);

    buffer = cJSON_CreateArray();
    cJSON *array_Element = NULL;
    cJSON_AddItemToObject(jsonRoot, "buffer", buffer);
    for (size_t i = 0; i < buffer_size; i++)
    {
        array_Element = cJSON_CreateObject();
        cJSON_AddItemToArray(buffer, array_Element);
        cJSON_AddNumberToObject(array_Element, "id_prod", 0);
        cJSON_AddNumberToObject(array_Element, "timestamp", 0);
        cJSON_AddNumberToObject(array_Element, "num_msg", 0);
        cJSON_AddStringToObject(array_Element, "str_msg", "00000");
    }

    cJSON_AddNumberToObject(jsonRoot, "prod_tot", 0);
    cJSON_AddNumberToObject(jsonRoot, "prod_viv", 0);
    cJSON_AddNumberToObject(jsonRoot, "cons_tot", 0);
    cJSON_AddNumberToObject(jsonRoot, "cons_viv", 0);
    cJSON_AddNumberToObject(jsonRoot, "cons_key", 0);
    cJSON_AddNumberToObject(jsonRoot, "wait_t", 0);
    cJSON_AddNumberToObject(jsonRoot, "block_t", 0);
    cJSON_AddNumberToObject(jsonRoot, "user_t", 0);
    cJSON_AddNumberToObject(jsonRoot, "kernel_t", 0);

    return cJSON_Print(jsonRoot);
}
