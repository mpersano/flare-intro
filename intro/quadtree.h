#pragma once

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "frustum.h"

struct frob
{
	frob(const glm::vec2& center, float height, float radius);

	void draw() const;

	static const int SIDES = 6;

	glm::vec2 center_;
	float height_;
	float radius_;
	glm::vec3 verts_[2*SIDES];
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
