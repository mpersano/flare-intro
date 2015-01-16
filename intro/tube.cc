#include <glm/vec2.hpp>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"
#include "program_manager.h"
#include "util.h"
#include "tube.h"
#include "bezier.h"
#include "particle_texture.h"

namespace {

const float Z_NEAR = .1;
const float Z_FAR = 1000.;
const float FOV = 45;

const float BALL_RADIUS = 50.;

const int NUM_PARTICLES = 900;

sg::node_ptr
make_portal_cell(float spectrum_offset)
{
	struct cell_node : public sg::leaf_node
	{
		cell_node(float spectrum_offset)
		: spectrum_offset_(spectrum_offset)
		, program_(program_manager::get_instance().get("data/shaders/vert-wiretri.glsl", "data/shaders/frag-wiretri.glsl"))
		{
			const float da = 2.*M_PI/NUM_SIDES;
			float a = 0;

			const float r = .5;

			for (int i = 0; i < NUM_SIDES; i++) {
				const glm::vec2 p0(r*sinf(a), r*cosf(a));
				const glm::vec2 p1(r*sinf(a + da), r*cosf(a + da));

				const glm::vec2 p00 = p0;
				const glm::vec2 p01 = .5f*(p0 + p1);

				const glm::vec2 p10 = .9f*p00;
				const glm::vec2 p11 = 3.f*p01;

				bbox_ += glm::vec3(p00, 0);
				bbox_ += glm::vec3(p01, 0);
				bbox_ += glm::vec3(p10, 0);
				bbox_ += glm::vec3(p11, 0);

				verts_[i*2] = std::make_pair(p00, p10);
				verts_[i*2 + 1] = std::make_pair(p01, p11);

				a += da;
			}
		}

		void render(float t) const
		{
			float offset = spectrum_offset_ + 5.*t;

			int index = static_cast<int>(offset)%NUM_SPECTRUM_BANDS;
			float s = offset - static_cast<int>(offset);

			float value = (1. - s)*g_spectrum_bars[index] + s*g_spectrum_bars[(index + 1)%NUM_SPECTRUM_BANDS];

			float state = 10.*value;
			if (state > 1.)
				state = 1;

			static glm::vec2 obj_verts[NUM_VERTS], screen_verts[NUM_VERTS], screen_center;

			glm::mat4 projection, modelview;
			glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projection));
			glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelview));

			glm::mat4 projection_modelview = projection*modelview;

			glm::vec2 viewport(g_viewport_width, g_viewport_height);

			for (int i = 0; i < NUM_VERTS; i++) {
				obj_verts[i] = state*verts_[i].second + (1.f - state)*verts_[i].first;

				// obj -> world
				glm::vec4 p = projection_modelview*glm::vec4(obj_verts[i], 0, 1);

				// world -> screen
				screen_verts[i] = (p.xy()/p.w)*.5f*viewport;
			}

			{
			glm::vec4 p = projection_modelview*glm::vec4(0, 0, 0, 1);
			screen_center = (p.xy()/p.w)*.5f*viewport;
			}

			static ggl::vertex_array<ggl::vertex_texcoord<GLfloat, 2, GLfloat, 1>> va(3*NUM_VERTS);

			va.clear();

			for (int i = 0; i < NUM_VERTS; i++) {
				int j = (i + 1)%NUM_VERTS;

				const glm::vec2& p0 = obj_verts[i];
				const glm::vec2& p1 = obj_verts[j];

				const glm::vec2& q0 = screen_verts[i];
				const glm::vec2& q1 = screen_verts[j];

				const glm::vec2 v0 = q1 - q0;
				const glm::vec2 v1 = q1 - screen_center;
				const glm::vec2 v2 = q0 - screen_center;

				float area = fabs(v1.x*v2.y - v1.y*v2.x);

				va.add_vertex({{ 0, 0 }, { area/glm::length(v0) }});
				va.add_vertex({{ p0.x, p0.y }, { 0 }});
				va.add_vertex({{ p1.x, p1.y }, { 0 }});
			}

			float c = .1f + .9f*state*state;

			program_->use();
			program_->set_uniform_f("thick", 1.);
			program_->set_uniform_f("color", c, c, c);
			va.draw(GL_TRIANGLES);
		}

		const bounding_box&
		get_bounding_box() const
		{ return bbox_; }

		enum { NUM_SIDES = 5, NUM_VERTS = 2*NUM_SIDES };
		std::pair<glm::vec2, glm::vec2> verts_[NUM_VERTS];

		bounding_box bbox_;
		float spectrum_offset_;
		const ggl::program *program_;
	};

	return std::unique_ptr<sg::node>(new cell_node(spectrum_offset));
}

sg::node_ptr
make_portal_node(int num_sections, float spectrum_offset)
{
	sg::group_node *root = new sg::group_node;

	float a = 0;
	const float da = 2.*M_PI/num_sections;

	for (int i = 0; i < num_sections; i++) {
		glm::mat4 m = glm::rotate(a, glm::vec3(0, 0, 1))*glm::translate(glm::vec3(0, 1., 0));
		sg::transform_node *transform = new sg::transform_node(m, make_portal_cell(spectrum_offset + i));
		root->add_child(sg::node_ptr(transform));

		a += da;
	}

	return sg::node_ptr(root);
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

particle::particle(const std::vector<bezier> *path)
: speed_(frand(.15, .3)*((irand() & 1) ? -1 : 1))
, pos_offset_(frand(0, 5000.))
, offset_(frand(-.5, .5), frand(-.5, .5), frand(-.5, .5))
, path_(path)
{ }

void
particle::draw(vertex_array& va, const glm::vec3& up, const glm::vec3& right, float t) const
{
	float pos = speed_*t + pos_offset_;

	const auto& seg = (*path_)[static_cast<int>(pos)%path_->size()];
	float u = fmod(pos, 1.);

	const glm::vec3 p = seg.eval(u) + offset_;

	const float r = 1.;

	const glm::vec3 p0 = p - r*up - r*right;
	const glm::vec3 p1 = p - r*up + r*right;
	const glm::vec3 p2 = p + r*up + r*right;
	const glm::vec3 p3 = p + r*up - r*right;

	va.add_vertex({{ p0.x, p0.y, p0.z }, { 0, 0 }});
	va.add_vertex({{ p1.x, p1.y, p1.z }, { 1, 0 }});
	va.add_vertex({{ p2.x, p2.y, p2.z }, { 1, 1 }});

	va.add_vertex({{ p2.x, p2.y, p2.z }, { 1, 1 }});
	va.add_vertex({{ p3.x, p3.y, p3.z }, { 0, 1 }});
	va.add_vertex({{ p0.x, p0.y, p0.z }, { 0, 0 }});
}

camera_path::camera_path(const bezier& path, const glm::vec3& target, float ttl)
: path_(path)
, up_(glm::normalize(glm::cross(path_.p0 - path_.p1, path_.p2 - path_.p1)))
, target_(target)
, ttl_(ttl)
{ }

glm::mat4
camera_path::get_mv(float t) const
{
	const float u = fmod(t, ttl_)/ttl_;
	const glm::vec3 pos = path_.eval(u);
	return glm::lookAt(pos, target_, up_);
}

tube::tube()
{
	rand_init();

	gen_path(glm::vec3(.25*BALL_RADIUS, BALL_RADIUS, 0), glm::vec3(0, BALL_RADIUS, 0), 6);
	gen_path(glm::vec3(0, BALL_RADIUS, 0), glm::vec3(-.25*BALL_RADIUS, -BALL_RADIUS, 0), 6);

	for (int i = 0; i < NUM_PARTICLES; i++)
		particles_.push_back(particle(&segs_));

	camera_paths_.push_back(
		{ { { -44.94, 4.98, 27.64 }, { -45.50, 5.01, 25.81 }, { -45.88, 6.36, 27.11 } }, { -45.24, 6.99, 30.49 }, 9.f });
	camera_paths_.push_back(
		{ { { -38.63, 24.88, -35.15 }, { -40.22, 26.10, -35.22 }, { -38.80, 26.81, -34.01 } }, { -35.71, 25.34, -33.29 }, 4.f });
	camera_paths_.push_back(
		{ { { -45.66, 47.60, -10.26 }, { -45.81, 46.88, -8.44 }, { -47.25, 48.06, -9.05 } }, { -47.75, 49.74, -12.08 }, 7.f });
	camera_paths_.push_back(
		{ { { -24.02, 29.10, -37.42 }, { -25.27, 27.87, -37.10 }, { -24.61, 29.31, -37.92 } }, { -22.40, 31.87, -38.82 }, 4.5f });
	camera_paths_.push_back(
		{ { { -19.06, 17.36, 47.81 }, { -20.29, 18.17, 46.85 }, { -19.29, 17.24, 47.96 } }, { -16.95, 15.57, 49.95 }, 4.5f });

	particle_texture_.load(*render_particle_texture());

	particle_texture_.set_wrap_s(GL_CLAMP);
	particle_texture_.set_wrap_t(GL_CLAMP);

	particle_texture_.set_mag_filter(GL_LINEAR);
	particle_texture_.set_min_filter(GL_LINEAR);

	particle_texture_.set_env_mode(GL_MODULATE);

	particle_program_.attach_vertex_shader("data/shaders/vert-particle.glsl");
	particle_program_.attach_fragment_shader("data/shaders/frag-particle.glsl");
	particle_program_.link();
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
	// bezier seg

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

	// portals

	sg::group_node *group = new sg::group_node;

	const int num_segs = 6;
	const float du = 1./num_segs;
	float u = 0;

	for (int i = 0; i < num_segs; i++) {
		sg::transform_node *transform =
			new sg::transform_node(
				matrix_on_seg(seg, u),
				make_portal_node(5, 6*i));
		group->add_child(sg::node_ptr(transform));
		u += du;
	}

	scene_.add_child(sg::node_ptr(group));
}

void
tube::draw(float t) const
{
#if 1
	float dt = t;

	auto it = camera_paths_.begin();

	while (it != camera_paths_.end() && dt > it->ttl_) {
		dt -= it->ttl_;
		++it;
	}

	glm::mat4 mv;

	if (it != camera_paths_.end()) {
		mv = it->get_mv(dt);

		draw(mv, false, dt);
	} else {
		const float SPEED = .125;
		const auto& seg = segs_[static_cast<int>(SPEED*t)%segs_.size()];
		float u = fmod(SPEED*dt, 1.);

		mv = glm::inverse(matrix_on_seg(seg, u)*glm::translate(glm::vec3(0, .25, 0)));

		draw(mv, true, t);
	}
#else
	draw(glm::translate(glm::vec3(0, 0, -200)), false, 0);
#endif
}

void
tube::draw(const glm::mat4& mv, bool show_particles, float t) const
{
	const float aspect = static_cast<float>(g_viewport_width)/g_viewport_height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, aspect, Z_NEAR, Z_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	const frustum f(FOV, aspect, Z_NEAR, Z_FAR);

	// scene

	scene_.draw(mv, f, t);

	// particles

	if (show_particles) {
		particle_texture_.bind();

		particle_program_.use();

		glLoadMatrixf(glm::value_ptr(mv));

		glm::vec3 up(mv[0][0], mv[1][0], mv[2][0]);
		glm::vec3 right(mv[0][1], mv[1][1], mv[2][1]);

		static particle::vertex_array va(6*NUM_PARTICLES);

		va.clear();

		for (auto& p : particles_)
			p.draw(va, up, right, t);

		va.draw(GL_TRIANGLES);
	}
}
