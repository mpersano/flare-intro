#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

struct mesh
{
	struct poly {
		std::vector<int> indices;
	};

	std::vector<glm::vec3> verts;
	std::vector<poly> polys;
};

using mesh_ptr = std::shared_ptr<mesh>;
