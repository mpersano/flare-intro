#include <memory>

#include <unistd.h>

#include <GL/glew.h>

#include <ggl/window.h>
#include <ggl/panic.h>

#include <AL/alc.h>
#include <AL/al.h>

#include "ogg_player.h"
#include "fx.h"
#include "tube.h"
#include "util.h"

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
: ggl::window(512, 256)
, fx_(new tube(width_, height_))
, start_t_(now())
, frame_count_(0)
, mute_(mute)
{
	if (!mute_)
		init_openal();
}

intro_window::~intro_window()
{
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

		fx_->set_ogg_player(player_.get());
	}

	start_t_ = last_fps_update_t_ = now();
}

void
intro_window::draw()
{
	if (player_)
		player_->update();

	glViewport(0, 0, width_, height_);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	float t = now();

	fx_->draw(t - start_t_);

	const int FRAMES_PER_FPS_UPDATE = 16;

	if (++frame_count_ == FRAMES_PER_FPS_UPDATE) {
		printf("%.2f\n", (static_cast<float>(FRAMES_PER_FPS_UPDATE))/(t - last_fps_update_t_));
		last_fps_update_t_ = t;
		frame_count_ = 0;
	}
}

} // namespace

int
main(int argc, char *argv[])
{
	bool mute = false;

	int c;

	while ((c = getopt(argc, argv, "m")) != EOF) {
		switch (c) {
			case 'm':
				mute = true;
				break;
		}
	}

	intro_window intro(mute);
	intro.init();
	intro.run();
}
