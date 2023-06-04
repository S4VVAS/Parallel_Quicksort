#include "./graphics/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>

#define MAX_CHARS 2048

const int windowWidth=800;

int nThreads, isGraphic = 1, size, nBuckets, *bucketCounts;;
float gHeightY, smallest, largest;
double *elements, **buckets;



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
        usleep(700000);

    }
}




//SETUP and CLEANING
void setup(char** argv){
    gHeightY = 0.9;

    size = atoi(argv[1]);
    nBuckets = atoi(argv[2]);
    nThreads = atoi(argv[3]);
    isGraphic = atoi(argv[4]);
    
    if(isGraphic){
        InitializeGraphics(argv[0],windowWidth,windowWidth);
        SetCAxes(0,1);
    }
    
    elements = malloc(sizeof(double) * size);
    bucketCounts = calloc(nBuckets, sizeof(int));
    buckets = malloc(sizeof(double*) * nBuckets);
    for (int i = 0; i < nBuckets; i++) {
        buckets[i] = malloc(sizeof(double) * size);
    }

}


void clean(){
    if(isGraphic){
        FlushDisplay();
        CloseDisplay();
    }
    
    for (int i = 0; i < nBuckets; i++)
        free(buckets[i]);
    free(buckets);
    free(bucketCounts);
    
    free(elements);

}

//BUCKETSORT ALGORITHM
void bucketSort() {
    // Loop through array and put each array element in a bucket
    #pragma omp parallel for num_threads(nThreads)
    for (int i = 0; i < size; i++) {
        int bucketIndex = (int)(elements[i] * nBuckets);
        //If bucket index out of range, add to last bucket
        if (bucketIndex >= nBuckets)
            bucketIndex = nBuckets - 1;
        //If bucket index smaller than 0, add to first bucket
        if(bucketIndex < 0)
            bucketIndex = 0;
        
        //Increment element count on bucket
        int localIndex = 0;
        #pragma omp atomic capture
        localIndex = bucketCounts[bucketIndex]++;
        
        //Add to bucket
        buckets[bucketIndex][localIndex] = elements[i];
        
    }

    // Sort each of the non-empty buckets individually.
    #pragma omp parallel for num_threads(nThreads)
    for (int nB = 0; nB < nBuckets; nB++)
        for (int bC = 0; bC < bucketCounts[nB] - 1; bC++)
            for (int i = 0; i < bucketCounts[nB] - bC - 1; i++)
                if (buckets[nB][i] > buckets[nB][i + 1]) {
                    double temp = buckets[nB][i];
                    buckets[nB][i] = buckets[nB][i + 1];
                    buckets[nB][i + 1] = temp;
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



int main(int argc, char** argv){
    //NORMAL RUN
    setup(argv);
    populateNormal();
    updateGraphics();
    double nTime = omp_get_wtime();
    bucketSort();
    nTime = omp_get_wtime() - nTime;
    updateGraphics();
    clean();
    
    //EXPONENTIAL RUN
    setup(argv);
    populateExponential();
    updateGraphics();
    double eTime = omp_get_wtime();
    bucketSort();
    eTime = omp_get_wtime() - eTime;
    updateGraphics();
    clean();
    
    //UNIFORM RUN
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
