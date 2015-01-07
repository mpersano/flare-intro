#pragma once

#include <vector>

#include "fx.h"
#include "bezier.h"
#include "mesh.h"
#include "scene.h"

class tube : public fx
{
public:
	tube(int width, int height);

	void draw(float t) const;

private:
	void gen_path(const glm::vec3& p0, const glm::vec3& p1, int depth);
	void gen_segment(const glm::vec3& p0, const glm::vec3& p1);

	std::vector<bezier> segs_;
	sg::group_node scene_;
};
