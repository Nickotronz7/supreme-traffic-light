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

char *makeJson(int buffer_size);

int main(int argc, char **argv)
{
    char *buffer_name = NULL;

    int buffer_len, buffer_len_init;

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

    buffer_len_init = buffer_len;
    
    char *buffer_name_tmp = (char *)malloc((strlen(buffer_name) + 1) * sizeof(char));
    strcpy(buffer_name_tmp, "/");
    strcat(buffer_name_tmp, buffer_name);
    strcpy(buffer_name, buffer_name_tmp);
    free(buffer_name_tmp);

    // len y=82x+415

    char *json = makeJson(buffer_len);
    buffer_len = (int)(strlen(json));

    int shm_fd;
    char *shm_base;

    shm_fd = shm_open(buffer_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        printf("Init: Fallo en memoria compartida: %s\n", strerror(errno));
        exit(1);
    }

    ftruncate(shm_fd, buffer_len);

    shm_base = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
        printf("Init: Mapeo fallido: %s\n", strerror(errno));
        exit(1);
    }

    json = shm_base;
    json += sprintf(json, "%s", makeJson(buffer_len_init));

    // if (munmap(shm_base, buffer_len) == -1)
    // {
    //     printf("Init: Fallo unmap: %s\n", strerror(errno));
    //     exit(1);
    // }

    // if (close(shm_fd) == -1){
    //     printf("Init: Error en el close: %s\n", strerror(errno));
    // }

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

    // printf("%s\n", cJSON_Print(jsonRoot));

    return cJSON_Print(jsonRoot);
}
