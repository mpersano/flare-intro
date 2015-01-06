#include <GL/glew.h>

#include <glm/ext.hpp>

#include "util.h"
#include "tube.h"
#include "bezier.h"

namespace {

const float Z_NEAR = 1.;
const float Z_FAR = 1000.;
const float FOV = 45;

const float BALL_RADIUS = 50.;
const float TUBE_RADIUS = 1.5;

};

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
		glm::vec3 pm;

		if (segs_.empty()) {
			pm = .5f*(p0 + p1);
		} else {
			auto& prev = segs_.back();
			float l = .5*glm::length(p1 - p0);
			pm = p0 + glm::normalize(p0 - prev.p1)*l;
		}

		segs_.push_back(bezier(p0, pm, p1));
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
tube::draw(float t) const
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, static_cast<float>(width_)/height_, Z_NEAR, Z_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#if 0
	glTranslatef(0, 0, -200);
	glRotatef(30.*t, 0, 1, 0);
#else
	{
	const float SPEED = .5;

	int cur_seg = 0;

	const auto& seg = segs_[static_cast<int>(SPEED*t)%segs_.size()];
	float u = fmod(SPEED*t, 1.);

	const glm::vec3 pos = seg.eval(u);
	glm::vec3 dir = glm::normalize(seg.eval_dir(u));

#if 0
	glm::vec3 up = glm::normalize(pos);

	float delta = -glm::dot(up, dir)/glm::dot(dir, dir);
	up += delta*dir;
	up = glm::normalize(up);
#endif

	glm::mat4 mv = glm::lookAt(pos, pos + dir, glm::normalize(pos));

	glLoadMatrixf(glm::value_ptr(mv));
	}
#endif

	for (auto& seg : segs_) {
#if 1
		glColor3f(1, 0, 0);

		glBegin(GL_LINES);

		glVertex3f(seg.p0.x, seg.p0.y, seg.p0.z);
		glVertex3f(seg.p1.x, seg.p1.y, seg.p1.z);

		glVertex3f(seg.p1.x, seg.p1.y, seg.p1.z);
		glVertex3f(seg.p2.x, seg.p2.y, seg.p2.z);

		glEnd();
#endif
		const int num_segs = 16;

#if 1
		glColor3f(1, 1, 1);

		glBegin(GL_LINES);

		const float du = 1./num_segs;

		float u = 0;

		for (int i = 0; i < num_segs; i++) {
			glm::vec3 p0 = seg.eval(u);
			glm::vec3 p1 = seg.eval(u + du);

			glVertex3f(p0.x, p0.y, p0.z);
			glVertex3f(p1.x, p1.y, p1.z);

			u += du;
		}

		glEnd();
#endif

		{
		float u = 0;
		const float du = 1./num_segs;

		for (int i = 0; i < num_segs; i++) {
			const glm::vec3 pos = seg.eval(u);

			glm::vec3 up = glm::normalize(pos);
			glm::vec3 dir = glm::normalize(seg.eval_dir(u));

			float delta = -glm::dot(up, dir)/glm::dot(dir, dir);
			up += delta*dir;
			up = glm::normalize(up);

			glm::vec3 left = glm::cross(up, dir);

			const int num_segs = 5;
			const float da = 2.*M_PI/num_segs;

			glBegin(GL_LINE_LOOP);

			float a = 0;
#if 1
			for (int j = 0; j < num_segs; j++) {
				float s = TUBE_RADIUS*sinf(a);
				float c = TUBE_RADIUS*cosf(a);

				glm::vec3 p = pos + s*up + c*left;

				glVertex3f(p.x, p.y, p.z);

				a += da;
			}
#else
			const glm::vec3 p0 = pos + TUBE_RADIUS*up + TUBE_RADIUS*left;
			const glm::vec3 p1 = pos + TUBE_RADIUS*up - TUBE_RADIUS*left;
			const glm::vec3 p2 = pos - TUBE_RADIUS*up - TUBE_RADIUS*left;
			const glm::vec3 p3 = pos - TUBE_RADIUS*up + TUBE_RADIUS*left;

			glVertex3f(p0.x, p0.y, p0.z);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p3.x, p3.y, p3.z);
#endif

			glEnd();

			u += du;
		}
		}
	}
}
