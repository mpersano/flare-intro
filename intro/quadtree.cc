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
	float a = 0;
	const float da = 2*M_PI/SIDES;

	for (int i = 0; i < SIDES; i++) {
		const float x = center_.x + radius_*cosf(a);
		const float z = center_.y + radius_*sinf(a);

		verts_[i] = glm::vec3(x, 0, z);
		verts_[i + SIDES] = glm::vec3(x, height_, z);

		a += da;
	}
}

void
frob::draw() const
{
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < SIDES; i++)
		glVertex3fv(glm::value_ptr(verts_[i]));
	glEnd();

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < SIDES; i++)
		glVertex3fv(glm::value_ptr(verts_[i + SIDES]));
	glEnd();

	glBegin(GL_LINES);
	for (int i = 0; i < SIDES; i++) {
		glVertex3fv(glm::value_ptr(verts_[i]));
		glVertex3fv(glm::value_ptr(verts_[i + SIDES]));
	}
	glEnd();
}
