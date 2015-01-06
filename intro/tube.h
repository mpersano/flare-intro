#pragma once

#include <vector>

#include "fx.h"
#include "bezier.h"

class tube : public fx
{
public:
	tube(int width, int height);

	void draw(float t) const;

private:
	void gen_path(const glm::vec3& p0, const glm::vec3& p1, int depth);

	std::vector<bezier> segs_;
};
