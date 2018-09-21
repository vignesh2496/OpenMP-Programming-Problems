#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
// A Mersenne prime
#define MOD 2147483647
#define ADDEND 12345
#define MULTIPLIER 1103515245
#define ITERATIONS 10000000

// leap_multiplier and leap_addend represent the multiplier and addend for the leap by a number of steps = number of threads
long long rand_last = 0, leap_multiplier, leap_addend;

// Setting rand_last to be private to each thread so that the threads don't use each others' rand_last values   
#pragma omp threadprivate(rand_last)

// Modular exponentiation
long long modexp(long long base, long long exp)
{
	long long ans = 1;
	while(exp)
	{
		if(exp % 2 == 1)
			ans = (ans * base) % MOD;
		base = (base * base) % MOD;
		exp = exp / 2; 
	}
	return ans;
}

// Returns a thread specific random number 
double get_rand()
{
	long long rand_next = ((rand_last * leap_multiplier) % MOD + leap_addend) % MOD;
	rand_last = rand_next;
	return (double)rand_next / MOD;
}

// Checking if the point lies inside the circle
int inside_circle(double x, double y)
{
	return ((x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5) < 0.25) ? 1 : 0;
}

int main()
{
	int inside_circle_points = 0, i, j, threads; 
	double start, end, time, t1, PI = 0;

	// Array for storing the thread seeds for each thread
	long long *threads_seeds;

	for(i = 1; i <= 16; i++)
	{
		// Ask for i threads
		omp_set_num_threads(i);

		// Start timer
		start = omp_get_wtime();

		// Parallel section
		#pragma omp parallel
		{
			// Ask one thread to compute the seeds for all the reamining threads
			#pragma omp single
			{
				// Find number of allocated threads
				threads = omp_get_num_threads();

				// Allocate space for the seeds
				threads_seeds = (long long*) malloc(sizeof(long long) * threads);
			
				// Initialize the first seed
				threads_seeds[0] = MOD / MULTIPLIER;
			
				// Compute remaining seeds
				for(j = 1; j < threads; j++)
					threads_seeds[j] = ((MULTIPLIER * threads_seeds[j - 1]) % MOD + ADDEND) % MOD;
			
				// Compute the modified multiplier and addend 
				leap_multiplier = modexp(MULTIPLIER, threads);
				leap_addend = ((leap_multiplier - 1 + MOD) % MOD * modexp(MULTIPLIER - 1, MOD - 2)) % MOD;
				leap_addend *= ADDEND; 
				leap_addend %= MOD;
			}

			int ID = omp_get_thread_num();
			// Assign the seed to respective threads
			rand_last = threads_seeds[ID];

			// Use work-sharing for plotting random points in a unit square 
			#pragma omp for reduction(+ : inside_circle_points)
			for(j = 0; j < ITERATIONS; j++)
			{
				double x = get_rand(), y = get_rand();
				if(inside_circle(x, y))
					inside_circle_points++;
			}
 		}

 		// Stop timer
		end = omp_get_wtime();
	
		// Print results
		PI = 4 * (double)inside_circle_points / ITERATIONS;
		time = end - start;
		if(threads == 1)
			t1 = time;
		printf("%d threads : PI = %lf \t Speedup = %lf\n", threads, PI, t1 / time);

		// Reset inside_circle_points
		inside_circle_points = 0;
		free(threads_seeds);
	}

	return 0;
}