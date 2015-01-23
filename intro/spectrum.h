#pragma once

#include "ogg_player.h"

static const int NUM_SPECTRUM_BANDS = 30;
extern float g_spectrum_bars[NUM_SPECTRUM_BANDS];

void
update_spectrum_bars(const ogg_player& player, unsigned cur_ms);
