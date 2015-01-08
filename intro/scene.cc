#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "scene.h"

namespace sg {

int leaf_draw_count;

void
group_node::draw(const glm::mat4& mv, const frustum& f, float t) const
{
	for (auto& p : children_)
		p->draw(mv, f, t);
}

void
group_node::add_child(std::unique_ptr<node> child)
{
	children_.push_back(std::move(child));
}

transform_node::transform_node(const glm::mat4& mat)
: mat_(mat)
{ }

void
transform_node::draw(const glm::mat4& mv, const frustum& f, float t) const
{
	glm::mat4 m = mv*mat_;

	for (auto& p : children_)
		p->draw(m, f, t);
}

void
leaf_node::draw(const glm::mat4& mv, const frustum& f, float t) const
{
	if (f.intersects(mv, get_bounding_box(t))) {
		glLoadMatrixf(glm::value_ptr(mv));
		render(t);

		++leaf_draw_count;
	}
}

mesh_node::mesh_node(mesh_ptr mesh)
: mesh_(mesh)
{ }

bounding_box
mesh_node::get_bounding_box(float) const
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
