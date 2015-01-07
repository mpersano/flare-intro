#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"
#include "tube.h"
#include "bezier.h"

namespace {

const float Z_NEAR = 1.;
const float Z_FAR = 1000.;
const float FOV = 45;

const float BALL_RADIUS = 50.;
const float TUBE_RADIUS = 3.;

mesh_ptr
make_portal_mesh()
{
	mesh_ptr m(new mesh);

	const int num_segs = 5;

	// verts

	const float da = 2.*M_PI/num_segs;
	float a = 0;

	for (int i = 0; i < num_segs; i++) {
		const float x = TUBE_RADIUS*sinf(a);
		const float y = TUBE_RADIUS*cosf(a);

		m->verts.push_back(glm::vec3(x, y, 0));

		a += da;
	}

	// poly

	mesh::poly poly;

	for (int i = 0; i < num_segs; i++)
		poly.indices.push_back(i);

	m->polys.push_back(poly);

	return m;
}

glm::mat4
matrix_on_seg(const bezier& seg, float u)
{
	const glm::vec3 pos = seg.eval(u);

	glm::vec3 up = glm::normalize(pos);
	glm::vec3 dir = -glm::normalize(seg.eval_dir(u));

	float delta = -glm::dot(up, dir)/glm::dot(dir, dir);
	up += delta*dir;
	up = glm::normalize(up);

	glm::vec3 left = glm::cross(up, dir);

	glm::mat4 rot = glm::mat4(
			glm::vec4(left, 0),
			glm::vec4(up, 0),
			glm::vec4(dir, 0),
			glm::vec4(0, 0, 0, 1));

	glm::mat4 trans = glm::translate(glm::mat4(1), pos);

	return trans*rot;
}

} // namespace

tube::tube(int width, int height)
: fx(width, height)
{
	gen_path(glm::vec3(.25*BALL_RADIUS, BALL_RADIUS, 0), glm::vec3(0, BALL_RADIUS, 0), 6);
	gen_path(glm::vec3(0, BALL_RADIUS, 0), glm::vec3(-.25*BALL_RADIUS, -BALL_RADIUS, 0), 6);
}

void
tube::gen_path(const glm::vec3& p0, const glm::vec3& p1, int depth)
{
	if (depth == 0) {
		gen_segment(p0, p1);
	} else {
		const float fudge = 20.;

		float a = -fudge*depth;
		float b = fudge*depth;

		const float r = BALL_RADIUS*(1. + .1*depth);

		glm::vec3 pm = .5f*(p0 + p1) + glm::vec3(frand(a, b), frand(a, b), frand(a, b));
		pm *= r/glm::length(pm);

		gen_path(p0, pm, depth - 1);
		gen_path(pm, p1, depth - 1);
	}
}

void
tube::gen_segment(const glm::vec3& p0, const glm::vec3& p1)
{
	glm::vec3 pm;

	if (segs_.empty()) {
		pm = .5f*(p0 + p1);
	} else {
		auto& prev = segs_.back();
		float l = .5*glm::length(p1 - p0);
		pm = p0 + glm::normalize(p0 - prev.p1)*l;
	}

	bezier seg(p0, pm, p1);

	segs_.push_back(seg);

	const int num_segs = 6;
	const float du = 1./num_segs;
	float u = 0;

	for (int i = 0; i < num_segs; i++) {
		sg::transform_node *node = new sg::transform_node(matrix_on_seg(seg, u));
		node->add_child(std::unique_ptr<sg::node>(new sg::debug_mesh_node(make_portal_mesh())));

		scene_.add_child(std::unique_ptr<sg::node>(node));

		u += du;
	}
}

void
tube::draw(float t) const
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, static_cast<float>(width_)/height_, Z_NEAR, Z_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#if 0
	glm::mat4 mv = glm::translate(glm::mat4(1), glm::vec3(0, 0, -200))*glm::rotate(glm::mat4(1), 30.f*t, glm::vec3(0, 1, 0));
#else
	const float SPEED = .5;

	int cur_seg = 0;

	const auto& seg = segs_[static_cast<int>(SPEED*t)%segs_.size()];
	float u = fmod(SPEED*t, 1.);

	glm::mat4 mv = glm::inverse(matrix_on_seg(seg, u));
#endif

	scene_.draw(mv);
}
