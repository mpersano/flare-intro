#pragma once

#include <vector>

#include <ggl/texture.h>
#include <ggl/program.h>

#include "fx.h"
#include "bezier.h"
#include "scene.h"

class particle;

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
	std::vector<std::unique_ptr<particle>> particles_;

	ggl::texture particle_texture_;
	ggl::program particle_program_;
};
