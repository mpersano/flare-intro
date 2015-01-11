#include <cstdio>
#include <map>

#include "texture.h"

namespace ggl {

namespace {

std::map<pixmap::pixel_type, GLint> pixel_formats
	{ { pixmap::pixel_type::GRAY, GL_LUMINANCE },
	  { pixmap::pixel_type::GRAY_ALPHA, GL_LUMINANCE_ALPHA },
	  { pixmap::pixel_type::RGB, GL_RGB },
	  { pixmap::pixel_type::RGB_ALPHA, GL_RGBA } };

template <typename T>
static T
next_power_of_2(T n)
{
	T p = 1;

	while (p < n)
		p *= 2;

	return p;
}

} // anonymous namespaces

void
texture::load(const pixmap& pm)
{
	orig_width_ = pm.width;
	width_ = next_power_of_2(orig_width_);

	orig_height_ = pm.height;
	height_ = next_power_of_2(orig_height_);

	bind();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	const GLint format = pixel_formats[pm.type];

	pixmap resized_pm = pm.resize(width_, height_);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		width_, height_,
		0,
		format,
		GL_UNSIGNED_BYTE,
		&resized_pm.data[0]);
}

}
