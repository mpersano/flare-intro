#include <fstream>
#include <string>
#include <vector>

#include "panic.h"
#include "shader.h"

namespace ggl {

shader::shader(GLenum type)
: id_(glCreateShader(type))
{ }

shader::~shader()
{
	glDeleteShader(id_);
}

void
shader::set_source(const char *source) const
{
	glShaderSource(id_, 1, &source, 0);
}

void
shader::load_source(const char *path) const
{
	std::ifstream ifs(path);
	// STOLEN FROM STACKOVERFLOW!$#@!! *shame*
	std::string source((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	set_source(source.c_str());
}

void
shader::compile() const
{
	glCompileShader(id_);

	GLint status;
	glGetShaderiv(id_, GL_COMPILE_STATUS, &status);
	if (!status)
		panic("%s", get_info_log().c_str());
}

std::string
shader::get_info_log() const
{
	std::string log_string;

	GLint length;
	glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		GLint written;

		std::vector<GLchar> data(length + 1);
		glGetShaderInfoLog(id_, length, &written, &data[0]);

		log_string.assign(data.begin(), data.begin() + written);
	}

	return log_string;
}

}
