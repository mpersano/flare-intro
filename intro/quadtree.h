#pragma once

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <ggl/vertex_array.h>

#include "frustum.h"

struct cell
{
	cell(const glm::vec3& center, const glm::vec3& normal, float radius);

	void draw() const;

	static const int SIDES = 6;

	glm::vec3 center_;
	ggl::vertex_array<ggl::vertex_texcoord<GLfloat, 3, GLshort, 1>> tri_strip_;
	bounding_box box_;
};

struct quadtree_node
{
	quadtree_node(const glm::vec2& min, const glm::vec2& max)
	: min_(min), max_(max)
	{ }

	virtual ~quadtree_node() = default;

	virtual void draw(const glm::mat4& mv, const frustum& f) const = 0;
	virtual void insert(const cell& f) = 0;

	bounding_box box_;
	glm::vec2 min_, max_;
};

std::unique_ptr<quadtree_node>
make_quadtree_node(const glm::vec2& min, const glm::vec2& max);
