#pragma once

#include <GL/glew.h>

#include "texture.h"

namespace ggl {

class texture;

class framebuffer
{
public:
	framebuffer(int width, int height);
	~framebuffer();

	framebuffer(const framebuffer&) = delete;
	framebuffer& operator=(const framebuffer&) = delete;

	const texture& get_texture() const
	{ return texture_; }

	void bind() const;
	void unbind() const;

private:
	texture texture_;
	GLuint id_;
};

}
