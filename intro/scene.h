#pragma once

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "frustum.h"

namespace sg {

class node
{
public:
	virtual ~node() { }

	virtual void draw(const glm::mat4& mv, const frustum& f, float t) const = 0;
	virtual const bounding_box& get_bounding_box() const = 0;
};

using node_ptr = std::unique_ptr<node>;

class group_node : public node
{
public:
	void draw(const glm::mat4& mv, const frustum& f, float t) const;

	void add_child(node_ptr child);

	const bounding_box& get_bounding_box() const
	{ return bbox_; }

protected:
	std::vector<node_ptr> children_;
	bounding_box bbox_;
};

struct transform_node : public node
{
public:
	transform_node(const glm::mat4& mat, node_ptr child);

	void draw(const glm::mat4& mv, const frustum& f, float t) const;

	const bounding_box& get_bounding_box() const
	{ return bbox_; }

private:
	glm::mat4 mat_;
	node_ptr child_;
	bounding_box bbox_;
};

class leaf_node : public node
{
public:
	void draw(const glm::mat4& mv, const frustum& f, float t) const;

	virtual void render(float t) const = 0;
};

extern int leaf_draw_count;

} // sg
