#pragma once

#include <GL/glew.h>

#include <vector>

#include <glm/vec3.hpp>

#include <ggl/texture.h>
#include <ggl/program.h>
#include <ggl/vertex_array.h>

#include "fx.h"
#include "bezier.h"
#include "scene.h"

struct particle
{
	using vertex_array = ggl::vertex_array<ggl::vertex_texcoord<GLfloat, 3, GLshort, 2>>;

	particle(const std::vector<bezier>& path);

	void draw(vertex_array& va, const glm::vec3& up, const glm::vec3& right, float t) const;

	float speed_;
	float pos_offset_;
	glm::vec3 offset_;
	const std::vector<bezier>& path_;
};

struct camera_path
{
	camera_path(const bezier& path, const glm::vec3& target, float ttl);

	glm::mat4 get_mv(float t) const;

	bezier path_;
	glm::vec3 up_;
	glm::vec3 target_;
	float ttl_;
};

class tube : public fx
{
public:
	tube();

	void draw(float t) const;

private:
	void draw(const glm::mat4& mv, bool show_particles, float t) const;

	void gen_path(const glm::vec3& p0, const glm::vec3& p1, int depth);
	void gen_segment(const glm::vec3& p0, const glm::vec3& p1);

	std::vector<bezier> segs_;
	sg::group_node scene_;
	std::vector<particle> particles_;
	std::vector<camera_path> camera_paths_;

	ggl::texture particle_texture_;
	ggl::program particle_program_;
};
