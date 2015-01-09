#pragma once

#include "ogg_player.h"

struct spectrum
{
	spectrum(const ogg_player& player, unsigned cur_ms);

	void draw_bars(int num_bands, int width, int height) const;

	enum {
		WINDOW_SIZE = 4096,
		LOG2_WINDOW_SIZE = 12,
	};

	float sample_window[WINDOW_SIZE];
	float spectrum_window[WINDOW_SIZE/2];
};
