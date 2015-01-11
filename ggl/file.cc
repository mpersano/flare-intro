#include <cmath>

#include "panic.h"
#include "file.h"

namespace ggl {

file_reader::file_reader(const char *path)
: file(path, "rb")
{ }

uint8_t
file_reader::read_uint8()
{
	if (!fp)
		panic("read from invalid file?");

	int val = fgetc(fp);

	if (val == EOF)
		panic("unexpected end of file");

	return val;
}

uint16_t
file_reader::read_uint16()
{
	uint8_t hi = read_uint8();
	uint8_t lo = read_uint8();
	return hi << 8 | lo;
}

uint32_t
file_reader::read_uint32()
{
	uint16_t hi = read_uint16();
	uint16_t lo = read_uint16();
	return hi << 16 | lo;
}

std::string
file_reader::read_string()
{
	std::string str;

	size_t len = read_uint8();
	for (size_t i = 0; i < len; i++)
		str.push_back(read_uint8());

	return str;
}

file_writer::file_writer(const char *path)
: file(path, "wb")
{ }

void
file_writer::write_uint8(uint8_t value)
{
	fputc(value, fp);
}

void
file_writer::write_uint16(uint16_t value)
{
	write_uint8(value >> 8);
	write_uint8(value);
}

void
file_writer::write_uint32(uint32_t value)
{
	write_uint16(value >> 16);
	write_uint16(value);
}

void
file_writer::write_string(const std::string& str)
{
	write_uint8(str.size());
	fwrite(str.c_str(), str.size(), 1, fp);
}

} // ggl
