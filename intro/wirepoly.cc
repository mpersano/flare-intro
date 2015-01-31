#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ggl/vertex_array.h>

#include "common.h"
#include "wirepoly.h"

void
wirepoly::draw() const
{
	glm::vec2 screen_verts[verts_.size()];

	glm::mat4 projection, modelview;
	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projection));
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelview));
	glm::mat4 projection_modelview = projection*modelview;

	glm::vec2 viewport(g_viewport_width, g_viewport_height);

	for (int i = 0; i < verts_.size(); i++) {
		// obj -> world
		glm::vec4 p = projection_modelview*glm::vec4(verts_[i], 1);

		// world -> screen
		screen_verts[i] = (p.xy()/p.w)*.5f*viewport;
	}

	glm::vec3 center;
	for (const auto& v : verts_)
		center += v;
	center *= 1.f/verts_.size();

	const glm::vec4 p = projection_modelview*glm::vec4(center, 1);
	const glm::vec2 screen_center = (p.xy()/p.w)*.5f*viewport;

	static ggl::vertex_array<ggl::vertex_texcoord<GLfloat, 3, GLfloat, 1>> va;

	va.clear();

	for (int i = 0; i < verts_.size(); i++) {
		int j = (i + 1)%verts_.size();

		const glm::vec3& p0 = verts_[i];
		const glm::vec3& p1 = verts_[j];

		const glm::vec2& q0 = screen_verts[i];
		const glm::vec2& q1 = screen_verts[j];

		const glm::vec2 v0 = q1 - q0;
		const glm::vec2 v1 = q1 - screen_center;
		const glm::vec2 v2 = q0 - screen_center;

		float area = fabs(v1.x*v2.y - v1.y*v2.x);

		va.add_vertex({{ center.x, center.y, center.z }, { area/glm::length(v0) }});
		va.add_vertex({{ p0.x, p0.y, p0.z }, { 0 }});
		va.add_vertex({{ p1.x, p1.y, p1.z }, { 0 }});
	}

	va.draw(GL_TRIANGLES);
}
