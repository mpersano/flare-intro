#pragma once

#include <vector>
#include <memory>

#include <glm/vec3.hpp>

#include "bounding_box.h"

struct mesh
{
	void initialize_bounding_box();

	struct poly {
		std::vector<int> indices;
	};

	std::vector<glm::vec3> verts;
	std::vector<poly> polys;
	bounding_box bbox;
};

using mesh_ptr = std::shared_ptr<mesh>;
