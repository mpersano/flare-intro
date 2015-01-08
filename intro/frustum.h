#pragma once

#include <glm/glm.hpp>

#include "bounding_box.h"

class frustum
{
public:
	frustum(float fov_y, float aspect, float z_near, float z_far);

	bool intersects(const glm::mat4& transform, const bounding_box& box) const;

private:
	glm::vec4 planes_[6];
};
