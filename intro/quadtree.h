#pragma once

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "frustum.h"

struct frob
{
	frob(const glm::vec2& center, float height, float radius)
	: center_(center), height_(height), radius_(radius)
	{ }

	void draw() const;

	void draw_section(float h) const;

	static const int SIDES = 6;

	glm::vec2 center_;
	float height_;
	float radius_;
};

class quadtree_node
{
public:
	quadtree_node(const glm::vec2& min, const glm::vec2& max)
	: min_(min), max_(max)
	{ }

	virtual ~quadtree_node() = default;

	virtual void draw(const glm::mat4& mv, const frustum& f) const = 0;
	virtual void insert(const frob& f) = 0;

protected:
	glm::vec2 min_, max_;
	bounding_box bbox_;
};

std::unique_ptr<quadtree_node>
make_quadtree_node(const glm::vec2& min, const glm::vec2& max);
