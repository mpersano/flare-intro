#include <cstdio>
#include <cstring>

#include <algorithm>

#include <png.h>

#include "panic.h"
#include "file.h"
#include "pixmap.h"

namespace ggl {

pixmap::pixmap(size_t width, size_t height, pixel_type type)
: width(width)
, height(height)
, type(type)
, data(width*height*get_pixel_size())
{ }

size_t
pixmap::get_pixel_size() const
{
	switch (type) {
		case pixmap::pixel_type::GRAY:
			return 1;

		case pixmap::pixel_type::GRAY_ALPHA:
			return 2;

		case pixmap::pixel_type::RGB:
			return 3;

		case pixmap::pixel_type::RGB_ALPHA:
			return 4;
	}
}

size_t
pixmap::get_row_stride() const
{
	return get_pixel_size()*width;
}

pixmap
pixmap::resize(size_t new_width, size_t new_height) const
{
	pixmap new_pixmap(new_width, new_height, type);

	uint8_t *dest = &new_pixmap.data[0];
	const size_t dest_stride = new_pixmap.get_row_stride();

	const uint8_t *src = &data[0];
	const size_t src_stride = get_row_stride();

	const size_t copy_height = std::min(height, new_height);
	const size_t copy_stride = std::min(src_stride, dest_stride);

	for (size_t i = 0; i < copy_height; i++) {
		::memcpy(dest, src, copy_stride);

		src += src_stride;
		dest += dest_stride;
	}

	return new_pixmap;
}

namespace {

png_byte
to_png_color_type(pixmap::pixel_type type)
{
	switch (type) {
		case pixmap::pixel_type::GRAY:
			return PNG_COLOR_TYPE_GRAY;

		case pixmap::pixel_type::GRAY_ALPHA:
			return PNG_COLOR_TYPE_GRAY_ALPHA;

		case pixmap::pixel_type::RGB:
			return PNG_COLOR_TYPE_RGB;

		case pixmap::pixel_type::RGB_ALPHA:
			return PNG_COLOR_TYPE_RGBA;
	}
}

pixmap::pixel_type
to_pixel_type(png_byte png_color_type)
{
	switch (png_color_type) {
		case PNG_COLOR_TYPE_GRAY:
			return pixmap::pixel_type::GRAY;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			return pixmap::pixel_type::GRAY_ALPHA;

		case PNG_COLOR_TYPE_RGB:
			return pixmap::pixel_type::RGB;

		case PNG_COLOR_TYPE_RGBA:
			return pixmap::pixel_type::RGB_ALPHA;

		default:
			panic("invalid PNG color type: %x", png_color_type);
	}
}

} // namespace

std::unique_ptr<pixmap>
pixmap::load_from_png(const char *path)
{
	file in_file(path, "rb");
	if (!in_file)
		panic("failed to open %s: %s", path, strerror(errno));

	png_structp png_ptr;

	if (!(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)))
		panic("png_create_read_struct");

	png_infop info_ptr;

	if (!(info_ptr = png_create_info_struct(png_ptr)))
		panic("png_create_info_struct");

	if (setjmp(png_jmpbuf(png_ptr)))
		panic("png error?");

	png_init_io(png_ptr, in_file.fp);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);

	if (png_get_bit_depth(png_ptr, info_ptr) != 8)
		panic("invalid PNG bit depth");

	const size_t width = png_get_image_width(png_ptr, info_ptr);
	const size_t height = png_get_image_height(png_ptr, info_ptr);
	const png_byte type = png_get_color_type(png_ptr, info_ptr);

	std::unique_ptr<pixmap> pm(new pixmap(width, height, to_pixel_type(type)));

	png_bytep *rows = png_get_rows(png_ptr, info_ptr);

	uint8_t *dest = &pm->data[0];
	const size_t stride = pm->get_row_stride();

	for (size_t i = 0; i < height; i++) {
		::memcpy(dest, rows[i], stride);
		dest += stride;
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	return pm;
}

void
pixmap::save_as_png(const char *path) const
{
	file out_file(path, "wb");
	if (!out_file)
		panic("failed to open %s: %s", strerror(errno));

	png_structp png_ptr;

	if (!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)))
		panic("png_create_write_struct");

	png_infop info_ptr;

	if (!(info_ptr = png_create_info_struct(png_ptr)))
		panic("png_create_info_struct");

	if (setjmp(png_jmpbuf(png_ptr)))
		panic("png error?");

	png_init_io(png_ptr, out_file.fp);

	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	png_set_IHDR(
		png_ptr,
		info_ptr,
		width, height,
		8,
		to_png_color_type(type),
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	const png_byte *src = reinterpret_cast<const png_byte *>(&data[0]);
	const size_t stride = get_row_stride();

	for (size_t i = 0; i < height; i++) {
		png_write_row(png_ptr, const_cast<png_byte *>(src));
		src += stride;
	}

	png_write_end(png_ptr, info_ptr);

	png_destroy_write_struct(&png_ptr, &info_ptr);
}


} // ggl
