#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 100000

void fill_array(int n, double a[])
{
	int i;
	for(i = 0; i < n; i++)
		a[i] = 1;
}

double sum_array(int n, double a[])
{
	int i;
	double sum = 0;
	for(i = 0; i < n; i++)
		sum += a[i];
	return sum;
}

int main()
{
	int flag = 0;
	double *A, sum, start, end, time;	
	A = (double*) malloc(N * sizeof(double));

	// Start timer
	start = omp_get_wtime();

	// Parallel section
	#pragma omp parallel sections
	{
		// Producer section populates array
		#pragma omp section
		{
			fill_array(N,A); 
			printf("Producer : Finished populating array\n");
			// Flush after populating
			#pragma omp flush 

			flag = 1 ;
			// Flush to let the consumer know that the producer has finished
			#pragma omp flush(flag)
		}
		
		// Consumer section sums the array
		#pragma omp section
		{
			// Keep checking flag to know if producer has finished
			while(flag == 0)
			{
				// Flush to retrieve updated value of flag
				#pragma omp flush(flag)
			}

			// Flush to retireve populated array
			#pragma omp flush 
			sum = sum_array(N, A); 
			printf("Consumer : Finished summing array\n");
		}		
	}

	// Stop timer
	end = omp_get_wtime();

	// Print results
	time = end - start;
	printf("In %lf seconds, the computed array-sum is %lf \n", time, sum);

	return 0 ;
}