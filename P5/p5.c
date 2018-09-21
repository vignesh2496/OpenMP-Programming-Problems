#include <omp.h>
#include <stdio.h>
#define ITERATIONS 100000

int main()
{
	int i, threads;
	double start, end, time, t1, step = 1.0 / ITERATIONS, PI = 0;
	
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

			double sum = 0;
			int j, ID = omp_get_thread_num(), threads = omp_get_num_threads();
			// Use the SPMD paradigm to compute partial sums in a Round-Robin fashion 
			for (j = ID; j < ITERATIONS; j += threads)
			{
				double x = (j + 0.5) * step;
				sum += 4 / (1 + x * x);
			}

			// Only one thread can update PI at a given time to avoid race conditions
			#pragma omp atomic
			PI = PI + sum;
		}		

		// Stop timer
		end = omp_get_wtime();
	
		// Print results
		PI = PI * step;
		time = end - start;
		if(threads == 1)
			t1 = time;
		printf("%d threads : PI = %lf \t Speedup = %lf\n", threads, PI, t1 / time);

		// Reset PI
		PI = 0;
	}

	return 0;
}
