#pragma once

#include <stdint.h>

uint32_t
irand();

uint32_t
irand(int top);

float
frand();

float
frand(float from, float to);

void
rand_init();
