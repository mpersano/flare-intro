#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <ggl/vertex_array.h>

struct boid
{
public:
	boid();

	void draw() const;
	void update(float dt, const glm::vec3& target);

private:
	void init_faces();

	glm::vec3 position_;
	glm::mat4 direction_;
	float ang_speed_;
	ggl::indexed_vertex_array<GLubyte, ggl::vertex_texcoord<GLfloat, 3, GLfloat, 3>> va_;
};
