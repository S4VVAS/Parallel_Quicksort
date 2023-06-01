#include "./graphics/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
//#include <float.h>
#include <sys/time.h>

#define MAX_CHARS 2048

const int windowWidth=800;

int nThreads, isGraphic = 1, col, order, size, *element_pos, *pivots, *starts, *ends;
char *colName, *fileName;
float gHeightY, smallest, largest;
double *elements;

double reduceRange(double number, double oldMin, double oldMax, double newMin, double newMax) {
    return ((number - oldMin) / (oldMax - oldMin)) * (newMax - newMin) + newMin;
}

void updateGraphics(){
    if(isGraphic){
        ClearScreen();
        for(int i = 0; i < size; i++){
            DrawLine(reduceRange(element_pos[i],0,size,0.1, 0.9), gHeightY, 1.0, 1.0, reduceRange(elements[i],smallest,largest,0,650) , 0.5);
        }
        Refresh();
        usleep(300);
    }
}

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
                    
                    element_pos = realloc(element_pos, sizeof(int) * (size + 1));
                    element_pos[size] = size;
                    
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

/*void merge(int arr[], int left[], int leftSize, int right[], int rightSize) {
    int i = 0, j = 0, k = 0;
    while (i < leftSize && j < rightSize) {
        if (left[i] <= right[j])
            arr[k++] = left[i++];
        else
            arr[k++] = right[j++];
    }
    while (i < leftSize)
        arr[k++] = left[i++];
    while (j < rightSize)
        arr[k++] = right[j++];
}

void globalSort(int arr[]) {
    if(endIndex-startIndex == 1)
        return;

    int mid = n / 2;
    int *left = (int*)malloc(mid * sizeof(int));
    int *right = (int*)malloc((n - mid) * sizeof(int));

    // Split the array into two halves
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            for (int i = 0; i < mid; i++)
                left[i] = arr[i];
        }

        #pragma omp section
        {
            for (int i = mid; i < n; i++)
                right[i - mid] = arr[i];
        }
    }

    // Recursively sort the two halves
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            mergeSort(left, mid);
        }

        #pragma omp section
        {
            mergeSort(right, n - mid);
        }
    }

    // Merge the sorted halves
    merge(arr, left, mid, right, n - mid);

    free(left);
    free(right);
}*/

int selectPivot(int startIndex, int endIndex){
    return startIndex + (endIndex-startIndex)/2;
}

void swap(int* a, int* b){
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int low, int high) {
    int pivot = elements[selectPivot(low, high)];
    
    int i = low - 1;
    
    for (int j = low; j <= high - 1; j++) {
        if (elements[element_pos[j]] <= pivot) {
            i++;
            swap(&element_pos[i], &element_pos[j]);
        }
    }
    swap(&element_pos[i + 1], &element_pos[high]);
    return (i + 1);
}


void merge(int startIndex, int endIndex, int neighborStartIndex) {
    int mergedSize = endIndex - startIndex + 1;
    int merged[mergedSize];
    int leftIndex = startIndex;
    int rightIndex = neighborStartIndex;
    int mergedIndex = 0;

    while (leftIndex <= endIndex && rightIndex <= endIndex) {
        if (elements[element_pos[leftIndex]] <= elements[element_pos[rightIndex]]) {
            merged[mergedIndex] = element_pos[leftIndex];
            leftIndex++;
        } else {
            merged[mergedIndex] = element_pos[rightIndex];
            rightIndex++;
        }
        mergedIndex++;
    }

    while (leftIndex <= endIndex) {
        merged[mergedIndex] = element_pos[leftIndex];
        leftIndex++;
        mergedIndex++;
    }

    while (rightIndex <= endIndex) {
        merged[mergedIndex] = element_pos[rightIndex];
        rightIndex++;
        mergedIndex++;
    }

    for (int i = 0; i < mergedSize; i++) {
        element_pos[startIndex + i] = merged[i];
    }
}

void globalSort(int startIndex, int endIndex, int threadId) {
    int locid = threadId % size;
    double group = threadId / size;

    if (locid == 0) {
        pivots[(int)group] = selectPivot(startIndex, endIndex);
    }
    
    // Synchronize group
    #pragma omp barrier

    int splitpoint = partition(startIndex, endIndex, pivots[group]);

    // Wait for neighbor split
    if (locid < size / 2) {
        merge(startIndex, endIndex, startIndex + size / 2);
    } else {
        merge(startIndex, endIndex, startIndex - size / 2);
    }

    // Wait for neighbor merge
    #pragma omp barrier

    // Recursive global sort
    if (size / 2 >= 1) {
        globalSort(startIndex, endIndex, threadId);
    }
}


void swap(int* a, int* b){
    int t = *a;
    *a = *b;
    *b = t;
}




void mergeSort(int startIndex, int endIndex){
    if (startIndex < endIndex) {
        int prt = partition(startIndex, endIndex);
        
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                mergeSort(startIndex, prt-1);
            }
            #pragma omp section
            {
                mergeSort(prt+1, endIndex);
            }
        }
        
    }
    
    
   // updateGraphics();
    
}




void localSort(int startIndex, int endIndex){
    double temp = 0;
    
    for (int i = startIndex; i < endIndex; i++) {
            for (int j = i+1; j < endIndex; j++) {
               if(elements[i] > elements[j]) {
                   temp = elements[i];
                   elements[i] = elements[j];
                   elements[j] = temp;

               }
            }
        }

   
}

void quickSort(){

/*Algorithm:
1. Divide the data into p equal parts
2. Sort the data locally in each processor
3. Perform global sort
3.1 Select pivot in each processor set
3.2 In each processor, divide the data
into two sets (smaller or larger)
3.3 Split the processors into two groups
and exchange data pair-wise
3.4 Merge data into a sorted list in each
processor
4. Repeat 3.1-3.4 recursively for each
processor group
   
 */
    int rem = size%nThreads;
    
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
        ends[0] = step + rem;
        for(int i = 1; i < nThreads; i++){
            starts[i] = (i * step) + rem;
            ends[i] = ((i+1) * step) + rem;
        }
        
    }
    #pragma omp parallel num_threads(nThreads)
    {
     
      
            int tid = omp_get_thread_num();
            //Sort locally in processor
            mergeSort(starts[tid], ends[tid]);
            globalSort(starts[tid], ends[tid], tid);
            
        
    }

}

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
    
    //omp_set_max_active_levels(nThreads+1);


    //Max amount of pivots size/2
    pivots = malloc(sizeof(int) * (size / 2));
    starts = malloc(sizeof(int) * nThreads);
    ends = malloc(sizeof(int) * nThreads);
        
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
    free(element_pos);
    
    free(pivots);
    free(starts);
    free(ends);
}

int main(int argc, char** argv){
    
    //Check for correct num of arguments
    //Setup using arguments

    setup(argv);

    
    quickSort();
    

    while(!CheckForQuit()){
        updateGraphics();
    }
    
    
    //for(int i = 0; i < size; i++){
    //    printf("%d = %f\n", element_pos[i], elements[i]);
    //}
   // printf("size: %d\n",size);
    
   
    
    clean();
}
