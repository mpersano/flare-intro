#include <unistd.h>
#include <cmath>

#include <GL/glew.h>

#include <memory>

#include <ggl/window.h>
#include <ggl/panic.h>

#include <AL/alc.h>
#include <AL/al.h>

#include "common.h"
#include "spectrum.h"
#include "ogg_player.h"
#include "fx.h"
#include "tube.h"
#include "util.h"

int g_viewport_width;
int g_viewport_height;

float g_spectrum_bars[NUM_SPECTRUM_BANDS];

namespace {

float
now()
{
	timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return .001*(tp.tv_sec*1000 + tp.tv_nsec/1000000);
}

class intro_window : public ggl::window
{
public:
	intro_window(bool mute = false);
	~intro_window();

	void init();
	void draw();

private:
	void init_openal();
	void release_openal();

	void update_spectrum_bars(float t) const;

	ALCdevice *al_device_;
	ALCcontext *al_context_;

	std::unique_ptr<ogg_player> player_;
	std::unique_ptr<fx> fx_;
	float start_t_;
	int frame_count_;
	float last_fps_update_t_;
	bool mute_;
};

intro_window::intro_window(bool mute)
: ggl::window(g_viewport_width, g_viewport_height)
, fx_(new tube)
, start_t_(now())
, frame_count_(0)
, mute_(mute)
{
	if (!mute_)
		init_openal();
}

intro_window::~intro_window()
{
	player_.reset(nullptr);

	if (!mute_)
		release_openal();
}

void
intro_window::init_openal()
{
	if (!(al_device_ = alcOpenDevice(nullptr)))
		panic("alcOpenDevice failed");

	if (!(al_context_ = alcCreateContext(al_device_, nullptr)))
		panic("alcCreateContext failed");

	alcMakeContextCurrent(al_context_);
	alGetError();
}

void
intro_window::release_openal()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(al_context_);
	alcCloseDevice(al_device_);
}

void
intro_window::init()
{
	if (!mute_) {
		player_.reset(new ogg_player);

		player_->open("data/music/music.ogg");

		player_->set_gain(1.);
		player_->start();
	}

	start_t_ = last_fps_update_t_ = now();
}

void
intro_window::draw()
{
	if (player_)
		player_->update();

	glViewport(0, 0, g_viewport_width, g_viewport_height);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	const float t = now();

	update_spectrum_bars(t - start_t_);

	fx_->draw(t - start_t_);

	const int FRAMES_PER_FPS_UPDATE = 16;

	if (++frame_count_ == FRAMES_PER_FPS_UPDATE) {
		printf("%.2f\n", (static_cast<float>(FRAMES_PER_FPS_UPDATE))/(t - last_fps_update_t_));
		last_fps_update_t_ = t;
		frame_count_ = 0;
	}
}

void
intro_window::update_spectrum_bars(float t) const
{
	spectrum s(*player_, static_cast<unsigned>(t*1000.));

	const int num_samples = spectrum::WINDOW_SIZE/8;
	const int samples_per_band = num_samples/NUM_SPECTRUM_BANDS;

	for (int i = 0; i < NUM_SPECTRUM_BANDS; i++) {
		float w = 0;

		for (int j = 0; j < samples_per_band; j++)
			w += s.spectrum_window[i*samples_per_band + j];

		w /= samples_per_band;
		w = sqrtf(w);

		g_spectrum_bars[i] = w;
	}

	// draw bars

	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, g_viewport_width, 0, g_viewport_height, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(20, 20, 0);
	s.draw_bars(30, 400, 100);
}

} // namespace

int
main(int argc, char *argv[])
{
	bool mute = false;

	g_viewport_width = 512;
	g_viewport_height = 256;

	int c;

	while ((c = getopt(argc, argv, "mw:h:")) != EOF) {
		switch (c) {
			case 'm':
				mute = true;
				break;

			case 'w':
				g_viewport_width = atoi(optarg);
				break;

			case 'h':
				g_viewport_height = atoi(optarg);
				break;
		}
	}

	intro_window intro(mute);
	intro.init();
	intro.run();
}
