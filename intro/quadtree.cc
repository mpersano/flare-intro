#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include <ggl/vertex_array.h>

#include "quadtree.h"

namespace {

struct quadtree_leaf : quadtree_node
{
	quadtree_leaf(const glm::vec2& min, const glm::vec2& max)
	: quadtree_node(min, max)
	{ }

	void draw(const glm::mat4& mv, const frustum& f) const;
	void insert(const cell& f);

	ggl::indexed_vertex_array<GLuint, ggl::vertex_texcoord<GLfloat, 3, GLshort, 1>> va_;
};

struct quadtree_inner : quadtree_node
{
	quadtree_inner(const glm::vec2& min, const glm::vec2& max)
	: quadtree_node(min, max)
	{ }

	void draw(const glm::mat4& mv, const frustum& f) const;
	void insert(const cell& f);

	std::unique_ptr<quadtree_node> children_[4];
};

void
quadtree_leaf::draw(const glm::mat4& mv, const frustum& f) const
{
	if (f.intersects(mv, box_)) {
		// box_.draw();
		va_.draw(GL_TRIANGLES);
	}
}

void
quadtree_leaf::insert(const cell& f)
{
	const glm::vec3& center = f.center;
	const glm::vec3& normal = f.normal;
	const float radius = f.radius;

	assert(center.x >= min_.x && center.x < max_.x);
	assert(center.z >= min_.y && center.z < max_.y);

	const glm::vec3 up = glm::normalize(glm::cross(glm::vec3(1, 0, 0), normal));
	const glm::vec3 left = glm::normalize(glm::cross(up, normal));

	const int base_index = va_.get_num_verts();

	va_.add_vertex({ { center.x, center.y, center.z }, { 0 } });

	static const int SIDES = 6;

	float a = 0;
	const float da = 2*M_PI/SIDES;

	for (int i = 0; i < SIDES; i++) {
		const float dx = radius*cosf(a);
		const float dy = radius*sinf(a);
		const glm::vec3 v = center + dx*left + dy*up;

		va_.add_vertex({ { v.x, v.y, v.z }, { 1 } });

		box_ += v;
		a += da;
	}

	for (int i = 0; i < SIDES; i++) {
		va_.add_index(base_index);
		va_.add_index(base_index + (i + 1)%SIDES + 1);
		va_.add_index(base_index + i + 1);
	}
}

void
quadtree_inner::draw(const glm::mat4& mv, const frustum& f) const
{
	if (f.intersects(mv, box_)) {
		// box_.draw();

		for (const auto& child : children_) {
			if (child)
				child->draw(mv, f);
		}
	}
}

void
quadtree_inner::insert(const cell& f)
{
	assert(f.center.x >= min_.x && f.center.x < max_.x);
	assert(f.center.z >= min_.y && f.center.z < max_.y);

	glm::vec2 mid = .5f*(min_ + max_);

	if (f.center.x < mid.x) {
		if (f.center.z < mid.y) {
			// insert into 0
			if (!children_[0])
				children_[0] = make_quadtree_node(min_, mid);

			children_[0]->insert(f);
			box_ += children_[0]->box_;
		} else {
			// insert into 1
			if (!children_[1])
				children_[1] = make_quadtree_node(glm::vec2(min_.x, mid.y), glm::vec2(mid.x, max_.y));

			children_[1]->insert(f);
			box_ += children_[1]->box_;
		}
	} else {
		if (f.center.z < mid.y) {
			// insert into 2
			if (!children_[2])
				children_[2] = make_quadtree_node(glm::vec2(mid.x, min_.y), glm::vec2(max_.x, mid.y));

			children_[2]->insert(f);

			box_ += children_[2]->box_;
		} else {
			// insert into 3
			if (!children_[3])
				children_[3] = make_quadtree_node(mid, max_);

			children_[3]->insert(f);
			box_ += children_[3]->box_;
		}
	}
}

} // namespace

std::unique_ptr<quadtree_node>
make_quadtree_node(const glm::vec2& min, const glm::vec2& max)
{
	const float MIN_SIZE = 100.;

	if (max.x - min.x < MIN_SIZE && max.y - min.y < MIN_SIZE)
		return std::unique_ptr<quadtree_node>(new quadtree_leaf(min, max));
	else
		return std::unique_ptr<quadtree_node>(new quadtree_inner(min, max));
}
