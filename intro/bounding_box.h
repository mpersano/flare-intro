#pragma once

#include <glm/vec3.hpp>

class bounding_box
{
public:
	bounding_box()
	: initialized_(false)
	{ }

	bounding_box(const glm::vec3& min, const glm::vec3& max)
	: min(min)
	, max(max)
	, initialized_(true)
	{ }

	bounding_box& operator+=(const glm::vec3& v);

	glm::vec3 min, max;

private:
	bool initialized_;
};
