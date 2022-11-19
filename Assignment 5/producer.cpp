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

#include "support.h"
#include "checksum.cpp"

using namespace std;

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

        printf("Producer: Data written.\n");
        /* calculate checksum */
        cksum = checksum(&temp.data[0], 30);

        // ENTERING CRITICAL SECTION
        sem_wait(&empty);

        pthread_mutex_lock(&mutex);

        //copy checksum to shared mem
        memcpy(&temp.cksum, &cksum, 2);

        //add item
        ptr[i] = temp;

        pthread_mutex_unlock(&mutex);

        sem_post(&full);
        // LEAVING CRITICAL SECTION
    }

    return NULL;
}

// void* consumer(void* args){
//     struct args* arguments = (struct args*)args;
//     struct shmid_ds buf;
//     BUFFER temp;
//     int shmid =  arguments->shmid_;
//     int nitems = arguments->nitems_;

//     BUFFER* ptr = (BUFFER*)arguments->ptr_;

//     unsigned short cksum1, cksum2;

//     for(int i = 0; i < nitems; i++){
//         // ENTERING CRITICAL SECTION
//         sem_wait(&full);

//         pthread_mutex_lock(&mutex);

//         // read item
//         temp = ptr[i];

//         printf("Consumer: Data consumed.\n");

//         /* calculate checksum */
//         cksum2 = checksum(&temp.data[0], 30);

//         //copy checksum to shared mem
//         memcpy(&cksum1, &temp.cksum, 2);

//         // check if checksums match
//         if (cksum1 != cksum2){
//             printf("Checksum mismatch: expected %02x, received %02x \n", cksum2, cksum1);
//             exit(1);
//         }
//         printf("CHECKSUM SUCCESS\n");

//         pthread_mutex_unlock(&mutex);
//         sem_post(&empty);
//         // LEAVING CRITICAL SECTION
//     }
//     return NULL;
// }