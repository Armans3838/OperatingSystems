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

using namespace std;

void* consumer(void* args){
    struct args* arguments = (struct args*)args;
    struct shmid_ds buf;
    BUFFER temp;
    int shmid =  arguments->shmid_;
    int nitems = arguments->nitems_;

    BUFFER* ptr = (BUFFER*)arguments->ptr_;

    unsigned short cksum1, cksum2;

    for(int i = 0; i < nitems; i++){
        // ENTERING CRITICAL SECTION
        sem_wait(&full);

        pthread_mutex_lock(&mutex);

        // read item
        temp = ptr[i];

        printf("Consumer: Data consumed.\n");

        /* calculate checksum */
        cksum2 = checksum(&temp.data[0], 30);

        //copy checksum to shared mem
        memcpy(&cksum1, &temp.cksum, 2);

        // check if checksums match
        if (cksum1 != cksum2){
            printf("Checksum mismatch: expected %02x, received %02x \n", cksum2, cksum1);
            exit(1);
        }
        printf("CHECKSUM SUCCESS\n");

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        // LEAVING CRITICAL SECTION
    }
    return NULL;
}