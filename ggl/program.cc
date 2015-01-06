#include <vector>

#include "panic.h"
#include "program.h"

namespace ggl {

program::program()
: id_(glCreateProgram())
{
}

program::~program()
{
	glDeleteProgram(id_);
}

void
program::attach(const shader& shader) const
{
	glAttachShader(id_, shader.get_id());
}

void
program::attach_vertex_shader(const char *path) const
{
	attach(GL_VERTEX_SHADER, path);
}

void
program::attach_fragment_shader(const char *path) const
{
	attach(GL_FRAGMENT_SHADER, path);
}

void
program::attach(GLenum shader_type, const char *shader_path) const
{
	ggl::shader shader(shader_type);
	shader.load_source(shader_path);
	shader.compile();

	attach(shader);
}

void
program::link() const
{
	glLinkProgram(id_);
}

GLint
program::get_uniform_location(const GLchar *name) const
{
	GLint rv = glGetUniformLocation(id_, name);
	if (rv == -1)
		panic("get_uniform_location failed for %s\n", name);
	return rv;
}

GLint
program::get_attribute_location(const GLchar *name) const
{
	GLint rv = glGetAttribLocation(id_, name);
	if (rv == -1)
		panic("get_attribute_location failed for %s\n", name);
	return rv;
}

void
program::set_uniform_f(const GLchar *name, GLfloat v0) const
{
	glUniform1f(get_uniform_location(name), v0);
}

void
program::set_uniform_f(const GLchar *name, GLfloat v0, GLfloat v1) const
{
	glUniform2f(get_uniform_location(name), v0, v1);
}

void
program::set_uniform_f(const GLchar *name, GLfloat v0, GLfloat v1, GLfloat v2) const
{
	glUniform3f(get_uniform_location(name), v0, v1, v2);
}

void
program::set_uniform_f(const GLchar *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const
{
	glUniform4f(get_uniform_location(name), v0, v1, v2, v3);
}

void
program::set_uniform_i(const GLchar *name, GLint v0) const
{
	glUniform1i(get_uniform_location(name), v0);
}

void
program::set_uniform_i(const GLchar *name, GLint v0, GLint v1) const
{
	glUniform2i(get_uniform_location(name), v0, v1);
}

void
program::set_uniform_i(const GLchar *name, GLint v0, GLint v1, GLint v2) const
{
	glUniform3i(get_uniform_location(name), v0, v1, v2);
}

void
program::set_uniform_i(const GLchar *name, GLint v0, GLint v1, GLint v2, GLint v3) const
{
	glUniform4i(get_uniform_location(name), v0, v1, v2, v3);
}

void
program::parameter_i(GLenum name, GLint value)
{
	glProgramParameteri(id_, name, value);
}

void
program::use() const
{
	glUseProgram(id_);
}

std::string
program::get_info_log() const
{
	std::string log_string;

	GLint length;
	glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &length);

	if (length) {
		GLint written;

		std::vector<GLchar> data(length + 1);
		glGetProgramInfoLog(id_, length, &written, &data[0]);

		log_string.assign(data.begin(), data.begin() + written);
	}

	return log_string;
}

}
