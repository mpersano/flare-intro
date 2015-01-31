#include <unistd.h>
#include <cmath>

#include <GL/glew.h>

#include <memory>

#include <ggl/window.h>
#include <ggl/panic.h>

#include <AL/alc.h>
#include <AL/al.h>

#include "common.h"
#include "program_manager.h"
#include "spectrum.h"
#include "ogg_player.h"
#include "fx.h"
#include "tube.h"
#include "text.h"
#include "boids.h"
#include "util.h"

int g_viewport_width;
int g_viewport_height;

namespace {

class intro_window : public ggl::window
{
public:
	intro_window(bool mute = false, bool fullscreen = false);
	~intro_window();

	void init();
	void draw(float t);

private:
	void init_openal();
	void release_openal();

	void update_spectrum_bars(float t) const;

	ALCdevice *al_device_;
	ALCcontext *al_context_;

	std::unique_ptr<ogg_player> player_;
	std::unique_ptr<fx> tube_, text_, boids_;
	int frame_count_;
	float last_fps_update_t_;
	bool mute_;
};

intro_window::intro_window(bool mute, bool fullscreen)
: ggl::window(g_viewport_width, g_viewport_height, fullscreen)
, tube_(/* new tube */ nullptr)
, text_(new text)
, boids_(new boids)
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

	last_fps_update_t_ = 0;
}

void
intro_window::draw(float t)
{
	if (player_)
		player_->update();

	glViewport(0, 0, g_viewport_width, g_viewport_height);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (player_)
		update_spectrum_bars(t);

#if 0
	tube_->draw(t);
	text_->draw(t);
#else
	boids_->draw(t);
#endif

#if 1
	const int FRAMES_PER_FPS_UPDATE = 16;

	if (++frame_count_ == FRAMES_PER_FPS_UPDATE) {
		printf("%.2f\n", (static_cast<float>(FRAMES_PER_FPS_UPDATE))/(t - last_fps_update_t_));
		last_fps_update_t_ = t;
		frame_count_ = 0;
	}
#endif
}

void
intro_window::update_spectrum_bars(float t) const
{
	::update_spectrum_bars(*player_, static_cast<unsigned>(t*1000.));

#if 0
	// draw bars

	get_program(PROG_WHITE)->use();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, g_viewport_width, 0, g_viewport_height, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(20, 20, 0);

	const float scale = 400;

	const int width = 400;
	const int dx = width/NUM_SPECTRUM_BANDS;

	int x = 0;

	glBegin(GL_QUADS);

	for (int i = 0; i < NUM_SPECTRUM_BANDS; i++) {
		float w = g_spectrum_bars[i]*scale;

		glVertex2f(x, 0);
		glVertex2f(x, w);
		glVertex2f(x + dx - 1, w);
		glVertex2f(x + dx - 1, 0);

		x += dx;
	}

	glEnd();
#endif
}

} // namespace

int
main(int argc, char *argv[])
{
	bool mute = false;
	bool fullscreen = false;

	g_viewport_width = 512;
	g_viewport_height = 256;

	int c;

	while ((c = getopt(argc, argv, "mfw:h:")) != EOF) {
		switch (c) {
			case 'm':
				mute = true;
				break;

			case 'f':
				fullscreen = true;
				break;

			case 'w':
				g_viewport_width = atoi(optarg);
				break;

			case 'h':
				g_viewport_height = atoi(optarg);
				break;
		}
	}

	intro_window intro(mute, fullscreen);
	intro.init();
	intro.run();
}
