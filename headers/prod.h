#ifndef PROD_H
#define PROD_H

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

char *write_buffer(char *sh_json);

bool included = false;
int msg_prod, ac_wait_time, ac_wait_time_sem, t_ker;

#endif