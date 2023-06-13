#include "./graphics/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>

const int windowWidth = 800;

int nThreads, isGraphic = 1, size, nBuckets, *bucketCounts, bSize;
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
    nBuckets = atoi(argv[2]);
    nThreads = atoi(argv[3]);
    isGraphic = atoi(argv[4]);

    if (isGraphic) {
        InitializeGraphics(argv[0], windowWidth, windowWidth);
        SetCAxes(0, 1);
    }

    bSize = size / nBuckets * 1.1;
    
    elements = malloc(sizeof(double) * size);
    bucketCounts = calloc(nBuckets, sizeof(int));
    buckets = malloc(sizeof(double *) * nBuckets);
    for (int i = 0; i < nBuckets; i++) {
           buckets[i] = calloc(bSize, sizeof(double));
       }
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
    
    #pragma omp parallel for num_threads(nThreads) schedule(static)
    for(int b = 0; b < nBuckets; b++){
        for(int i = 0; i < size; i++){
            //Converts index to int, rounds down
            int bucketIndex = (int)reduceRange(elements[i], minimum, maximum, 0.0, nBuckets);
            
            if(bucketIndex == b){
                int localIndex = bucketCounts[b]++;
                if(localIndex > bSize)
                    buckets[bucketIndex] = realloc(buckets[bucketIndex], sizeof(double) * bucketCounts[b]);
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

int main(int argc, char **argv) {
    // NORMAL RUN
    setup(argv);
    populateNormal();
    updateGraphics();
    double nTime = omp_get_wtime();
    bucketSort();
    nTime = omp_get_wtime() - nTime;
    updateGraphics();
    clean();

    // EXPONENTIAL RUN
    setup(argv);
    populateExponential();
    updateGraphics();
    double eTime = omp_get_wtime();
    bucketSort();
    eTime = omp_get_wtime() - eTime;
    updateGraphics();
    clean();

    // UNIFORM RUN
    setup(argv);
    populateUniform();
    updateGraphics();
    double uTime = omp_get_wtime();
    bucketSort();
    uTime = omp_get_wtime() - uTime;
    updateGraphics();
    clean();

    printf("Wall time for %d elements distributed in %d buckets using %d threads:\n", size, nBuckets, nThreads);
    printf("Normal distribution:\t\t%fs\n", nTime);
    printf("Exponential distribution:\t%fs\n", eTime);
    printf("Uniform distribution:\t\t%fs\n", uTime);
}
