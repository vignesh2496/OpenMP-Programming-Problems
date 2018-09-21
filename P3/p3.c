#include <omp.h>
#include <stdio.h>

int X[1 << 16], Y[1 << 16];

void init_vectors()
{
	int j;
	#pragma omp parallel for
	for(j = 0; j < 1 << 16; j++)
	{
		X[j] = j;
		Y[j] = 2 * j;
	}	
}

int main()
{
	int i, j, threads, a = 3;	
	double start, end, time, t1;

	for(i = 1 ; i <= 16 ; i++)
	{
		//  Ask for i threads
		omp_set_num_threads(i);

		// Initialize vectors		
		init_vectors();	

		// Start timer
		start = omp_get_wtime();

		// Parallel section
		#pragma omp parallel
		{
			// Master finds number of threads allocated
			#pragma omp master
			threads = omp_get_num_threads();
			
			// Compute DAXPY using work-sharing
			#pragma omp for
			for(j = 0; j < 1 << 16; j++)
				X[j] = a * X[j] + Y[j];
		}

		// Stop timer
		end = omp_get_wtime();
		
		// Print results
		time = end - start;
		if(threads == 1)
			t1 = time;
		printf("Speedup for %d threads = %lf\n", threads, t1 / time );
	}
	return 0;
}