#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

namespace ggl
{

struct file
{
	file(const char *path, const char *mode)
	: fp(fopen(path, mode))
	{ }

	file(const file&) = delete;
	file& operator=(const file&) = delete;

	~file()
	{ fclose(fp); }

	operator bool() const
	{ return fp != nullptr; }

	FILE *fp;
};

struct file_reader : file
{
	file_reader(const char *path);

	uint8_t read_uint8();
	uint16_t read_uint16();
	uint32_t read_uint32();
	std::string read_string();
};

struct file_writer : file
{
	file_writer(const char *path);

	void write_uint8(uint8_t value);
	void write_uint16(uint16_t value);
	void write_uint32(uint32_t value);
	void write_string(const std::string& str);
};

} // ggl
