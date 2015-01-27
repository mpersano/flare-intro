#include <cmath>

#include <ggl/vertex_array.h>
#include <ggl/pixmap.h>

#include "common.h"
#include "program_manager.h"
#include "text.h"

text::label::label(const char *source, int x, int y)
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
	texture_.bind();

	const ggl::program *prog = get_program(program_manager::TextBlur);

	prog->use();

	float l = (.5f + .5f*sinf(10.*t))*(2./texture_.get_width());
	prog->set_uniform_f("dist", l);
	prog->set_uniform_f("phase", 20.*t);
	prog->set_uniform_f("vscale", texture_.get_height());

	va_.draw(GL_TRIANGLE_STRIP);
}

text::text()
{
	struct {
		const char *source;
		int x, y;
	} label_defs[] = {
		{ "data/images/1.png", 20, 70 },
		{ "data/images/2.png", 20, 20 },
	};

	for (const auto& p : label_defs)
		labels_.push_back(std::unique_ptr<label>(new label(p.source, p.x, p.y)));
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
