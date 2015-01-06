#pragma once

#include <GL/glew.h>

namespace ggl {

class texture
{
public:
	texture(int width, int height);
	~texture();

	texture(const texture&) = delete;
	texture& operator=(const texture&) = delete;

	GLuint get_gl_id() const
	{ return id_; }

	void bind() const;

private:
	GLuint id_;
};

}
