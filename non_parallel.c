#include <stdio.h>
#include <math.h>
#include <float.h>
#include "time.h"
#define SIZE       40
#define RADIUS 10

// This function performs heavy computations, 
// its run time depends on x and y values
double heavy(int x, int y) {
    int i, loop;
    double sum = x;
    

    if (sqrt((x - 0.75 * SIZE) * (x - 0.75 * SIZE) + (y - 0.25 * SIZE) * (y - 0.25 * SIZE)) < RADIUS)
        loop =  10 + 5 * x * y;
    else
        loop = 5 + y + x;

    for (i = 1; i < loop * 100; i++)
        sum += sin(exp(cos((double)i / 100)));

    return  sum;
}


int main(int argc, char* argv[]) {
    int x, y; 
    double answer = DBL_MAX, value;
		clock_t start, end;

    start = clock();
    
    for (x = 0; x < SIZE; x++)
        for (y = 0; y < SIZE; y++) {
            value = heavy(x, y);
            answer = answer < value ? answer : value;
        }
        
		end = clock(); // Record the end time
    double time = ((double)(end - start)) / CLOCKS_PER_SEC; // Calculate the time taken
		printf("the time is: %f,  ", time);
    printf("answer = %e\n", answer);
}

