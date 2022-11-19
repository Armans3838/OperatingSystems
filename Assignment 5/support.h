struct args{
    int nitems_;
    int shmid_;
    void* ptr_;
};

typedef struct{
    u_int8_t data[30];
    u_int16_t cksum;
} BUFFER;

sem_t full;
sem_t empty;
pthread_mutex_t mutex;