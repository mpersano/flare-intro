#include <cmath>
#include <cstdio>

#include "frustum.h"

frustum::frustum(float fov_y, float aspect, float z_near, float z_far)
{
	const float f = tanf(.5*M_PI*fov_y/180.);

	planes_[0] = glm::vec4(0, 1, -f, 0);
	planes_[1] = glm::vec4(0, -1, -f, 0);
	planes_[2] = glm::vec4(1, 0, -f*aspect, 0);
	planes_[3] = glm::vec4(-1, 0, -f*aspect, 0);
	planes_[4] = glm::vec4(0, 0, -1, z_near);
	planes_[5] = glm::vec4(0, 0, 1, z_far);
}

bool
frustum::intersects(const glm::mat4& transform, const bounding_box& box) const
{
	const glm::vec4 verts[]
		{ glm::vec4(box.min.x, box.min.y, box.min.z, 1),
		  glm::vec4(box.max.x, box.min.y, box.min.z, 1),
		  glm::vec4(box.max.x, box.max.y, box.min.z, 1),
		  glm::vec4(box.min.x, box.max.y, box.min.z, 1),

		  glm::vec4(box.min.x, box.min.y, box.max.z, 1),
		  glm::vec4(box.max.x, box.min.y, box.max.z, 1),
		  glm::vec4(box.max.x, box.max.y, box.max.z, 1),
		  glm::vec4(box.min.x, box.max.y, box.max.z, 1) };

	glm::vec4 transformed_verts[8];

	for (int i = 0; i < 8; i++)
		transformed_verts[i] = transform*verts[i];

	for (auto& p : planes_) {
		bool in = false;

		for (auto& v : transformed_verts) {
			if (glm::dot(p, v) > 0) {
				in = true;
				break;
			}
		}

		if (!in)
			return false;
	}

	return true;
}
