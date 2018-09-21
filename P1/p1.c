#include "omp.h"
#include <stdio.h>
int main()
{
	// Parallel section
	#pragma omp parallel
	{
		printf("Hello World\n");
	}
	return 0;
}
