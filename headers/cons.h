#ifndef CONS_H
#define CONS_H

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
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

char *read_buffer(char *sh_json);

void auto_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
               char *buffer_name, int buffer_len, int dist_med);

void manual_mode(sem_t *sem_p, sem_t *sem_c, int buffer_len_sem,
                 char *buffer_name, int buffer_len);

void kill();

bool included = false;
bool alive = true;
int msg_cons, ac_wait_time, ac_wait_time_sem, t_ker;

#endif