#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "scene.h"

namespace sg {

void
group_node::draw(const glm::mat4& mv) const
{
	for (auto& p : children_)
		p->draw(mv);
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
transform_node::draw(const glm::mat4& mv) const
{
	glm::mat4 m = mv*mat_;

	for (auto& p : children_)
		p->draw(m);
}

void
leaf_node::draw(const glm::mat4& mv) const
{
	glLoadMatrixf(glm::value_ptr(mv));
	render();
}

mesh_node::mesh_node(mesh_ptr mesh)
: mesh_(mesh)
{ }

void
debug_mesh_node::render() const
{
	for (auto& p : mesh_->polys) {
		glBegin(GL_LINE_LOOP);

		for (auto& i : p.indices)
			glVertex3fv(glm::value_ptr(mesh_->verts[i]));

		glEnd();
	}
}

} // sg
