#include <cmath>

#include "util.h"
#include "particle_texture.h"

enum {
	NUM_SPIKES = 130,
	TEXTURE_SIZE = 256,
};

struct spike
{
	spike()
	: angle(frand(0, 2.*M_PI))
	{ }

	float angle;
};

struct rgb
{
	rgb(float r, float g, float b)
	: r(r), g(g), b(b)
	{ }

	rgb operator*(float s) const
	{ return rgb(r*s, g*s, b*s); }

	rgb operator+(const rgb& o) const
	{ return rgb(r + o.r, g + o.g, b + o.b); }

	rgb& operator+=(const rgb& o)
	{ r += o.r; g += o.g; b += o.b; return *this; }

	float r, g, b;
};

static const rgb center_color(1, 1, 1);
static const rgb glow_color(1, .8, 0);
static const rgb spike_color(1, .5, 0);

const float mix = .8;

static spike spikes[NUM_SPIKES];

std::unique_ptr<ggl::pixmap>
render_particle_texture()
{
	std::unique_ptr<ggl::pixmap> pm(new ggl::pixmap(TEXTURE_SIZE, TEXTURE_SIZE, ggl::pixmap::pixel_type::RGB));

	uint8_t *q = &pm->data[0];

	for (int i = 0; i < TEXTURE_SIZE; i++) {
		const float x = i - .5*TEXTURE_SIZE;

		for (int j = 0; j < TEXTURE_SIZE; j++) {
			const float y = j - .5*TEXTURE_SIZE;

			const float a = atan2f(y, x) + M_PI;
			const float r = sqrt(x*x + y*y);

			rgb color(0, 0, 0);

			// spikes

			for (const spike *p = spikes; p != &spikes[NUM_SPIKES]; p++) {
				float da = fabs(a - p->angle);
				if (da > M_PI)
					da = 2.*M_PI - da;
				da *= .05*(1 + r);

				const float v0 = expf(-30*da*da)/(.7*(1. + .1*r));

				color += spike_color*v0*.05;
			}

			// center glow

			{
			const float gauss = .003;
			const float linear = .05;
			color += center_color*mix*expf(-gauss*r*r) + glow_color*(1. - mix)*expf(-linear*r);
			}

			int ir = color.r*255;
			if (ir > 255)
				ir = 255;

			int ig = color.g*255;
			if (ig > 255)
				ig = 255;

			int ib = color.b*255;
			if (ib > 255)
				ib = 255;

			*q++ = ir;
			*q++ = ig;
			*q++ = ib;
		}
	}

	return pm;
}
