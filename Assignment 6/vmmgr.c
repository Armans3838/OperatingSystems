#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>
#include <time.h>

#define FRAME_SIZE        256       // Size of each frame
#define TOTAL_FRAME_COUNT 256       // Total number of frames in physical memory
#define PAGE_MASK         0xFF00    // Masks everything but the page number
#define OFFSET_MASK       0xFF      // Masks everything but the offset
#define SHIFT             8         // Amount to shift when bitmasking
#define TLB_SIZE          16        // size of the TLB
#define PAGE_TABLE_SIZE   256       // size of the page table
#define MAX_ADDR_LEN      10        // The number of characters to read for each line from input file.
#define PAGE_READ_SIZE    256       // Number of bytes to read

typedef struct vmTable_t {
    int *pageNumArr; 
    int *frameNumArr; 
    int *entryAgeArr; 
    int length;
    int pageFaultCount;
    int tlbHitCount;
    int tlbMissCount;
} vmTable_t;

vmTable_t* tlbTable; // The TLB Structure
vmTable_t* pageTable; // The Page Table
int** dram; // Physical Memory
int nextTLBentry = 0; // Tracks the next available index of entry into the TLB
int nextPage = 0;  // Tracks the next available page in the page table
int nextFrame = 0; // Tracks the next available frame TLB or Page Table

// input file and backing store
FILE* address_file;
FILE* backing_store;

// how we store reads from input file
char addressReadBuffer[MAX_ADDR_LEN];
int virtual_addr;
int page_number;
int offset_number;

// Generating length of time for a function
clock_t start, end;
double cpu_time_used;
int functionCallCount = 0;

// the buffer containing reads from backing store
signed char fileReadBuffer[PAGE_READ_SIZE];

// the translatedValue of the byte (signed char) in memory
signed char translatedValue;

// Function Prototypes
void translateAddress();
void readFromStore(int pageNumber);
void tlbFIFOinsert(int pageNumber, int frameNumber);
int getOldestEntry(int tlbSize);
double getAvgTimeInBackingStore();

vmTable_t* makeVMtable(int length)
{
    vmTable_t* new_table = malloc(sizeof(vmTable_t));
    new_table->length = length;
    new_table->pageNumArr = malloc(sizeof(int) * length);
    new_table->frameNumArr = malloc(sizeof(int) * length);
    new_table->entryAgeArr = malloc(sizeof(int) * length);
    new_table->pageFaultCount = 0;
    new_table->tlbHitCount = 0;
    new_table->tlbMissCount = 0;

    for (int i = 0; i < length; i++) {
        new_table->pageNumArr[i] = 0;
    }

    // If there is not enough memory on the heap to make a call to malloc() // Notify and Exit
    if(new_table == NULL || new_table->pageNumArr == NULL || new_table->frameNumArr == NULL) {
        printf("Error. Couldn't allocated new virtual memory addressing table\r\n");
        exit(-1);
    }
    return new_table;
}

// Method to free dynamically allocated memory for the virtual memory table
void freeVMtable(vmTable_t** table)
{
    if ((*table)->pageNumArr != NULL) {
        free((*table)->pageNumArr);
    }
    if ((*table)->frameNumArr != NULL) {
        free((*table)->frameNumArr);
    }
    if ((*table)->entryAgeArr != NULL) {
        free((*table)->entryAgeArr);
    }
    free(*table);
}

// Method to create simulated physical memory space
int** dramAllocate(int frameCount, int blockSize)
{
    int** temp;
    temp = malloc(frameCount * sizeof(int*));
    for(int i = 0; i < frameCount; i++) {
        temp[i] = (int*)malloc(sizeof(int) * blockSize);
        for(int j = 0; j < blockSize; j++) {
            temp[i][j] = 0;
        }
    }
    // If there is not enough memory to make call to malloc() // Notify and exit
    if(temp == NULL) {
        printf("Error - Could not allocate a new Physical Memory Matrix using dramAllocate() function!\r\n");
        exit(-1);
    }
    return temp;
}

// Method to free dynamically allocated memory for the simulated physical memory space
void freeDRAM(int*** dblPtrArr, int frameCount)
{
  for (int i = 0; i < frameCount; i++) {
      if ((*dblPtrArr)[i] != NULL) {
          free((*dblPtrArr)[i]);
      }
  }
  free(*dblPtrArr);
}

// Get page number from logical address
int getPageNumber(int mask, int value, int shift) {
   return ((value & mask)>>shift);
}

// Get offset from logical address
int getOffset(int mask, int value) {
   return value & mask;
}

// main opens necessary files and calls on translateAddress for every entry in the addresses file
int main(int argc, char *argv[])
{
    tlbTable = makeVMtable(TLB_SIZE); // The TLB Structure
    pageTable = makeVMtable(PAGE_TABLE_SIZE); // The Page Table
    dram = dramAllocate(TOTAL_FRAME_COUNT, FRAME_SIZE); // Physical Memory
    int translationCount = 0;
    char* algoName;

    // perform basic error checking
    if (argc != 2) {
        fprintf(stderr,"Usage: ./vm_sim [input file]\n");
        return -1;
    }

    // open the file containing the backing store
    backing_store = fopen("BACKING_STORE.bin", "rb");

    if (backing_store == NULL) {
        fprintf(stderr, "Error opening BACKING_STORE.bin %s\n","BACKING_STORE.bin");
        return -1;
    }

    // open the file containing the logical addresses
    address_file = fopen(argv[1], "r");

    if (address_file == NULL) {
        fprintf(stderr, "Error opening %s. Expecting [InputFile].txt or equivalent.\n",argv[1]);
        return -1;
    }

    printf("\nNumber of logical pages: %d\nPage size: %d bytes\nPage Table Size: %d\nTLB Size: %d entries\nNumber of Physical Frames: %d\nPhysical Memory Size: %d bytes", PAGE_TABLE_SIZE, PAGE_READ_SIZE, PAGE_TABLE_SIZE, TLB_SIZE, FRAME_SIZE, PAGE_READ_SIZE * FRAME_SIZE);

    // Read through the input file and output each virtual address
    while (fgets(addressReadBuffer, MAX_ADDR_LEN, address_file) != NULL) {
        virtual_addr = atoi(addressReadBuffer); // converting from ascii to int

        // 32-bit masking function to extract page number
        page_number = getPageNumber(PAGE_MASK, virtual_addr, SHIFT);

        // 32-bit masking function to extract page offset
        offset_number = getOffset(OFFSET_MASK, virtual_addr);

        // Get the physical address and translatedValue stored at that address
        // translateAddress(algo_choice);
        translateAddress();
        translationCount++;  // increment the number of translated addresses
    }

    algoName = "FIFO";

    printf("\n-----------------------------------------------------------------------------------\n");
    // calculate and print out the stats
    printf("\nResults Using %s Algorithm: \n", algoName);
    printf("Number of translated addresses = %d\n", translationCount);
    double pfRate = (double)pageTable->pageFaultCount / (double)translationCount;
    double TLBRate = (double)tlbTable->tlbHitCount / (double)translationCount;

    printf("Page Faults = %d\n", pageTable->pageFaultCount);
    printf("Page Fault Rate = %.3f %%\n",pfRate * 100);
    printf("TLB Hits = %d\n", tlbTable->tlbHitCount);
    printf("TLB Hit Rate = %.3f %%\n", TLBRate * 100);
    printf("Average time spent retrieving data from backing store: %.3f millisec\n", getAvgTimeInBackingStore());
    printf("\n-----------------------------------------------------------------------------------\n");

    // close the input file and backing store
    fclose(address_file);
    fclose(backing_store);

    // NOTE: REMEMBER TO FREE DYNAMICALLY ALLOCATED MEMORY
    freeVMtable(&tlbTable);
    freeVMtable(&pageTable);
    freeDRAM(&dram, TOTAL_FRAME_COUNT);
    return 0;
}

// translateAddress takes the page number and offset and translates it into a physical address
void translateAddress()
{
    // First try to get page from TLB
    int frame_number = -1; // Assigning -1 to frame_number means invalid initially

   // Looking through TLB for the memory page
    for (int i = 0; i < tlbTable->length; i++) {
        if (tlbTable->pageNumArr[i] == page_number) {
            frame_number = tlbTable->frameNumArr[i];
            tlbTable->tlbHitCount++;
            break;
        }
    }

   // Check if page number is in page table or if there was a TLB miss
    if (frame_number == -1) {
        tlbTable->tlbMissCount++; // Increment the miss count
        // walk the contents of the page table
        for(int i = 0; i < nextPage; i++){
            if(pageTable->pageNumArr[i] == page_number){  // If the page is found in those contents
                frame_number = pageTable->frameNumArr[i]; // Extract the frame_number
                break; // Found in pageTable
            }
        }
        // NOTE: Page Table Fault Encountered
        if(frame_number == -1) {  // if the page is not found in those contents
            pageTable->pageFaultCount++;   // Increment the number of page faults
            // page fault, call to readFromStore to get the frame into physical memory and the page table
            start = clock();
            readFromStore(page_number);
            cpu_time_used += (double)(clock() - start) / CLOCKS_PER_SEC;
            functionCallCount++;
            frame_number = nextFrame - 1;  // And set the frame_number to the current nextFrame index
        }
    }

    tlbFIFOinsert(page_number, frame_number);

    // Frame number and offset used to get the signed translatedValue stored at that address
    translatedValue = dram[frame_number][offset_number];

    printf("\nVirtual address: %d\t\tPhysical address: %d\t\tValue: %d", virtual_addr, (frame_number << SHIFT) | offset_number, translatedValue);
}

// Function to read from the backing store and bring the frame into physical memory and the page table
void readFromStore(int pageNumber)
{
    if (fseek(backing_store, pageNumber * PAGE_READ_SIZE, SEEK_SET) != 0) {
        fprintf(stderr, "Error seeking in backing store\n");
    }

    // now read PAGE_READ_SIZE bytes from the backing store to the fileReadBuffer
    if (fread(fileReadBuffer, sizeof(signed char), PAGE_READ_SIZE, backing_store) == 0) {
        fprintf(stderr, "Error reading from backing store\n");
    }

    // Load the bits into the first available frame in the physical memory 2D array
    for (int i = 0; i < PAGE_READ_SIZE; i++) {
        dram[nextFrame][i] = fileReadBuffer[i];
    }

    // Then we want to load the frame number into the page table in the next frame
    pageTable->pageNumArr[nextPage] = pageNumber;
    pageTable->frameNumArr[nextPage] = nextFrame;

    // increment the counters that track the next available frames
    nextFrame++;
    nextPage++;
}

// Function to insert a page number and frame number into the TLB with a FIFO replacement
void tlbFIFOinsert(int pageNumber, int frameNumber)
{
    int i;

    // If it's already in the TLB, break
    for(i = 0; i < nextTLBentry; i++){
        if(tlbTable->pageNumArr[i] == pageNumber){
            break;
        }
    }

    // if the number of entries is equal to the index
    if(i == nextTLBentry){
        if(nextTLBentry < TLB_SIZE){  // IF TLB Buffer has open positions
            tlbTable->pageNumArr[nextTLBentry] = pageNumber;    // insert the page and frame on the end
            tlbTable->frameNumArr[nextTLBentry] = frameNumber;
        }
        else{ // No room in TLB Buffer

            // Replace the last TLB entry with our new entry
            tlbTable->pageNumArr[nextTLBentry - 1] = pageNumber;
            tlbTable->frameNumArr[nextTLBentry - 1] = frameNumber;

            // Then shift up all the TLB entries by 1 so we can maintain FIFO order
            for(i = 0; i < TLB_SIZE - 1; i++){
                tlbTable->pageNumArr[i] = tlbTable->pageNumArr[i + 1];
                tlbTable->frameNumArr[i] = tlbTable->frameNumArr[i + 1];
            }
        }
    }

    // If the index is not equal to the number of entries
    else{

        for(i = i; i < nextTLBentry - 1; i++){  // iterate through up to one less than the number of entries
            // Move everything over in the arrays
            tlbTable->pageNumArr[i] = tlbTable->pageNumArr[i + 1];
            tlbTable->frameNumArr[i] = tlbTable->frameNumArr[i + 1];
        }
        if(nextTLBentry < TLB_SIZE){  // if there is still room in the array, put the page and frame on the end
             // Insert the page and frame on the end
            tlbTable->pageNumArr[nextTLBentry] = pageNumber;
            tlbTable->frameNumArr[nextTLBentry] = frameNumber;

        }
        else{  // Otherwise put the page and frame on the number of entries - 1
            tlbTable->pageNumArr[nextTLBentry - 1] = pageNumber;
            tlbTable->frameNumArr[nextTLBentry - 1] = frameNumber;
        }
    }
    if(nextTLBentry < TLB_SIZE) { // If there is still room in the arrays, increment the number of entries
        nextTLBentry++;
    }
}

// Finds the oldest entry in TLB and returns the replacement Index
int getOldestEntry(int tlbSize) {
  int i, max, index;

  max = tlbTable->entryAgeArr[0];
  index = 0;

  for (i = 1; i < tlbSize; i++) {
    if (tlbTable->entryAgeArr[i] > max) {
       index = i;
       max = tlbTable->entryAgeArr[i];
    }
  }
  return index;
}

// Just computes the average time complexity of accessing the backing store
double getAvgTimeInBackingStore() {
    double temp = (double) cpu_time_used / (double) functionCallCount;
    return temp * 1000000;
}