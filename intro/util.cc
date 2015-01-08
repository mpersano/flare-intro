#include <cstdlib>

#include "util.h"

float
frand()
{
	return static_cast<float>(rand())/RAND_MAX;
}

float
frand(float from, float to)
{
	return from + frand()*(to - from);
}

void
frand_init()
{
	srand(0x666);
}
