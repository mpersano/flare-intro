#include <GL/glew.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "spectrum.h"
#include "util.h"
#include "tube.h"
#include "bezier.h"

int draw_count;

namespace {

const float Z_NEAR = .1;
const float Z_FAR = 1000.;
const float FOV = 45;

const float BALL_RADIUS = 50.;

const int NUM_PARTICLES = 1500;

mesh_ptr
make_portal_mesh(float radius)
{
	mesh_ptr m = std::make_shared<mesh>();

	const int num_segs = 5;

	// verts

	const float da = 2.*M_PI/num_segs;
	float a = 0;

	for (int i = 0; i < num_segs; i++) {
		const float x = radius*sinf(a);
		const float y = radius*cosf(a);

		m->verts.push_back(glm::vec3(x, y, 0));

		a += da;
	}

	// poly

	mesh::poly poly;

	for (int i = 0; i < num_segs; i++)
		poly.indices.push_back(i);

	m->polys.push_back(poly);
	m->initialize_bounding_box();

	return m;
}

std::unique_ptr<sg::node>
make_portal_node()
{
	sg::group_node *root = new sg::group_node;

	const int num_sections = 5;

	float a = 0;
	const float da = 360./num_sections; // angles not radians$@#!!!

	for (int i = 0; i < num_sections; i++) {
		glm::mat4 m = glm::rotate(a, glm::vec3(0, 0, 1))*glm::translate(glm::vec3(0, 1., 0));
		sg::transform_node *transform = new sg::transform_node(m);
		transform->add_child(std::unique_ptr<sg::node>(new sg::debug_mesh_node(make_portal_mesh(.5), glm::vec4(1, 1, 1, 1))));

		root->add_child(std::unique_ptr<sg::node>(transform));

		a += da;
	}

	return std::unique_ptr<sg::node>(root);
}

glm::mat4
matrix_on_seg(const bezier& seg, float u)
{
	const glm::vec3 pos = seg.eval(u);

	glm::vec3 up = glm::normalize(pos);
	glm::vec3 dir = -glm::normalize(seg.eval_dir(u));

	float delta = -glm::dot(up, dir)/glm::dot(dir, dir);
	up = glm::normalize(up + delta*dir);

	glm::vec3 left = glm::cross(up, dir);

	glm::mat4 rot = glm::mat4(
				glm::vec4(left, 0),
				glm::vec4(up, 0),
				glm::vec4(dir, 0),
				glm::vec4(0, 0, 0, 1));

	glm::mat4 trans = glm::translate(pos);

	return trans*rot;
}

} // namespace

tube::tube(int width, int height)
: fx(width, height)
{
	rand_init();

	gen_path(glm::vec3(.25*BALL_RADIUS, BALL_RADIUS, 0), glm::vec3(0, BALL_RADIUS, 0), 6);
	gen_path(glm::vec3(0, BALL_RADIUS, 0), glm::vec3(-.25*BALL_RADIUS, -BALL_RADIUS, 0), 6);

	struct runner_node : public sg::node
	{
		runner_node(const std::vector<bezier>& path)
		: speed_(frand(.15, .3))
		, pos_offset_(frand(0, 5000.))
		, ang_speed_(frand(30, 100))
		, xy_offset_(frand(-.5, .5), frand(-.5, .5))
		, path_(path)
		, child_(new sg::debug_mesh_node(make_portal_mesh(.15), glm::vec4(1, 0, 0, 1)))
		{ }

		void draw(const glm::mat4& mv, const frustum& f, float t) const
		{
			float pos = speed_*t + pos_offset_;
			float ang = ang_speed_*t;

			const auto& seg = path_[static_cast<int>(pos)%path_.size()];
			float u = fmod(pos, 1.);

			glm::mat4 m = matrix_on_seg(seg, u)*glm::rotate(ang, glm::vec3(0, 0, 1))*glm::translate(glm::vec3(xy_offset_, 0));

			child_->draw(mv*m, f, t);
		}

		float speed_;
		float pos_offset_;
		float ang_speed_;
		glm::vec2 xy_offset_;
		const std::vector<bezier>& path_;
		std::unique_ptr<sg::node> child_;
	};

	for (int i = 0; i < NUM_PARTICLES; i++)
		scene_.add_child(std::unique_ptr<sg::node>(new runner_node(segs_)));
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
		sg::transform_node *transform = new sg::transform_node(matrix_on_seg(seg, u));
		transform->add_child(make_portal_node());
		scene_.add_child(std::unique_ptr<sg::node>(transform));
		u += du;
	}

	struct seg_node : public sg::leaf_node
	{
		seg_node(const bezier& seg)
		{
			const int num_segs = 12;

			float u = 0;
			const float du = 1./num_segs;

			for (int i = 0; i <= num_segs; i++) {
				glm::vec3 p = seg.eval(u);

				verts_.push_back(p);
				bbox_ += p;

				u += du;
			}
		}

		void render(float) const
		{
			using glm::value_ptr;

			glColor4f(1, 1, 1, 1);

			glBegin(GL_LINES);

			for (int i = 0; i < verts_.size() - 1; i++) {
				glVertex3fv(value_ptr(verts_[i]));
				glVertex3fv(value_ptr(verts_[i + 1]));
			}

			glEnd();
		}

		bounding_box
		get_bounding_box(float) const
		{ return bbox_; }

		std::vector<glm::vec3> verts_;
		bounding_box bbox_;
	};

	// scene_.add_child(std::unique_ptr<sg::node>(new seg_node(seg)));
}

void
tube::draw(float t) const
{
	static const float aspect = static_cast<float>(width_)/height_;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, aspect, Z_NEAR, Z_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#if 0
	glm::mat4 mv = glm::translate(glm::vec3(0, 0, -200))*glm::rotate(30.f*t, glm::vec3(0, 1, 0));
#else
	const float SPEED = .5;

	const auto& seg = segs_[static_cast<int>(SPEED*t)%segs_.size()];
	float u = fmod(SPEED*t, 1.);

	glm::mat4 mv = glm::inverse(matrix_on_seg(seg, u));
#endif

	sg::leaf_draw_count = 0;
	scene_.draw(mv, frustum(FOV, aspect, Z_NEAR, Z_FAR), t);
	printf("%d\n", sg::leaf_draw_count);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width_, 0, height_, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(20, 20, 0);
	spectrum(*player_, static_cast<unsigned>(t*1000.)).draw_bars(32, 400, 100);
}
