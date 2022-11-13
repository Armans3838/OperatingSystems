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

#define FTOK_FILE          "ftok.file"
#define FTOK_ID            1

using namespace std;

sem_t full;
sem_t empty;
pthread_mutex_t mutex;

typedef struct{
    u_int8_t data[30];
    u_int16_t cksum;
} BUFFER;

// struct for passing data to threads
struct args{
    int nitems_;
    int shmid_;
    void* ptr_;
};

// function to calculate checksum
uint16_t checksum(uint8_t *addr, uint32_t count)
{
    uint32_t sum = 0;
    uint16_t *buf = (uint16_t *) addr;

    while(count > 1)
    {
        sum = sum + *(buf)++;
        count = count - 2;
    }

    // Add left-over byte, if any
    if (count > 0)
        sum = sum + *addr;

    // Fold 32-bit sum to 16 bits
    while (sum>>16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return(~sum);
}

void* producer(void* args){
    struct args* arguments = (struct args*)args;
    struct shmid_ds buf;
    BUFFER temp;
    int shmid =  arguments->shmid_;
    int nitems = arguments->nitems_;

    BUFFER* ptr = (BUFFER*)arguments->ptr_;

    unsigned short cksum; 

    for(int i = 0; i < nitems; i++){
        //write random data to buffer
        for (int j = 0; j < 30; j++) {
        temp.data[j] = (u_int8_t) (rand() % 256);
        }
        printf("Prod: Data written.\n");
        /* calculate checksum */
        cksum = checksum(&temp.data[0], 30);

        // CRITICAL SECTION
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        //copy checksum to shared mem
        memcpy(&temp.cksum, &cksum, 2);

        //add item
        ptr[i] = temp;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        // CRITICAL SECTION OVER
    }

    return NULL;
}


void* consumer(void* args){
    struct args* arguments = (struct args*)args;
    struct shmid_ds buf;
    BUFFER temp;
    int shmid =  arguments->shmid_;
    int nitems = arguments->nitems_;

    BUFFER* ptr = (BUFFER*)arguments->ptr_;

    unsigned short cksum1, cksum2;

    for(int i = 0; i < nitems; i++){
        // CRITICAL SECTION
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        //read item
        temp = ptr[i];
        printf("Con: Data consumed.\n");

        /* calculate checksum */
        cksum2 = checksum(&temp.data[0], 30);

        //copy checksum to shared mem
        memcpy(&cksum1, &temp.cksum, 2);
        if (cksum1 != cksum2){
            printf("Checksum mismatch: expected %02x, received %02x \n", cksum2, cksum1);
            exit(1);
        }
        printf("CHECKSUM SUCCESS\n");

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        // CRITICAL SECTION OVER
    }
    return NULL;
}


int main(int argc, char const *argv[])
{
    if(argc < 2){
        printf("Invalid number of arguments.\n");
        exit(1);
    }

    int nitems = atoi(argv[1]);
    srand(time(NULL)); // set time to current time

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
        printf("Error: creating shared memory, errno = %d \n", errno); // error handling
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

    args a = {nitems, shmid, ptr};

    // create args struct 
    // args a;
    // a.nitems_ = nitems;
    // a.shmid_ = shmid;
    // a.ptr_ = ptr;

    // create threads and pass in args
    pthread_create(&prod, NULL, producer, &a);
    pthread_create(&con, NULL, consumer, &a);

    // wait for threads to finish and then joiing
    pthread_join(prod, NULL);
    pthread_join(con, NULL);

    //remove shared memory after done with it
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
