#include <memory>

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
	intro_window();
	~intro_window();

	void start_music();
	void draw();

private:
	void init_openal();
	void release_openal();

	ALCdevice *al_device_;
	ALCcontext *al_context_;

	std::unique_ptr<ogg_player> player_;
	std::unique_ptr<fx> fx_;
	float start_t_;
};

intro_window::intro_window()
: ggl::window(512, 512)
, fx_(new tube(width_, height_))
, start_t_(now())
{
	init_openal();
}

intro_window::~intro_window()
{
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
intro_window::start_music()
{
	player_.reset(new ogg_player);

	player_->open("music.ogg");

	player_->set_gain(1.);
	player_->start();
}

void
intro_window::draw()
{
	player_->update();

	glViewport(0, 0, width_, height_);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	fx_->draw(now() - start_t_);
}

} // namespace

int
main()
{
	intro_window intro;

	intro.start_music();
	intro.run();
}
