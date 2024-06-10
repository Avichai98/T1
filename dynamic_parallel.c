#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <float.h>

#define SIZE   				40
#define RADIUS 				10
#define TASK   				1			
#define NO_MORE_TASKS 2
#define RESULT 				3


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


void master(int numprocs)
{
	MPI_Status status;
	double time1 = MPI_Wtime();
	int x = 0, y = 0, tasks_sent = 0, tasks_completed = 0, dummy = 0, i, p;
	int flag = 0; // Check if sent "no more tasks"
  double answer = DBL_MAX, value, received_value;
  
 	for(i = 1; i < numprocs; i++)
 	{
  	MPI_Send(&x, 1, MPI_INT, i, TASK, MPI_COMM_WORLD);
 		MPI_Send(&y, 1, MPI_INT, i, TASK, MPI_COMM_WORLD);
 		tasks_sent++;
 		if(++y >= SIZE)
  	{
  		y = 0;
  		if(++x >= SIZE)
  			break;
  	}		
 	}
 	
  while(tasks_completed < SIZE * SIZE)
  {
    MPI_Recv(&received_value, 1, MPI_DOUBLE, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &status);
	
    answer = (received_value < answer) ? received_value : answer;
    tasks_completed++;
 
  	if(tasks_sent < SIZE * SIZE)
  	{
  		MPI_Send(&x, 1, MPI_INT, status.MPI_SOURCE, TASK, MPI_COMM_WORLD);
      MPI_Send(&y, 1, MPI_INT, status.MPI_SOURCE, TASK, MPI_COMM_WORLD);
    	//printf("I sent to proc: %d,   index x: %d,    index y: %d\n", status.MPI_SOURCE, x, y);
      
      tasks_sent++;
      if(++y >= SIZE)
      {
      	y = 0;
     		x++;
      }	
  	}
  	
  	if(tasks_sent >= SIZE * SIZE && flag == 0) // No more tasks
  	{
  		for (p = 1; p < numprocs; p++)
  		{
        MPI_Send(&dummy, 1, MPI_INT, p, NO_MORE_TASKS, MPI_COMM_WORLD);
        MPI_Send(&dummy, 1, MPI_INT, p, NO_MORE_TASKS, MPI_COMM_WORLD);
        flag = 1;
      }
  	}
  }
 
	double time2 = MPI_Wtime();
  double time = (time2 - time1);
  printf("the time is: %f,  ", time);
	printf("answer = %e\n", answer);
}

void worker()
{
	int x, y;
  double answer = DBL_MAX, value;
  MPI_Status status;
	
  while (1)
  {
  	MPI_Recv(&x, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&y, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
    if (status.MPI_TAG == NO_MORE_TASKS) 
    	break;
    
    value = heavy(x, y);
		//answer = (value < answer) ? value : answer;
		
    MPI_Send(&value, 1, MPI_DOUBLE, 0, RESULT, MPI_COMM_WORLD);
  }
}

int main(int argc,char **argv)
{
    int myid, numprocs;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
		
		if (numprocs < 2) 
		{
		printf("launch at least 2 processes, the first process sending the tasks\n");
		MPI_Abort(MPI_COMM_WORLD, 0);
		}
		
    if(myid == 0)
    	master(numprocs);
    	
    else
    	worker();   
        
    MPI_Finalize();
    return 0;
}
