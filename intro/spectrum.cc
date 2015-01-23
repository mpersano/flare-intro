#include <cmath>
#include <cstring>

#include "fft.h"
#include "spectrum.h"

float g_spectrum_bars[NUM_SPECTRUM_BANDS];

enum {
	WINDOW_SIZE = 4096,
	LOG2_WINDOW_SIZE = 12,
};

void
update_spectrum_bars(const ogg_player& player, unsigned cur_ms)
{
	static float sample_window[WINDOW_SIZE];

	const int buffer_samples = player.get_num_buffer_samples();
	const int total_buffer_samples = buffer_samples*ogg_player::NUM_BUFFERS;

	// ogg_player::rate samples --> 1 second
	// y samples --> cur_ms msecs

	// 1 tic --> ogg_player::rate/30 samples

	unsigned sample_index = (static_cast<unsigned long long>(cur_ms)*player.rate/1000)%total_buffer_samples;

	for (int i = 0; i < WINDOW_SIZE; i++) {
		const ogg_player::buffer& buf = player.buffers[sample_index/buffer_samples];
		const int16_t *buffer_data = reinterpret_cast<const int16_t *>(buf.data);

		int j = sample_index%buffer_samples;

		switch (player.format) {
			case AL_FORMAT_MONO16:
				sample_window[i] = static_cast<float>(buffer_data[j])/(1 << 15);
				break;

			case AL_FORMAT_STEREO16:
				{
				int16_t l = buffer_data[j*2];
				int16_t r = buffer_data[j*2 + 1];
				sample_window[i] = (.5*(static_cast<float>(l) + static_cast<float>(r)))/(1 << 15);
				}
				break;
		}

		if (++sample_index == total_buffer_samples)
			sample_index = 0;
	}

	static float real[WINDOW_SIZE], imag[WINDOW_SIZE];
	memcpy(real, sample_window, sizeof real);
	memset(imag, 0, sizeof imag);

	fft(1, LOG2_WINDOW_SIZE, real, imag);

	const int num_samples = WINDOW_SIZE/8;
	const int samples_per_band = num_samples/NUM_SPECTRUM_BANDS;

	for (int i = 0; i < NUM_SPECTRUM_BANDS; i++) {
		float w = 0;

		int index = i*samples_per_band;

		for (int j = 0; j < samples_per_band; j++) {
			w += sqrtf(real[index]*real[index] + imag[index]*imag[index]);
			++index;
		}

		w /= samples_per_band;
		w = sqrtf(w);

		g_spectrum_bars[i] = w;
	}
}
