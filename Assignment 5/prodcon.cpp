#include <vector>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <cstring>
#include <stdint.h>

#include "producer.cpp"
#include "consumer.cpp"

#define FTOK_FILE          "ftok.file"
#define FTOK_ID            1

using namespace std;

int main(int argc, char const *argv[])
{
    if(argc < 2){
        printf("Missing arguments, please try again.\n");
        exit(1);
    }

    int nitems = atoi(argv[1]);
    srand(time(NULL)); // set to current time

    if(nitems < 1 || nitems > 2000){
        printf("nitems (32 bytes each) must be between 1 and 2000. (64k memory limit)\n");
        exit(1);
    }

    sem_init(&full, 0, 0);
    sem_init(&empty, 0, nitems);
    pthread_mutex_init(&mutex, NULL);


    //create shared memory
    int shmid = shmget(ftok(FTOK_FILE, FTOK_ID), nitems*32, IPC_CREAT | 0666);
    if (shmid < 0) {
        printf("Error: creating shared memory, errno = %d \n", errno);
        return -1;
    }

    //attach shared memory to pointer
    BUFFER* ptr = (BUFFER*)(shmat(shmid, NULL, 0));
    if (ptr == NULL) {
        printf("Error: attaching to shared memory, errno = %d \n", errno);
        return -1;
    }

    pthread_t prod;
    pthread_t con;

    args a;
    a.nitems_ = nitems;
    a.shmid_ = shmid;
    a.ptr_ = ptr;

    pthread_create(&prod, NULL, producer, &a);
    pthread_create(&con, NULL, consumer, &a);

    pthread_join(prod, NULL);
    pthread_join(con, NULL);

    //remove shared memory
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}