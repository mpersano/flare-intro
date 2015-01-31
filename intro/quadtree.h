#pragma once

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <ggl/vertex_array.h>

#include "frustum.h"

struct frob
{
	frob(const glm::vec2& center, float height, float radius);

	void draw() const;

	static const int SIDES = 6;

	glm::vec2 center_;
	float height_;
	float radius_;
	ggl::vertex_array<ggl::vertex_texcoord<GLfloat, 3, GLshort, 1>> tri_strips_[SIDES + 1];
};

struct quadtree_node
{
	quadtree_node(const glm::vec2& min, const glm::vec2& max)
	: min_(min), max_(max)
	{ }

	virtual ~quadtree_node() = default;

	virtual void draw(const glm::mat4& mv, const frustum& f) const = 0;
	virtual void insert(const frob& f) = 0;

	bounding_box box_;
	glm::vec2 min_, max_;
};

std::unique_ptr<quadtree_node>
make_quadtree_node(const glm::vec2& min, const glm::vec2& max);
