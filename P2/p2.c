#include <stdio.h>
#include <omp.h>

int func(int ID)
{
	printf("Hello World %d\n", ID);
}

int main()
{
	// Parallel section
	#pragma omp parallel
	{
		// Get thread ID
		int ID = omp_get_thread_num();
		func(ID);
	}
	return 0;
}  