#include <stdio.h>
#include <omp.h>
#define ITERATIONS 100000

int main()
{
	int i, j, threads;
	double start, end, time, t1, step = 1.0 / ITERATIONS, sum = 0, PI = 0;

	for(i = 1; i <= 16; i++)
	{
		// Ask for i threads
		omp_set_num_threads(i);

		// Start timer 
		start = omp_get_wtime();

		// Parallel section
		#pragma omp parallel 
		{
			// Master finds number of threads allocated
			#pragma omp master
			threads = omp_get_num_threads();

			// Use work-sharing to compute partial sums
			#pragma omp for reduction(+ : sum)
			for(j = 0; j < ITERATIONS; j++)
			{
				double x = (j + 0.5) * step;
				sum += 4 / (1 + x * x);
			}
		}

		// Stop timer
		end = omp_get_wtime();
	
		// Print results	
		PI = sum * step;
		time = end - start;
		if(threads == 1)
			t1 = time;
		printf("%d threads : PI = %lf \t Speedup = %lf\n", threads, PI, t1 / time);

		// Reset sum
		sum = 0;
	}

	return 0;
}