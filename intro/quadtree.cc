#include <cassert>

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include "quadtree.h"

namespace {

struct quadtree_leaf : quadtree_node
{
public:
	quadtree_leaf(const glm::vec2& min, const glm::vec2& max)
	: quadtree_node(min, max)
	{ }

	void draw(const glm::mat4& mv, const frustum& f) const;
	void insert(const frob& f);

private:
	std::vector<frob> frobs_;
};

struct quadtree_inner : quadtree_node
{
public:
	quadtree_inner(const glm::vec2& min, const glm::vec2& max)
	: quadtree_node(min, max)
	{ }

	void draw(const glm::mat4& mv, const frustum& f) const;
	void insert(const frob& f);

private:
	std::unique_ptr<quadtree_node> children_[4];
};

void
quadtree_leaf::draw(const glm::mat4& mv, const frustum& f) const
{
	// XXX; check frustum

	glLoadMatrixf(glm::value_ptr(mv));

	for (const auto& f : frobs_)
		f.draw();
}

void
quadtree_leaf::insert(const frob& f)
{
	assert(f.center_.x >= min_.x && f.center_.x < max_.x);
	assert(f.center_.y >= min_.y && f.center_.y < max_.y);

	frobs_.push_back(f);
}

void
quadtree_inner::draw(const glm::mat4& mv, const frustum& f) const
{
	for (const auto& child : children_) {
		if (child)
			child->draw(mv, f);
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
		} else {
			// insert into 1
			if (!children_[1])
				children_[1] = make_quadtree_node(glm::vec2(min_.x, mid.y), glm::vec2(mid.x, max_.y));

			children_[1]->insert(f);
		}
	} else {
		if (f.center_.y < mid.y) {
			// insert into 2
			if (!children_[2])
				children_[2] = make_quadtree_node(glm::vec2(mid.x, min_.y), glm::vec2(max_.x, mid.y));

			children_[2]->insert(f);
		} else {
			// insert into 3
			if (!children_[3])
				children_[3] = make_quadtree_node(mid, max_);

			children_[3]->insert(f);
		}
	}
}

} // namespace

std::unique_ptr<quadtree_node>
make_quadtree_node(const glm::vec2& min, const glm::vec2& max)
{
	const float MIN_SIZE = 10.;

	if (max.x - min.x < MIN_SIZE && max.y - min.y < MIN_SIZE)
		return std::unique_ptr<quadtree_node>(new quadtree_leaf(min, max));
	else
		return std::unique_ptr<quadtree_node>(new quadtree_inner(min, max));
}

void
frob::draw() const
{
	// FOR NOW

	draw_section(0);
	draw_section(height_);

	float a = 0;
	const float da = 2*M_PI/SIDES;

	glBegin(GL_LINES);

	for (int i = 0; i < SIDES; i++) {
		const float c = cosf(a), s = sinf(a);
		glVertex3f(center_.x + radius_*c, 0, center_.y + radius_*s);
		glVertex3f(center_.x + radius_*c, height_, center_.y + radius_*s);
		a += da;
	}

	glEnd();
}

void
frob::draw_section(float h) const
{
	float a = 0;
	const float da = 2*M_PI/SIDES;

	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < SIDES; i++) {
		const float c = cosf(a), s = sinf(a);
		glVertex3f(center_.x + radius_*c, h, center_.y + radius_*s);
		a += da;
	}

	glEnd();
}
