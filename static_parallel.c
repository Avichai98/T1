#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <float.h>

#define SIZE   40
#define RADIUS 10
#define MAX    10


// This function performs heavy computations, 
// its run time depends on x and y values
double heavy(int x, int y)
{
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

int main(int argc,char **argv)
{
    int myid, numprocs;
    MPI_Status status;
		double data[SIZE];
		double value_and_index [2];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    
		int x, y;
    double answer = DBL_MAX, value;
    double start_time = MPI_Wtime();

    for (x = myid; x < SIZE; x += numprocs)
    {
    	for (y = 0; y < SIZE; y++) 
    	{
      	value = heavy(x, y);
        answer = answer < value ? answer : value;
        //printf("value: %f,   process: %d,   index x: %d,   index y: %d\n", answer, myid, x, y);
      }
      value_and_index[0] = answer;
      value_and_index[1] = (double)x;
      if(myid != 0)     	
     		MPI_Send(value_and_index, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
     	
     	else
     		data[x] = answer;
		}
		
		// The first process collects all the data from the other processes
		if (myid == 0)
    {
    	int index;
    	for (int i = 1; i < numprocs; i++)
      {
      	for (x = i; x < SIZE; x += numprocs)
        {
        	MPI_Recv(value_and_index, 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
          index = (int)value_and_index[1];
          data[index] = value_and_index[0];
        }
      }

			answer = DBL_MAX;
			for(int i = 0; i < SIZE; i++)
			{
				answer = answer < data[i] ? answer : data[i];
			}
      double end_time = MPI_Wtime();
      double time = (end_time - start_time);
      printf("The time is: %f,  ", time);
			printf("answer = %e\n", answer);
		}	
    MPI_Finalize();
    return 0;
}

