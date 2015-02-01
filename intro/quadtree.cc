#include <cassert>

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include "quadtree.h"

int leaves_drawn;

namespace {

struct quadtree_leaf : quadtree_node
{
	quadtree_leaf(const glm::vec2& min, const glm::vec2& max)
	: quadtree_node(min, max)
	{ }

	void draw(const glm::mat4& mv, const frustum& f) const;
	void insert(const cell& f);

	std::vector<cell> cells_;
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
		++leaves_drawn;

		glLoadMatrixf(glm::value_ptr(mv));
		// box_.draw();

		for (const auto& f : cells_)
			f.draw();
	}
}

void
quadtree_leaf::insert(const cell& f)
{
	assert(f.center_.x >= min_.x && f.center_.x < max_.x);
	assert(f.center_.z >= min_.y && f.center_.z < max_.y);

	cells_.push_back(f);
	box_ += f.box_;
}

void
quadtree_inner::draw(const glm::mat4& mv, const frustum& f) const
{
	if (f.intersects(mv, box_)) {
		// glLoadMatrixf(glm::value_ptr(mv));
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
	assert(f.center_.x >= min_.x && f.center_.x < max_.x);
	assert(f.center_.z >= min_.y && f.center_.z < max_.y);

	glm::vec2 mid = .5f*(min_ + max_);

	if (f.center_.x < mid.x) {
		if (f.center_.z < mid.y) {
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
		if (f.center_.z < mid.y) {
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

cell::cell(const glm::vec3& center, const glm::vec3& normal, float radius)
: center_(center)
{
	glm::vec3 up = glm::normalize(glm::cross(glm::vec3(1, 0, 0), normal));
	glm::vec3 left = glm::normalize(glm::cross(up, normal));

	glm::vec3 verts[SIDES];

	float a = 0;
	const float da = 2*M_PI/SIDES;

	for (int i = 0; i < SIDES; i++) {
		const float dx = radius*cosf(a);
		const float dy = radius*sinf(a);

		verts[i] = center + dx*left + dy*up;
		box_ += verts[i];

		a += da;
	}

	tri_strip_.add_vertex({ { center.x, center.y, center.z }, { 0 } });

	for (int i = SIDES - 1; i >= 0; i--) {
		const auto& v = verts[i];
		tri_strip_.add_vertex({ { v.x, v.y, v.z }, { 1 } });
	}

	const auto& v = verts[SIDES - 1];
	tri_strip_.add_vertex({ { v.x, v.y, v.z }, { 1 } });
}

void
cell::draw() const
{
	tri_strip_.draw(GL_TRIANGLE_FAN);
}
