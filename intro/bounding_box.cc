#include <algorithm>

#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

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

bounding_box&
bounding_box::operator+=(const bounding_box& box)
{
	operator+=(box.min);
	operator+=(box.max);

	return *this;
}

void
bounding_box::draw() const
{
	const glm::vec3 verts[] = {
		{ min.x, min.y, min.z },
		{ min.x, max.y, min.z },
		{ max.x, max.y, min.z },
		{ max.x, min.y, min.z },

		{ min.x, min.y, max.z },
		{ min.x, max.y, max.z },
		{ max.x, max.y, max.z },
		{ max.x, min.y, max.z },
	};

	const int faces[][4] = {
		{ 0, 1, 2, 3 },
		{ 6, 5, 4, 7 },
		{ 3, 2, 6, 7 },
		{ 1, 0, 4, 5 },
		{ 0, 3, 7, 4 },
		{ 1, 5, 6, 2 },
	};

	for (const auto& face : faces) {
		glBegin(GL_LINE_LOOP);

		glVertex3fv(glm::value_ptr(verts[face[0]]));
		glVertex3fv(glm::value_ptr(verts[face[1]]));
		glVertex3fv(glm::value_ptr(verts[face[2]]));
		glVertex3fv(glm::value_ptr(verts[face[3]]));

		glEnd();
	}
}
