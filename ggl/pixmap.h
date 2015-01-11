#pragma once

#include <cstdint>
#include <cstddef>

#include <memory>
#include <vector>

namespace ggl {

struct pixmap
{
	enum class pixel_type { GRAY, GRAY_ALPHA, RGB, RGB_ALPHA };

	pixmap(size_t width, size_t height, pixel_type type);

	pixmap resize(size_t new_width, size_t new_height) const;

	size_t get_pixel_size() const;
	size_t get_row_stride() const;

	static std::unique_ptr<pixmap> load_from_png(const char *path);
	void save_as_png(const char *path) const;

	size_t width;
	size_t height;
	pixel_type type;
	std::vector<uint8_t> data;
};

} // ggl
