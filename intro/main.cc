#include <memory>

#include <GL/glew.h>

#include <ggl/window.h>

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

	void draw();

private:
	std::unique_ptr<fx> fx_;
	float start_t_;
};

intro_window::intro_window()
: ggl::window(512, 512)
, fx_(new tube(width_, height_))
, start_t_(now())
{ }

void
intro_window::draw()
{
	glViewport(0, 0, width_, height_);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	fx_->draw(now() - start_t_);
}

} // namespace

int
main()
{
	frand_init();
	intro_window().run();
}
