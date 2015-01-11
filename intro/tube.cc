#include <GL/glew.h>

#include <glm/vec2.hpp>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ggl/vertex_array.h>

#include "program_manager.h"
#include "spectrum.h"
#include "util.h"
#include "tube.h"
#include "bezier.h"
#include "particle_texture.h"

namespace {

const float Z_NEAR = .1;
const float Z_FAR = 1000.;
const float FOV = 45;

const float BALL_RADIUS = 50.;

const int NUM_PARTICLES = 1600;

static const int NUM_SPECTRUM_BANDS = 30;
float spectrum_bars[NUM_SPECTRUM_BANDS];

void
update_spectrum_bars(const spectrum &s)
{
	const int num_samples = spectrum::WINDOW_SIZE/8;
	const int samples_per_band = num_samples/NUM_SPECTRUM_BANDS;

	for (int i = 0; i < NUM_SPECTRUM_BANDS; i++) {
		float w = 0;

		for (int j = 0; j < samples_per_band; j++)
			w += s.spectrum_window[i*samples_per_band + j];

		w /= samples_per_band;
		w = sqrtf(w);

		spectrum_bars[i] = w;
	}
}

sg::node_ptr
make_portal_cell(int viewport_width, int viewport_height, float spectrum_offset)
{
	struct cell_node : public sg::leaf_node
	{
		cell_node(int viewport_width, int viewport_height, float spectrum_offset)
		: viewport_width_(viewport_width)
		, viewport_height_(viewport_height)
		, spectrum_offset_(spectrum_offset)
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

			float value = (1. - s)*spectrum_bars[index] + s*spectrum_bars[(index + 1)%NUM_SPECTRUM_BANDS];

			float state = 10.*value;
			if (state > 1.)
				state = 1;

			float c = .25f + .75f*state;

			static ggl::vertex_array<ggl::vertex_texcoord<2, 1>> va(3*NUM_VERTS);

			va.clear();

			for (int i = 0; i < NUM_VERTS; i++) {
				const auto& v0 = verts_[i];
				const auto& v1 = verts_[(i + 1)%NUM_VERTS];

				glm::vec2 p0(0, 0);
				glm::vec2 p1 = state*v0.second + (1.f - state)*v0.first;
				glm::vec2 p2 = state*v1.second + (1.f - state)*v1.first;

				// world coords --> screen coords

				glm::mat4 projection;
				glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projection));

				glm::mat4 modelview;
				glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelview));

				glm::mat4 projection_modelview = projection*modelview;

				glm::vec2 viewport(viewport_width_, viewport_height_);

				glm::vec4 p0_transformed = projection_modelview*glm::vec4(p0, 0, 1);

				// normalized screen coords (-1, -1), (1, 1) to actual screen coords
				glm::vec2 p0_screen = (p0_transformed.xy()/p0_transformed.w)*.5f*viewport;

				glm::vec4 p1_transformed = projection_modelview*glm::vec4(p1, 0, 1);
				glm::vec2 p1_screen = (p1_transformed.xy()/p1_transformed.w)*.5f*viewport;

				glm::vec4 p2_transformed = projection_modelview*glm::vec4(p2, 0, 1);
				glm::vec2 p2_screen = (p2_transformed.xy()/p2_transformed.w)*.5f*viewport;

				// interpolate distance from verts to opposite edge

				{
				glm::vec2 v0 = p2_screen - p1_screen;
				glm::vec2 v1 = p2_screen - p0_screen;
				glm::vec2 v2 = p1_screen - p0_screen;

				float area = fabs(v1.x*v2.y - v1.y*v2.x);

				va.add_vertex({{ p0.x, p0.y }, { area/glm::length(v0) }});
				va.add_vertex({{ p1.x, p1.y }, { 0 }});
				va.add_vertex({{ p2.x, p2.y }, { 0 }});
				}
			}

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

		int viewport_width_, viewport_height_;
		bounding_box bbox_;
		float spectrum_offset_;
		const ggl::program *program_;
	};

	return std::unique_ptr<sg::node>(new cell_node(viewport_width, viewport_height, spectrum_offset));
}

sg::node_ptr
make_portal_node(int viewport_width, int viewport_height, const float *values, int num_sections, float spectrum_offset)
{
	sg::group_node *root = new sg::group_node;

	float a = 0;
	const float da = 2.*M_PI/num_sections;

	for (int i = 0; i < num_sections; i++) {
		glm::mat4 m = glm::rotate(a, glm::vec3(0, 0, 1))*glm::translate(glm::vec3(0, 1., 0));
		sg::transform_node *transform = new sg::transform_node(m, make_portal_cell(viewport_width, viewport_height, spectrum_offset + i));
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

struct particle
{
	particle(const std::vector<bezier>& path);

	void draw(const glm::mat4& mv, const frustum& f, float t) const;

	float speed_;
	float pos_offset_;
	glm::vec3 offset_;
	const std::vector<bezier>& path_;
	ggl::vertex_array<ggl::vertex_texcoord<2, 2>> va_;
};

particle::particle(const std::vector<bezier>& path)
: speed_(frand(.15, .3)*((irand() & 1) ? -1 : 1))
, pos_offset_(frand(0, 5000.))
, offset_(frand(-.5, .5), frand(-.5, .5), frand(-.5, .5))
, path_(path)
{
	const float r = 1.;

	va_.add_vertex({{ -r, -r }, { 0, 0 }});
	va_.add_vertex({{ r, -r }, { 1, 0 }});
	va_.add_vertex({{ -r, r }, { 0, 1 }});
	va_.add_vertex({{ r, r }, { 1, 1 }});
}

void
particle::draw(const glm::mat4& mv, const frustum& f, float t) const
{
	float pos = speed_*t + pos_offset_;

	const auto& seg = path_[static_cast<int>(pos)%path_.size()];
	float u = fmod(pos, 1.);

	const glm::vec3 p = seg.eval(u) + offset_;

	glm::mat4 m = mv*glm::translate(p);
	m[0] = glm::vec4(1, 0, 0, 0);
	m[1] = glm::vec4(0, 1, 0, 0);
	m[2] = glm::vec4(0, 0, 1, 0);
	glLoadMatrixf(glm::value_ptr(m));

	va_.draw(GL_TRIANGLE_STRIP);
}

tube::tube(int width, int height)
: fx(width, height)
{
	rand_init();

	gen_path(glm::vec3(.25*BALL_RADIUS, BALL_RADIUS, 0), glm::vec3(0, BALL_RADIUS, 0), 6);
	gen_path(glm::vec3(0, BALL_RADIUS, 0), glm::vec3(-.25*BALL_RADIUS, -BALL_RADIUS, 0), 6);

	for (int i = 0; i < NUM_PARTICLES; i++)
		particles_.push_back(std::unique_ptr<particle>(new particle(segs_)));

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
				make_portal_node(width_, height_, spectrum_bars, 5, 6*i));
		group->add_child(sg::node_ptr(transform));
		u += du;
	}

	scene_.add_child(sg::node_ptr(group));
}

void
tube::draw(float t) const
{
	if (player_) {
		spectrum s(*player_, static_cast<unsigned>(t*1000.));

		update_spectrum_bars(s);

		glDisable(GL_TEXTURE_2D);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width_, 0, height_, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(20, 20, 0);
		s.draw_bars(30, 400, 100);
	}

	const float aspect = static_cast<float>(width_)/height_;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, aspect, Z_NEAR, Z_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glDisable(GL_TEXTURE_2D);

#if 0
	glm::mat4 mv = glm::translate(glm::vec3(0, 0, -200))*glm::rotate(.08f*t, glm::vec3(0, 1, 0));
#else
	const float SPEED = .125;
#if 1
	const auto& seg = segs_[static_cast<int>(SPEED*t)%segs_.size()];
	float u = fmod(SPEED*t, 1.);
#else
	const float q = 1000;
	const auto& seg = segs_[static_cast<int>(SPEED*q)%segs_.size()];
	float u = fmod(SPEED*q, 1.);
#endif
	glm::mat4 mv = glm::inverse(
		matrix_on_seg(seg, u)*
		glm::translate(glm::vec3(0, .5, 0)));
#endif

	const frustum f(FOV, aspect, Z_NEAR, Z_FAR);

	sg::leaf_draw_count = 0;
	scene_.draw(mv, f, t);
	// printf("%d\n", sg::leaf_draw_count);

	glEnable(GL_TEXTURE_2D);
	particle_texture_.bind();

	particle_program_.use();

	for (auto& p : particles_)
		p->draw(mv, f, t);

	glUseProgram(0);
}
