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
	void insert(const frob& f);

	std::vector<frob> frobs_;
};

struct quadtree_inner : quadtree_node
{
	quadtree_inner(const glm::vec2& min, const glm::vec2& max)
	: quadtree_node(min, max)
	{ }

	void draw(const glm::mat4& mv, const frustum& f) const;
	void insert(const frob& f);

	std::unique_ptr<quadtree_node> children_[4];
};

void
quadtree_leaf::draw(const glm::mat4& mv, const frustum& f) const
{
	if (f.intersects(mv, box_)) {
		++leaves_drawn;

		glLoadMatrixf(glm::value_ptr(mv));
		// box_.draw();

		glColor4f(1, 1, 1, 1);
		for (const auto& f : frobs_)
			f.draw();
	}
}

void
quadtree_leaf::insert(const frob& f)
{
	assert(f.center_.x >= min_.x && f.center_.x < max_.x);
	assert(f.center_.y >= min_.y && f.center_.y < max_.y);

	frobs_.push_back(f);

	box_ += glm::vec3(f.center_.x - f.radius_, 0, f.center_.y - f.radius_);
	box_ += glm::vec3(f.center_.x + f.radius_, f.height_, f.center_.y + f.radius_);
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
quadtree_inner::insert(const frob& f)
{
	assert(f.center_.x >= min_.x && f.center_.x < max_.x);
	assert(f.center_.y >= min_.y && f.center_.y < max_.y);

	glm::vec2 mid = .5f*(min_ + max_);

	if (f.center_.x < mid.x) {
		if (f.center_.y < mid.y) {
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
		if (f.center_.y < mid.y) {
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

frob::frob(const glm::vec2& center, float height, float radius)
: center_(center), height_(height), radius_(radius)
{
	glm::vec3 verts[2*SIDES];

	float a = 0;
	const float da = 2*M_PI/SIDES;

	for (int i = 0; i < SIDES; i++) {
		const float x = center_.x + radius_*cosf(a);
		const float z = center_.y + radius_*sinf(a);

		verts[i] = glm::vec3(x, 0, z);
		verts[i + SIDES] = glm::vec3(x, height_, z);

		a += da;
	}

	// sides

	for (int i = 0; i < SIDES; i++) {
		const glm::vec3& v0 = verts[i];
		const glm::vec3& v1 = verts[i + SIDES];
		const glm::vec3& v2 = verts[(i + 1)%SIDES + SIDES];
		const glm::vec3& v3 = verts[(i + 1)%SIDES];

		const glm::vec3 vm = .25f*(v0 + v1 + v2 + v3);

		auto& va = tri_strips_[i];
		va.add_vertex({ { vm.x, vm.y, vm.z }, { 0 } });
		va.add_vertex({ { v0.x, v0.y, v0.z }, { 1 } });
		va.add_vertex({ { v1.x, v1.y, v1.z }, { 1 } });
		va.add_vertex({ { v2.x, v2.y, v2.z }, { 1 } });
		va.add_vertex({ { v3.x, v3.y, v3.z }, { 1 } });
		va.add_vertex({ { v0.x, v0.y, v0.z }, { 1 } });
	}

	// top

	auto& va = tri_strips_[SIDES];

	va.add_vertex({ { center_.x, height_, center_.y }, { 0 } });

	for (int i = 2*SIDES - 1; i >= SIDES; i--) {
		const auto& v = verts[i];
		va.add_vertex({ { v.x, v.y, v.z }, { 1 } });
	}

	const auto& v = verts[2*SIDES - 1];
	va.add_vertex({ { v.x, v.y, v.z }, { 1 } });
}

void
frob::draw() const
{
	// XXX: should use glMultiDrawArrays really

	for (const auto& va : tri_strips_)
		va.draw(GL_TRIANGLE_FAN);
}
