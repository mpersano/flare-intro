#include <stdint.h>
#include <cstdio>

#include "util.h"

#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffff
#endif

namespace {

// WELL512 random number generator by Chris Lomont
// see http://www.lomont.org/Math/Papers/2008/Lomont_PRNG_2008.pdf

/* initialize state to random bits */
static uint32_t state[16] = {
	0xaebd5197, 0x82ac3b20, 0xe9ea08c8, 0x5c49c559,
	0xf3a6690c, 0xbb8f4674, 0xa123d3b6, 0x3edc3a04,
	0x9c451890, 0xc8dce9e7, 0x6f4b8ca3, 0xac3ed48e,
	0x81c509e2, 0x21c92e27, 0x8f12ba17, 0x15f18553,
};

/* init should also reset this to 0 */
static uint32_t index = 0;

/* return 32 bit random number */
uint32_t
WELLRNG512()
{
	uint32_t a, b, c, d;

	a = state[index];
	c = state[(index+13)&15];
	b = a^c^(a<<16)^(c<<15);
	c = state[(index+9)&15];
	c ^= (c>>11);
	a = state[index] = b^c;
	d = a^((a<<5)&0xDA442D24UL);
	index = (index + 15)&15;
	a = state[index];
	state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);

	return state[index];
}

}

uint32_t
irand()
{
	return WELLRNG512();
}

uint32_t
irand(int top)
{
	return irand()%top;
}

float
frand()
{
	return static_cast<float>(irand())/static_cast<float>(UINT32_MAX);
}

float
frand(float from, float to)
{
	return from + frand()*(to - from);
}

void
rand_init()
{ }
