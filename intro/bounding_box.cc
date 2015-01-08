#include <algorithm>

#include "bounding_box.h"

bounding_box&
bounding_box::operator+=(const glm::vec3& v)
{
	if (!initialized_) {
		min = max = v;
		initialized_ = true;
	} else {
		min.x = std::min(min.x, v.x);
		min.y = std::min(min.y, v.y);
		min.z = std::min(min.z, v.z);

		max.x = std::max(max.x, v.x);
		max.y = std::max(max.y, v.y);
		max.z = std::max(max.z, v.z);
	}

	return *this;
}
