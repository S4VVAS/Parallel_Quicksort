#include "./graphics/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>

const int windowWidth = 800;

int nThreads, isGraphic = 1, size, nBuckets, *bucketCounts, *bucketSizes, bSize;
float gHeightY, smallest, largest;
double *elements;
double **buckets;


// GRAPHICS
double reduceRange(double number, double oldMin, double oldMax, double newMin, double newMax) {
    return ((number - oldMin) / (oldMax - oldMin)) * (newMax - newMin) + newMin;
}

void updateGraphics() {
    if (isGraphic) {
        ClearScreen();
        for (int i = 0; i < size; i++) {
            DrawLine(reduceRange(i, 0, size, 0.1, 0.9), gHeightY, 1.0, 1.0, reduceRange(elements[i], smallest, largest, 0, 650), 0.5);
        }
        Refresh();
        usleep(700000);
    }
}

// SETUP and CLEANING
void setup(char **argv) {
    gHeightY = 0.9;
    size = atoi(argv[1]);
  //  nBuckets = atoi(argv[2]);
   // nThreads = atoi(argv[3]);
    isGraphic = atoi(argv[4]);
    
    

    if (isGraphic) {
        InitializeGraphics(argv[0], windowWidth, windowWidth);
        SetCAxes(0, 1);
    }

    bSize = ((double)size / (double)nBuckets) * 1.1;
    
    elements = malloc(sizeof(double) * size);
    
    bucketCounts = calloc(nBuckets, sizeof(int));
    
    buckets = malloc(sizeof(double *) * nBuckets);
    for (int i = 0; i < nBuckets; i++)
           buckets[i] = calloc(bSize, sizeof(double));
    
    bucketSizes = malloc(sizeof(int) * nBuckets);
    for(int i = 0; i < nBuckets; i++)
        bucketSizes[i] = bSize;
}

void clean() {
    if (isGraphic) {
        FlushDisplay();
        CloseDisplay();
    }

    for (int i = 0; i < nBuckets; i++)
        free(buckets[i]);
    free(buckets);
    free(bucketCounts);
    free(bucketSizes);
    free(elements);
}

// BUCKETSORT ALGORITHM
int compare(const void *a, const void *b) {
    if (*(const double *)a < *(const double *)b)
        return -1;
    if (*(const double *)a > *(const double *)b)
        return 1;
    return 0;
}

void bucketSort() {
    //I could also use the smallest and largest variables used for graphics, but that would be cheating a bit when it comes to timing the algorithm
    double minimum = DBL_MAX;
    double maximum = -DBL_MAX;
    for(int i = 0; i < size; i++){
        if(elements[i] > maximum)
            maximum = elements[i];
        if(elements[i] < minimum)
            minimum = elements[i];
    }
    
    #pragma omp parallel for num_threads(nThreads) schedule(guided)
    for(int b = 0; b < nBuckets; b++){
        for(int i = 0; i < size; i++){
            //Converts index to int, rounds down
            int bucketIndex = (int)reduceRange(elements[i], minimum, maximum, 0.0, nBuckets);
        
            if(bucketIndex == b){
                int localIndex = bucketCounts[b]++;
                if(localIndex > bucketSizes[b]){
                    buckets[bucketIndex] = realloc(buckets[bucketIndex], sizeof(double) * (bucketCounts[b] * 1.5));
                    bucketSizes[b] = (int) (bucketCounts[b] * 1.5);
                }
                buckets[b][localIndex] = elements[i];
            }
        }
        qsort(buckets[b], bucketCounts[b], sizeof(double), compare);
    }
    
    // Merge the buckets back into the original array
    int index = 0;
    for (int nB = 0; nB < nBuckets; nB++) {
        for (int bC = 0; bC < bucketCounts[nB]; bC++)
            elements[index++] = buckets[nB][bC];
    }
}


//Find and set Smallest and Largest (used for graphics)
void setSnL(){
    smallest = DBL_MAX;
    largest = -DBL_MAX;
    for(int i = 0; i < size; i++){
        if(elements[i] > largest)
            largest = elements[i];
        if(elements[i] < smallest)
            smallest = elements[i];
    }
}

//ARRAY GEN FUNCTIONS
void populateNormal(){
    for(int i = 0; i < size; i++)
        elements[i] = sqrt(-2.0 * log(rand() / (double)RAND_MAX)) * cos((rand() / (double)RAND_MAX) * 2.0 * M_PI);
    setSnL();
}

void populateExponential(){
    for(int i = 0; i < size; i++)
        elements[i] = -log(1.0 - (rand() / (double)RAND_MAX));
    setSnL();
}

void populateUniform(){
    for(int i = 0; i < size; i++)
        elements[i] = rand() / (double)RAND_MAX;
    setSnL();
}

double run(int t, int b, int dist, char **argv){
    nBuckets = t;
    nThreads = b;
    setup(argv);
    switch(dist){
        case 0:
            populateNormal();
            break;
        case 1:
            populateExponential();
            break;
        case 2:
            populateUniform();
    }
    setSnL();
    updateGraphics();
    double uTime = omp_get_wtime();
    bucketSort();
    uTime = omp_get_wtime() - uTime;
    updateGraphics();
    clean();
    
    return uTime;
    
}


int main(int argc, char **argv) {

    double nTime1 = run(1,1,0,argv);
    double nTime2 = nTime1 / run(2,2,0,argv);
    double nTime4 = nTime1 / run(4,4,0,argv);
    double nTime8 = nTime1 / run(8,8,0,argv);
    double nTime16 = nTime1 / run(16,16,0,argv);
    double nTime32 = nTime1 / run(32,32,0,argv);
    
    double uTime1 = run(1,1,2,argv);
    double uTime2 = uTime1 / run(2,2,2,argv);
    double uTime4 = uTime1 / run(4,4,2,argv);
    double uTime8 = uTime1 / run(8,8,2,argv);
    double uTime16 = uTime1 / run(16,16,2,argv);
    double uTime32 = uTime1 / run(32,32,2,argv);
    
    double eTime1 = run(1,1,1,argv);
    double eTime2 = eTime1 / run(2,2,1,argv);
    double eTime4 = eTime1 / run(4,4,1,argv);
    double eTime8 = eTime1 / run(8,8,1,argv);
    double eTime16 = eTime1 / run(16,16,1,argv);
    double eTime32 = eTime1 / run(32,32,1,argv);
 
    printf("Speedup of distributions with %d elements. nBuckets = nThreads\n", size);

    printf("N = 1 1 2 %f 4 %f 8 %f 16 %f 32 %f\n", nTime2, nTime4, nTime8, nTime16, nTime32);
    printf("E = 1 1 2 %f 4 %f 8 %f 16 %f 32 %f\n", eTime2, eTime4, eTime8, eTime16, eTime32);
    printf("U = 1 1 2 %f 4 %f 8 %f 16 %f 32 %f\n", uTime2, uTime4, uTime8, uTime16, uTime32);
    
}
