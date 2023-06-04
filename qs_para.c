#include "./graphics/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>

#define MAX_CHARS 2048

const int windowWidth=800;

int nThreads, isGraphic = 1, col, order, size, *pivots, *starts, *ends;
char *colName, *fileName;
float gHeightY, smallest, largest;
double *elements, ***uppers, ***lowers, **allUppers, **allLowers;




//GRAPHICS
double reduceRange(double number, double oldMin, double oldMax, double newMin, double newMax) {
    return ((number - oldMin) / (oldMax - oldMin)) * (newMax - newMin) + newMin;
}

void updateGraphics(){
    if(isGraphic){
        ClearScreen();
        for(int i = 0; i < size; i++){
            DrawLine(reduceRange(i,0,size,0.1, 0.9), gHeightY, 1.0, 1.0, reduceRange(elements[i],smallest,largest,0,650) , 0.5);
        }
        Refresh();
        usleep(200000);
    }
}




//IO FUNCTIONS
void writeData(){}

void readData(FILE* file, int division) {
    size_t maxLineLength = 0;
    char* line = NULL;
    ssize_t bytesRead;
    size = 0;
    
    //Skip the header row
    getline(&line, &maxLineLength, file);

    // Read each line of the file
    while ((bytesRead = getline(&line, &maxLineLength, file)) != -1) {
        char* token = strtok(line, ";");
        int currentColumn = 0;
        while (token != NULL) {
            if (currentColumn == col-1) {
                if (token[0] != '\0') {
                    // Convert the value to double and store it
                    elements = realloc(elements, sizeof(double) * (size + 1));
                    elements[size] = atof(token);
                                        
                    //THE LARGEST AND SMALLEST VARIABLES ARE ONLY USED FOR GRAPHICS!!
                    if(elements[size] > largest)
                        largest = elements[size];
                    if(elements[size] < smallest)
                        smallest = elements[size];
                    
                    size++;
                }
                break;
            }
            token = strtok(NULL, ";");
            currentColumn++;
        }
    }
    // Increment size one last time to get the true size, not starting from 0
    size++;
    free(line);
}





//SETUP and CLEANING
void setup(char** argv){
    
    if(isGraphic){
        InitializeGraphics(argv[0],windowWidth,windowWidth);
        SetCAxes(0,1);
    }
    
    gHeightY = 0.9;

    col = atoi(argv[2]);
    order = atoi(argv[3]);
    nThreads = atoi(argv[4]);
    isGraphic = atoi(argv[5]);
    
    readData(fopen(argv[1], "r"), nThreads);
    
    //Max amount of pivots size/2
    pivots = malloc(sizeof(int) * size);
    starts = malloc(sizeof(int) * nThreads);
    ends = malloc(sizeof(int) * nThreads);
    
    //upper = malloc(sizeof(int*) * nThreads);
   // lower = malloc(sizeof(int*) * nThreads);
    
   /* for(int i = 0; i < nThreads; i++){
        upper[i] = malloc(sizeof(int) * 2);
        lower[i] = malloc(sizeof(int) * 2);
    }*/
    
    lowers = malloc(sizeof(double**) * nThreads/2);
    uppers = malloc(sizeof(double**) * nThreads/2);
    for(int i = 0; i < nThreads/2; i++){
        lowers[i] = malloc(sizeof(double*) * nThreads);
        uppers[i] = malloc(sizeof(double*) * nThreads);
        for(int j = 0; j < nThreads; j++){
            lowers[i][j] = malloc(sizeof(double) * (size / (nThreads / 2)));
            uppers[i][j] = malloc(sizeof(double) * (size / (nThreads / 2)));
        }
    }
    
    allUppers = malloc(sizeof(double**) * nThreads/2);
    allLowers = malloc(sizeof(double**) * nThreads/2);
    for(int i = 0; i < nThreads/2; i++){
        allLowers[i] = malloc(sizeof(double) * size);
        allUppers[i] = malloc(sizeof(double) * size);
    }
        
    printf("Running using %d threads.\n",nThreads);
    printf("Num of Elements = %d.\n", size);
    printf("isGraphic = %d.\n", isGraphic);
    printf("l = %f s = %f\n", largest, smallest);
}


void clean(){
    if(isGraphic){
        FlushDisplay();
        CloseDisplay();
    }
    free(colName);
    free(fileName);
    
    free(elements);
    
    for(int i = 0; i < nThreads/2; i++){
        for(int j = 0; j < nThreads; j++){
            free(lowers[i][j]);
            free(uppers[i][j]);
        }
        free(lowers[i]);
        free(uppers[i]);
    }
    free(uppers);
    free(lowers);
    
    for(int i = 0; i < nThreads/2; i++){
        free(allLowers[i]);
        free(allUppers[i]);
    }
    free(allLowers);
    free(allUppers);
    
    free(pivots);
    free(starts);
    free(ends);
}

//HELPER FUNCTIONS
int selectPivot(int startIndex, int endIndex){
    return startIndex + (endIndex-startIndex)/2;
}

void swap_pos(double* a, double* b) {
    double t = *a;
    *a = *b;
    *b = t;
}

//QUICKSORT ALGORITHMS
int partition(int startIndex, int endIndex) {
    int pi = selectPivot(startIndex, endIndex);
    int piv = elements[pi];
    
    swap_pos(&elements[pi], &elements[endIndex]);

    int i = startIndex - 1;
    for (int j = startIndex; j < endIndex; j++) {
        if (elements[j] < piv) {
            swap_pos(&elements[++i], &elements[j]);
        }
    }
    swap_pos(&elements[i + 1], &elements[endIndex]);
    return (i + 1);
}

void quickSort(int startIndex, int endIndex) {
    if (startIndex < endIndex) {
        int prt = partition(startIndex, endIndex);
        quickSort(startIndex, prt - 1);
        quickSort(prt + 1, endIndex);
    }
}
















//GLOBALSORT ALGORITHMS
/*int findSplit(int pivotIndex, int start, int end) {
    double pivotValue = elements[pivotIndex];
    for (int i = start; i < end; i++)
        if (elements[i] >= pivotValue)
            return i;
    return end;
}

void mergeSplits(int pivot){
    double* tmp = malloc(sizeof(double) * size);
    
    //Merge lowers
    for(int t = 0; i < nThreads);
    for(int i = 0; i < pivot; i++){
        
    }
    //Merge uppers
    for(int i = pivot; i < size; i++){
        
    }
}

void Merge(int *a, int *b, int *c, int size) {
    // Merge arrays a and b into array c
    int i = 0, j = 0, k = 0;
    while (i < size / 2 && j < size / 2) {
        if (a[i] <= b[j]) {
            c[k++] = a[i++];
        } else {
            c[k++] = b[j++];
        }
    }
    while (i < size / 2) {
        c[k++] = a[i++];
    }
    while (j < size / 2) {
        c[k++] = b[j++];
    }
}


void globalSort(int startIndex, int endIndex){
    int size = endIndex - startIndex + 1;
    if(size == 1)
        return;
    
    int pivot = selectPivot(starts[0],ends[0]);
    printf("PIVOT = %d\n\n", pivot);
    
    #pragma omp parallel num_threads(nThreads) shared(pivot)
    {
        int tid = omp_get_thread_num();
        int split = findSplit(pivot, starts[tid], ends[tid]);
        //printf("%d = %f -> %f -> %f\n",split, elements[split-1], elements[split], elements[split+1]);
        lower[tid][0] = starts[tid];
        lower[tid][1] = split-1;
        upper[tid][0] = split;
        upper[tid][1] = ends[tid];
        
        printf("t%d - lower (%d -> %d) / upper (%d -> %d)\n", tid, lower[tid][0], lower[tid][1], upper[tid][0],upper[tid][1]);
        
    }
    //Under split < pivot
    //Split and up => pivot
    mergeSplits();
    
    
    
    
    
    //All threads must exist with their parts
    //Select pivot
    //All processors must now swap their elements pair wise with all other untill all elements are
        //bellow or above pivot
    //This data should be divided into two groups, smaller and larger
    
    //split processors into 2 groups and they
    
    
}*/

int findSplit(int startIndex, int endIndex, double pivot) {
    for (int i = startIndex; i < endIndex; i++)
        if (elements[i] > pivot)
            return i;
    return size;
}

void mergeLower(int idA, int idB, int group, int threadCount) {
    int locidA = idA % threadCount;
    int locidB = idB % threadCount;

    // Calculate the size of the lower array
    int lowerSize = (size / (nThreads / 2)) - (ends[idA] - starts[idA]) - (ends[idB] - starts[idB]);

    // Create a temporary array to store the merged lower elements
    double* lowerTemp = malloc(sizeof(double) * lowerSize);

    int lowerIndex = 0;

    // Merge the lower arrays of idA and idB
    for (int i = 0; i < size / (nThreads / 2); i++) {
        if (i >= starts[idA] && i < ends[idA])
            continue; // Skip the elements already present in idA's lower array

        if (i >= starts[idB] && i < ends[idB])
            continue; // Skip the elements already present in idB's lower array

        lowerTemp[lowerIndex++] = lowers[group][locidA][i];
    }

    // Copy the merged lower elements back to idA's lower array
    lowerIndex = 0;
    for (int i = 0; i < size / (nThreads / 2); i++) {
        if (i >= starts[idA] && i < ends[idA])
            continue; // Skip the elements already present in idA's lower array

        if (i >= starts[idB] && i < ends[idB])
            continue; // Skip the elements already present in idB's lower array

        lowers[group][locidA][i] = lowerTemp[lowerIndex++];
    }

    free(lowerTemp);
}

void mergeUpper(int idA, int idB, int group, int threadCount) {
    int locidA = idA % threadCount;
    int locidB = idB % threadCount;

    // Calculate the size of the upper array
    int upperSize = (size / (nThreads / 2)) - (ends[idA] - starts[idA]) - (ends[idB] - starts[idB]);

    // Create a temporary array to store the merged upper elements
    double* upperTemp = malloc(sizeof(double) * upperSize);

    int upperIndex = 0;

    // Merge the upper arrays of idA and idB
    for (int i = 0; i < size / (nThreads / 2); i++) {
        if (i >= starts[idA] && i < ends[idA])
            continue; // Skip the elements already present in idA's upper array

        if (i >= starts[idB] && i < ends[idB])
            continue; // Skip the elements already present in idB's upper array

        upperTemp[upperIndex++] = uppers[group][locidA][i];
    }

    // Copy the merged upper elements back to idA's upper array
    upperIndex = 0;
    for (int i = 0; i < size / (nThreads / 2); i++) {
        if (i >= starts[idA] && i < ends[idA])
            continue; // Skip the elements already present in idA's upper array
        
        if (i >= starts[idB] && i < ends[idB])
            continue; // Skip the elements already present in idB's upper array
        
        uppers[group][locidA][i] = upperTemp[upperIndex++];
        
    }
    free(upperTemp);
}


void globalSort(double *data, int threadCount, int tid){
    if(threadCount == 1)
        return;
    int locid = tid % threadCount;
    int group = tid / threadCount;
    printf("locid %d = group %d = tsize = %d\n",locid,group, threadCount);
    // Calculate the length of the array
    //printf("len = %d\n",size);
    if(locid == 0)
        pivots[group] = selectPivot(starts[0], ends[0]);
    #pragma omp barrier
    //printf("piv for g%d = %d\n",group,pivots[group]);

    int splitpoint = findSplit(starts[tid], ends[tid],pivots[group]);
    
    //Reset Lowers and Uppers
    for(int i = 0; i < size / (nThreads / 2); i++){
        lowers[group][locid][i] = -DBL_MAX;
        uppers[group][locid][i] = -DBL_MAX;
    }
    
    //Populate Lowers
        for(int i = 0; i < splitpoint-starts[tid]; i++)
        lowers[group][locid][i] = elements[starts[tid]+i];
    //Populate Uppers
        for(int i = 0; i < ends[tid]-splitpoint; i++)
        uppers[group][locid][i] = elements[splitpoint+i];
    
    #pragma omp barrier

    
   // printf("split = %d\n\n",splitpoint);
    
    if(locid < size/2){
        mergeLower(tid, tid+(threadCount/2), group, threadCount);
        
    } //LowerParts
    else{
        mergeUpper(tid, tid-(threadCount/2), group, threadCount);
        
    }//UpperParts


    #pragma omp barrier
    globalSort(data,threadCount/2,tid);

    
}


void sort(){
   int rem = size%nThreads;
    updateGraphics();

    if(rem == 0){
        int step = size/nThreads;
        for(int i = 0; i < nThreads; i++){
            starts[i] = i * step;
            ends[i] = i+1 * step;
        }
    }
    
    
    else{
        int step = (size-rem)/nThreads;
        starts[0] = 0;
        ends[0] = step + rem - 1;
        for(int i = 1; i < nThreads; i++){
            starts[i] = (i * step) + rem;
            ends[i] = ((i+1) * step) + rem - 1;
        }
    }
    
    #pragma omp parallel num_threads(nThreads)
    {
        int tid = omp_get_thread_num();
        //printf("%d + %d\n", starts[tid], ends[tid]);
        //Sort locally in processor
        quickSort(starts[tid], ends[tid]);
        
    }
    updateGraphics();
    #pragma omp parallel num_threads(nThreads)
    {
        int tid = omp_get_thread_num();
        globalSort(elements, nThreads, tid);
    }
    updateGraphics();


}















int main(int argc, char** argv){
    
    //Check for correct num of arguments
    //Setup using arguments

    setup(argv);

    
    sort();
    

    while(!CheckForQuit()){
        updateGraphics();
    }
    
    
    for(int i = size-10; i < size; i++){
       
                printf("%d = %f\n", i ,elements[i]);
        
        
    }
   // printf("size: %d\n",size);
    

    
    clean();
}
