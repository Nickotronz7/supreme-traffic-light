#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SEMAPHORES 1
// New type definitions
typedef struct {
    long timestamp;
    char buff[256];
} PC_DATA;

/*--- Shared data ---*/
PC_DATA *shared;

int main(int argc, char* argv[]){
    
    int mem_desc; // shared memory descriptor
    int sem_desc;
    sem_t *sem;

    // Create shared mem in /dev/shm
    mem_desc = shm_open("/holis",
                  O_CREAT | O_RDWR,   // Create for R/W operations
                  S_IRUSR | S_IWUSR); // Give me R/W permission
    ftruncate(mem_desc, sizeof(PC_DATA));   // Set the shared memory size
    sem = sem_open ("/Semaf", O_CREAT | O_EXCL, 0644, 1);
    
    // Map (or "attach") the shared memory to an address in my process
    shared = mmap(NULL, sizeof(PC_DATA), 
            PROT_READ | PROT_WRITE,     // Enable Read/Write access
            MAP_SHARED,                 // Share among processes
            mem_desc, 0);   

    sem_wait(sem);
    shared->buff[0] =5;
    printf("hola a %d\n", shared->buff[0]);
    sleep(10);
    sem_post(sem);
    sem_unlink ("Semaf"); 

    // sem_post(semaforo);
}