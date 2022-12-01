#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algorithms.c"

int main(int argc, char const *argv[])
{
    // checking that there is both a position and file passed in
	if (argc < 3) {
        perror("Error: not enough arguments. Missing position and/or file.\n");
        exit(EXIT_FAILURE);
    }

    int cylinderRequests[SIZE];
    int initialHeadPosition = atoi(argv[1]);
    int i = 0;
    FILE *passedFile;

    // read each cylinder from our file and store it in our array and error handling for file opening and reading
    if ((passedFile= fopen(argv[2], "r"))) {
        while (fscanf(passedFile, "%d", &cylinderRequests[i]) != EOF) {
            i++;
        }
        fclose(passedFile);
    } else {
        perror("Error opening file!");
        exit(EXIT_FAILURE);
    }

    printf("\nFCFS:\n");
    fcfs(cylinderRequests, initialHeadPosition);

    printf("SSTF:\n");
    sstf(cylinderRequests, initialHeadPosition);

    printf("SCAN:\n");
    scan(cylinderRequests, initialHeadPosition);

    printf("CSCAN:\n");
    cscan(cylinderRequests, initialHeadPosition);
    return 0;
}