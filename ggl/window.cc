#include "window.h"
#include "panic.h"

#include <cstdio>
#include <cstdarg>

#include <SDL.h>
#include <GL/glew.h>

namespace ggl {

window::window(int width, int height)
: width_(width)
, height_(height)
, dump_frames_(false)
, frame_num_(0)
, frame_data_(3*width*height)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		panic("SDL_Init: %s", SDL_GetError());

	if (SDL_SetVideoMode(width, height, 0, SDL_OPENGL) == 0)
		panic("SDL_SetVideoMode: %s", SDL_GetError());

	if (GLenum rv = glewInit())
		panic("glewInit: %s", glewGetErrorString(rv));
}

window::~window()
{
	SDL_Quit();
}

void
window::do_redraw()
{
	draw();

	if (dump_frames_)
		dump_frame();

	SDL_GL_SwapBuffers();
}

void
window::run()
{
	for (;;) {
		do_redraw();

		if (!poll_events())
			break;
	}
}

bool
window::poll_events()
{
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				return false;

			case SDL_MOUSEBUTTONDOWN:
				on_mouse_button_down(event.button.button, event.button.x, event.button.y);
				break;

			case SDL_MOUSEBUTTONUP:
				on_mouse_button_up(event.button.button);
				break;

			case SDL_MOUSEMOTION:
				on_mouse_motion(event.motion.x, event.motion.y);
				break;
		}
	}

	return true;
}

void
window::dump_frame()
{
	glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, &frame_data_[0]); 

	char path[80];
	sprintf(path, "%05d.ppm", frame_num_);

	if (FILE *out = fopen(path, "wb")) {
		fprintf(out, "P6\n%d %d\n255\n", width_, height_);
		fwrite(&frame_data_[0], frame_data_.size(), 1, out);
		fclose(out);

		++frame_num_;
	}
}

void
window::set_dump_frames(bool b)
{
	dump_frames_ = b;
}

}
