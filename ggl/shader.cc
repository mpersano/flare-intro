#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "panic.h"
#include "file.h"
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
	file in_file(path, "rb");
	if (!in_file)
		panic("failed to open %s: %s\n", path, strerror(errno));

	struct stat sb;
	fstat(fileno(in_file.fp), &sb);

	const size_t size = sb.st_size;

	std::vector<char> buf(size + 1, '\0');

	if (fread(&buf[0], 1, size, in_file.fp) != size)
		panic("failed to read %s: %s\n", path, strerror(errno));

	set_source(&buf[0]);
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
