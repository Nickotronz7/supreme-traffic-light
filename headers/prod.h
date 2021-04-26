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
#include <stdbool.h>
#include <semaphore.h>

char *write_buffer(char *sh_json);

void auto_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
                 char *buffer_name, int buffer_len);
void manual_mode(sem_t *sem_p, sem_t *sem_c,int buffer_len_sem,
                 char *buffer_name, int buffer_len);

bool included = false;
bool alive = true;
int msg_prod, ac_wait_time, ac_wait_time_sem, t_ker;

#endif