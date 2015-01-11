#include <cstdlib>

#include "util.h"

int
irand()
{
	return rand();
}

float
frand()
{
	return static_cast<float>(irand())/RAND_MAX;
}

float
frand(float from, float to)
{
	return from + frand()*(to - from);
}

void
rand_init()
{
	srand(0x666);
}
