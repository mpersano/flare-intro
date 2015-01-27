#include <cmath>

#include <ggl/vertex_array.h>
#include <ggl/pixmap.h>

#include "common.h"
#include "program_manager.h"
#include "text.h"

namespace {

const float FADE_IN_DURATION = 2;
const float FADE_OUT_DURATION = 1;

}

text::label::label(const char *source, int x, int y, float start_t, float duration, const twitch_vector& twitches)
: start_t_(start_t)
, duration_(duration)
, twitches_(twitches)
{
	texture_.load(*ggl::pixmap::load_from_png(source));

	texture_.set_wrap_s(GL_CLAMP);
	texture_.set_wrap_t(GL_CLAMP);

	texture_.set_mag_filter(GL_LINEAR);
	texture_.set_min_filter(GL_LINEAR);

	texture_.set_env_mode(GL_MODULATE);

	const int w = texture_.get_orig_width();
	const int h = texture_.get_orig_height();

	const float du = static_cast<float>(texture_.get_orig_width())/texture_.get_width();
	const float dv = static_cast<float>(texture_.get_orig_height())/texture_.get_height();

	va_.add_vertex({ { x, y }, { 0, dv } });
	va_.add_vertex({ { x + w, y }, { du, dv } });
	va_.add_vertex({ { x, y + h }, { 0, 0 } });
	va_.add_vertex({ { x + w, y + h }, { du, 0 } });
}

void
text::label::draw(float t) const
{
	if (t < start_t_ || t > start_t_ + duration_)
		return;

	t -= start_t_;

	float fade;

	if (t < FADE_IN_DURATION)
		fade = t/FADE_IN_DURATION;
	else if (t > duration_ - FADE_OUT_DURATION)
		fade = 1 - (t - (duration_ - FADE_OUT_DURATION))/FADE_OUT_DURATION;
	else
		fade = 1;

	float dist = 0;

	for (auto& p : twitches_) {
		const float center = p.first;
		const float base = p.second;

		if (t >= center - base && t <= center + base)
			dist += cosf(.5*M_PI*(center - t)/base)*(2./texture_.get_width());
	}

	texture_.bind();

	const ggl::program *prog = get_program(PROG_TEXTBLUR);

	prog->use();

	prog->set_uniform_f("dist", dist);
	prog->set_uniform_f("phase", 20.*t);
	prog->set_uniform_f("vscale", texture_.get_height());
	prog->set_uniform_f("fade", fade);

	va_.draw(GL_TRIANGLE_STRIP);
}

text::text()
{
	struct {
		const char *source;
		int x, y;
		float start_t, duration;
		twitch_vector twitches;
	} label_defs[] = {
		{ "data/images/1.png", 20, 70, 1, 4, { { .75, .1 }, { 1.2, .2 } } },
		{ "data/images/2.png", 20, 20, 2, 3, { { .75, .1 }, { 1.75, .5 } } },
		{ "data/images/3.png", 20, 70, 4, 4, { { .75, .1 } } },
		{ "data/images/4.png", 20, 20, 5, 3, { { .75, .1 }, { 1.75, .5 } } },
	};

	for (const auto& p : label_defs)
		labels_.push_back(std::unique_ptr<label>(new label(p.source, p.x, p.y, p.start_t, p.duration, p.twitches)));
}

void
text::draw(float t) const
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, g_viewport_width, 0, g_viewport_height, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (const auto& p : labels_)
		p->draw(t);
}
