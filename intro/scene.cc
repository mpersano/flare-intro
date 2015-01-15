#include <GL/glew.h>

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "scene.h"

namespace sg {

void
group_node::draw(const glm::mat4& mv, const frustum& f, float t) const
{
	if (f.intersects(mv, bbox_)) {
		for (auto& p : children_)
			p->draw(mv, f, t);
	}
}

void
group_node::add_child(node_ptr child)
{
	const bounding_box& child_bbox = child->get_bounding_box();

	bbox_ += child_bbox.min;
	bbox_ += child_bbox.max;

	children_.push_back(std::move(child));
}

transform_node::transform_node(const glm::mat4& mat, node_ptr child)
: mat_(mat)
, child_(std::move(child))
{
	const bounding_box& child_bbox = child_->get_bounding_box();

	glm::mat4 inv_mat = glm::inverse(mat);
	bbox_ += (mat*glm::vec4(child_bbox.min, 1)).xyz();
	bbox_ += (mat*glm::vec4(child_bbox.max, 1)).xyz();
}

void
transform_node::draw(const glm::mat4& mv, const frustum& f, float t) const
{
	if (f.intersects(mv, bbox_))
		child_->draw(mv*mat_, f, t);
}

void
leaf_node::draw(const glm::mat4& mv, const frustum& f, float t) const
{
	if (f.intersects(mv, get_bounding_box())) {
		glLoadMatrixf(glm::value_ptr(mv));
		render(t);
	}
}

} // sg
