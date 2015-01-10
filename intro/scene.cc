#include <GL/glew.h>

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "scene.h"

namespace sg {

int leaf_draw_count;

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

		++leaf_draw_count;
	}
}

mesh_node::mesh_node(mesh_ptr mesh)
: mesh_(mesh)
{ }

const bounding_box&
mesh_node::get_bounding_box() const
{
	return mesh_->bbox;
}

debug_mesh_node::debug_mesh_node(mesh_ptr mesh, const glm::vec4& color)
: mesh_node(mesh)
, color_(color)
{ }

void
debug_mesh_node::render(float) const
{
	glColor4fv(glm::value_ptr(color_));

	for (auto& p : mesh_->polys) {
		glBegin(GL_LINE_LOOP);

		for (auto& i : p.indices)
			glVertex3fv(glm::value_ptr(mesh_->verts[i]));

		glEnd();
	}
}

} // sg
