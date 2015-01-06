#pragma once

#include <glm/glm.hpp>

struct bezier
{
	bezier(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	: p0(p0), p1(p1), p2(p2)
	{ }

	glm::vec3 eval(float u) const;
	glm::vec3 eval_dir(float u) const;

	glm::vec3 p0, p1, p2;
};
