#pragma once

#include <GL/glew.h>

#include <string>

#include "shader.h"

namespace ggl {

class program
{
public:
	program();
	~program();

	program(const program&) = delete;
	program& operator=(const program&) = delete;

	void attach(const shader& shader) const;
	void attach(GLenum type, const char *path) const;

	void attach_vertex_shader(const char *path) const;
	void attach_fragment_shader(const char *path) const;

	void link() const;

	GLint get_uniform_location(const GLchar *name) const;
	GLint get_attribute_location(const GLchar *name) const;

	void set_uniform_f(const GLchar *name, GLfloat v0) const;
	void set_uniform_f(const GLchar *name, GLfloat v0, GLfloat v1) const;
	void set_uniform_f(const GLchar *name, GLfloat v0, GLfloat v1, GLfloat v2) const;
	void set_uniform_f(const GLchar *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const;

	void set_uniform_i(const GLchar *name, GLint v0) const;
	void set_uniform_i(const GLchar *name, GLint v0, GLint v1) const;
	void set_uniform_i(const GLchar *name, GLint v0, GLint v1, GLint v2) const;
	void set_uniform_i(const GLchar *name, GLint v0, GLint v1, GLint v2, GLint v3) const;

	void parameter_i(GLenum name, GLint value);

	void use() const;

	std::string get_info_log() const;

private:
	GLuint id_;
};

}
