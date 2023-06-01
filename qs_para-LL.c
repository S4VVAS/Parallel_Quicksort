#include "./graphics/graphics.h"
#include "./linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <sys/time.h>

#define MAX_CHARS 2048

int nThreads, isGraphic = 1, col, order;
char *colName, *fileName;
float gHeightY;

int *starts, *ends;

double *pos;




const int windowWidth=800;

double reduceRange(double number, double oldMin, double oldMax, double newMin, double newMax) {
    return ((number - oldMin) / (oldMax - oldMin)) * (newMax - newMin) + newMin;
}

void updateGraphics(){

    if(isGraphic){
        ClearScreen();
        int currSize = getSize(0);
        for(int i = 0; i < nThreads; i++){
            int currSizeOfList = getSize(i);
            for(int j = 0; j < currSizeOfList; j++){
                DrawLine(reduceRange(i,0,currSize,0.1, 0.9), gHeightY, 1.0, 1.0, reduceRange(getLL(i, j),smallest,largest,0,650) , 0.5);
                printf("\n\nMADE IT HERE!!!\n\n");
            }
           

            
        }
        Refresh();
        usleep(3000);
    }
}

void writeData(){}

void readData(FILE* file, int division) {
    size_t maxLineLength = 0;
    char* line = NULL;
    ssize_t bytesRead;
    
    // Skip the header row
    getline(&line, &maxLineLength, file);


    for(int i = 0; bytesRead != -1; i+=division){
        for(int j = 0; j < division; j++){

            bytesRead = getline(&line, &maxLineLength, file);
            char* token = strtok(line, ";");
            int currentColumn = 0;
            
            while (token != NULL) {
                if (currentColumn == col-1) {
                    addLL(j, atof(token));
                    break;
                }
                currentColumn++;
                token = strtok(NULL, ";");
            }
        }
            
        
        
    }
    //Increment size one last time to get true size, not starting from 0
    free(line);
}

void divideElementsToLLs(){
    
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

        setupLL(nThreads);

        readData(fopen(argv[1], "r"), nThreads);

        starts = malloc(nThreads*sizeof(int));
        ends = malloc(nThreads*sizeof(int));

    
        
        printf("Running using %d threads.\n",nThreads);
        printf("Num of Elements = %d.\n", getSize(0));
        printf("isGraphic = %d.\n", isGraphic);
        printf("l = %f s = %f\n", largest, smallest);
}

int selectPivot(int startIndex, int endIndex){
    return 0;
}


/*void globalSort(int startIndex, int endIndex, int threadId){
    if(endIndex-startIndex == 1)
        return;
    
    locid=myid%size
    group=myid/size
    if locid==0
        pivot[group]=select(data[myid])
    //Synchronize group
    splitpoint=findsplit(data[myid],pivot[group])
    //wait neighbor split
    if locid<size/2
        Merge(data[myid],data[myid+size/2],lowerparts)
    else
        Merge(data[myid],data[myid-size/2],upperparts)
    //wait neighbor merge
    Global_sort(data,size/2,myid)
}

void quickSort(int startIndex, int endIndex){
    
Algorithm:
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
    
    int rem = size%nThreads;
    if(rem == 0){
        int step = size/nThreads;
        for(int i = 0; i < nThreads; i++){
            starts[i] = i * step;
            ends[i] = i+1 * step - 1;
        }
    }
    else{
        int step = (size-rem)/nThreads;
        
        starts[0] = 0;
        ends[0] = step + rem - 1;
        
        for(int i = 1; i < nThreads; i++){
            starts[i] = (i * step) + rem;
            ends[i] = (i+1 * step) + rem - 1;
        }
        
    }
    
    #pragma omp parallel for
    for(int i = 0; i < size; i++){
        
        
    }
    
    

}'
*/

void clean(){
    if(isGraphic){
        FlushDisplay();
        CloseDisplay();
    }
    destroyLL();
    free(colName);
    free(fileName);
    free(starts);
    free(ends);
}

int main(int argc, char** argv){
    //Check for correct num of arguments
    //Setup using arguments

    setup(argv);

    printf("Hit q to quit.\n");

    while(!CheckForQuit()){
        updateGraphics();

    }
    clean();
}
