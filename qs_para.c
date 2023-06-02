#include "./graphics/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
//#include <float.h>
#include <sys/time.h>

#define MAX_CHARS 2048

const int windowWidth=800;

int nThreads, isGraphic = 1, col, order, size, *pivots, *starts, *ends;
char *colName, *fileName;
float gHeightY, smallest, largest;
double *elements;




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
        usleep(3000);
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


int partition(int low, int high) {
    int pi = selectPivot(low, high);
    int piv = elements[pi];
    
    swap_pos(&elements[pi], &elements[high]);

    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (elements[j] < piv) {
            swap_pos(&elements[++i], &elements[j]);
        }
    }
    swap_pos(&elements[i + 1], &elements[high]);
    return (i + 1);
}

void quickSort(int startIndex, int endIndex) {
    if (startIndex < endIndex) {
        int prt = partition(startIndex, endIndex);
        quickSort(startIndex, prt - 1);
        quickSort(prt + 1, endIndex);
    }
    
    //if(startIndex % 999 == 0)
   //     updateGraphics();

}


void sort(){
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
            printf("%d + %d\n", starts[tid], ends[tid]);
            //Sort locally in processor
            quickSort(starts[tid], ends[tid]);
            //globalSort(starts[tid], ends[tid], tid);

        
    }


}















int main(int argc, char** argv){
    
    //Check for correct num of arguments
    //Setup using arguments

    setup(argv);

    
    sort();
    

    while(!CheckForQuit()){
        updateGraphics();
    }
    
    
    for(int i = 980; i < 1000; i++){
       
                printf("%d = %f\n", i ,elements[i]);
        
        
    }
   // printf("size: %d\n",size);
    

    
    clean();
}
