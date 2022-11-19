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

uint16_t checksum(uint8_t *addr, uint32_t count)
{
    uint32_t sum = 0;
    uint16_t *buf = (uint16_t *) addr;

    // Main summing loop
    while(count > 1)
    {
        sum = sum + *(buf)++;
        count = count - 2;
    }

    if (count > 0)
        sum = sum + *addr;

    while (sum>>16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return(~sum);
}