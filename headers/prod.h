#ifndef PROD_H
#define PROD_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

char *write_buffer(char *sh_json);

void auto_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
               char *buffer_name, int buffer_len, int dist_med);
void manual_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
                 char *buffer_name, int buffer_len);

void kill();

bool included = false;
bool alive = true;
int msg_prod;
double ac_wait_time = 0;
double ac_wait_time_sem = 0;
double t_ker;

#endif