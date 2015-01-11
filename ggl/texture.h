#pragma once

#include <GL/glew.h>

#include "pixmap.h"

namespace ggl {

class texture
{
public:
	texture()
	: orig_width_(0), width_(0)
	, orig_height_(0), height_(0)
	{ glGenTextures(1, &id_); }

	~texture()
	{ glDeleteTextures(1, &id_); }

	texture(const texture&) = delete;
	texture& operator=(const texture&) = delete;

	void bind() const
	{ glBindTexture(GL_TEXTURE_2D, id_); }

	void set_wrap_s(GLint wrap) const
	{ set_parameter(GL_TEXTURE_WRAP_S, wrap); }

	void set_wrap_t(GLint wrap) const
	{ set_parameter(GL_TEXTURE_WRAP_T, wrap); }

	void set_mag_filter(GLint filter) const
	{ set_parameter(GL_TEXTURE_MAG_FILTER, filter); }

	void set_min_filter(GLint filter) const
	{ set_parameter(GL_TEXTURE_MIN_FILTER, filter); }

	void set_parameter(GLenum name, GLint value) const
	{ bind(); glTexParameteri(GL_TEXTURE_2D, name, value); }

	static void set_env_mode(GLint mode)
	{ glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode); }

	void load(const pixmap& pm);

	size_t get_width() const
	{ return width_; }

	size_t get_orig_width() const
	{ return orig_width_; }

	size_t get_height() const
	{ return height_; }

	size_t get_orig_height() const
	{ return orig_height_; }

// private:
	size_t orig_width_, width_;
	size_t orig_height_, height_;
	GLuint id_;
};

}
