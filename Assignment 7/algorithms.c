#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int SIZE = 1000;

void getTime(float startTime, float endTime) {
    float timeElapsed = (endTime - startTime)/CLOCKS_PER_SEC;
    printf("Time elapsed (seconds): %f\n", timeElapsed);
}

// FCFS algorithm
void fcfs (int cylinderRequests[], int initialHeadPosition) {
    int curHeadPosition = initialHeadPosition;
    int totalCylinderRequests = 0;
    int totalHeadMovement = 0;
    clock_t startTime = clock();
    do {
        int diff = abs(curHeadPosition - cylinderRequests[totalCylinderRequests]);
        totalHeadMovement += diff;
        curHeadPosition = cylinderRequests[totalCylinderRequests];
        totalCylinderRequests++;
    } while (totalCylinderRequests < SIZE);
    clock_t endTime = clock();
    printf("total cylinder requests: %i\n", totalCylinderRequests);
    printf("total head movement: %i\n", totalHeadMovement);
    printf("avg head movement per request: %0.2f\n", (double)totalHeadMovement/SIZE);
    getTime(startTime, endTime);
    printf("\n");
}

// algorithm to find and return the index with the shortest seek time from the current head position.
int findShortestSeekTime(int cylinderRequests[], int curHeadPosition) {
    int curIndex = 0;
    int sstIndex = 0;
    int sst = INT32_MAX;
    do {
        if (cylinderRequests[curIndex] < 0) {
            curIndex++;
            continue;
        }
        int diff = abs(curHeadPosition - cylinderRequests[curIndex]);
        if (diff < sst) {
            sst = diff;
            sstIndex = curIndex;
        }
        curIndex++;
    } while (curIndex < SIZE);
    return sstIndex;
}

// SSTF algorithm
void sstf(int cylinderRequests[], int initialHeadPosition) {
    // copy elements to a new array
    int* seenCylRequests = malloc(sizeof(*cylinderRequests));
    for (int i = 0; i < SIZE; ++i) {
        seenCylRequests[i] = cylinderRequests[i];
    }
    int curHeadPosition = initialHeadPosition;
    int totalCylinderRequests = 0;
    int totalHeadMovement = 0; 
    int sstIndex;
    clock_t startTime = clock();
    do {
        sstIndex = findShortestSeekTime(seenCylRequests, curHeadPosition);
        int diff = abs(curHeadPosition - seenCylRequests[sstIndex]);
        totalHeadMovement += diff;
        curHeadPosition = seenCylRequests[sstIndex];
        seenCylRequests[sstIndex] = -1;
        totalCylinderRequests++;
    } while (totalCylinderRequests < SIZE);
    clock_t endTime = clock();
    printf("total cylinder requests: %i\n", totalCylinderRequests);
    printf("total head movement: %i\n", totalHeadMovement);
    printf("avg head movement per request: %0.2f\n", (double)totalHeadMovement/SIZE);
    getTime(startTime, endTime);
    printf("\n");
}

// finds and returns the index of the closest cylinder that is smaller than the current
int findNextAddressSmaller(int cylinderRequests[], int curHeadPosition) {
    int curIndex = 0;
    int sstIndex = 0;
    int sst = INT32_MAX;
    do {
        if (cylinderRequests[curIndex] < 0 || cylinderRequests[curIndex] > curHeadPosition) {
            curIndex++;
            continue;
        }
        int diff = abs(curHeadPosition - cylinderRequests[curIndex]);
        if (diff < sst) {
            sst = diff;
            sstIndex = curIndex;
        }
        curIndex++;
    } while (curIndex < SIZE);
    if (sstIndex == INT32_MAX)
        sstIndex = -1;
    return sstIndex;
}

// finds and returns the index of the closest cylinder that is bigger than the current
int findNextAddressBigger(int cylinderRequests[], int curHeadPosition) {
    int curIndex = 0;
    int sstIndex = 0;
    int sst = INT32_MAX;
    do {
        if (cylinderRequests[curIndex] < 0 || cylinderRequests[curIndex] < curHeadPosition) {
            curIndex++;
            continue;
        }
        int diff = abs(curHeadPosition - cylinderRequests[curIndex]);
        if (diff < sst) {
            sst = diff;
            sstIndex = curIndex;
        }
        curIndex++;
    } while (curIndex < SIZE);
    if (sstIndex == INT32_MAX)
        sstIndex = -1;
    return sstIndex;
}

// SCAN algorithm
void scan(int cylinderRequests[], int initialHeadPosition) {
    // copy elements to a new array
    int* seenCylRequests = malloc(sizeof(*cylinderRequests));
    for (int i = 0; i < SIZE; ++i) {
        seenCylRequests[i] = cylinderRequests[i];
    }
    int curHeadPosition = initialHeadPosition;
    int totalCylinderRequests = 0;
    int totalHeadMovement = 0; 
    int sstIndex;
    int isGoingToZero = 1; // 1 is true, 0 is false
    clock_t startTime = clock();
    do {
        if (isGoingToZero == 1) {
            sstIndex = findNextAddressSmaller(seenCylRequests, curHeadPosition);
            if (sstIndex == -1)
                isGoingToZero = 0;
        } else {
            sstIndex = findNextAddressBigger(seenCylRequests, curHeadPosition);
        }
        int diff = abs(curHeadPosition - seenCylRequests[sstIndex]);
        totalHeadMovement += diff;
        curHeadPosition = seenCylRequests[sstIndex];
        seenCylRequests[sstIndex] = -1;
        totalCylinderRequests++;
    } while (totalCylinderRequests < SIZE);
    clock_t endTime = clock();
    printf("total cylinder requests: %i\n", totalCylinderRequests);
    printf("total head movement: %i\n", totalHeadMovement);
    printf("avg head movement per request: %0.2f\n", (double)totalHeadMovement/SIZE);
    getTime(startTime, endTime);
    printf("\n");
}

// C-SCAN algorithm
void cscan(int cylinderRequests[], int initialHeadPosition) {
    // copy elements to a new array
    int* seenCylRequests = malloc(sizeof(*cylinderRequests));
    for (int i = 0; i < SIZE; ++i) {
        seenCylRequests[i] = cylinderRequests[i];
    }
    int curHeadPosition = initialHeadPosition;
    int totalCylinderRequests = 0;
    int totalHeadMovement = 0; 
    int sstIndex;
    clock_t startTime = clock();
    do {
        sstIndex = findNextAddressBigger(seenCylRequests, curHeadPosition);
        if (sstIndex == -1) {
            curHeadPosition = 0;
            continue;
        }
        int diff = abs(curHeadPosition - seenCylRequests[sstIndex]);
        totalHeadMovement += diff;
        curHeadPosition = seenCylRequests[sstIndex];
        seenCylRequests[sstIndex] = -1;
        totalCylinderRequests++;
    } while (totalCylinderRequests < SIZE);
    clock_t endTime = clock();
    printf("total cylinder requests: %i\n", totalCylinderRequests);
    printf("total head movement: %i\n", totalHeadMovement);
    printf("avg head movement per request: %0.2f\n", (double)totalHeadMovement/SIZE);
    getTime(startTime, endTime);
    printf("\n");
}