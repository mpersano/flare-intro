#include <cstdio>

#include <GL/glew.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"
#include "util.h"
#include "program_manager.h"
#include "quadtree.h"
#include "boids.h"

namespace {

const float Z_NEAR = .1;
const float Z_FAR = 1000.;
const float FOV = 45;

const float SPEED = 50.;
const float ANG_SPEED = .5;

float
sign(float x)
{
	if (x < 0)
		return -1;
	else if (x == 0)
		return 0;
	else
		return 1;
}

void
draw_target()
{
	const float l = 10;

	glBegin(GL_LINES);
	glVertex3f(-l, 0, 0); glVertex3f(l, 0, 0);
	glVertex3f(0, -l, 0); glVertex3f(0, l, 0);
	glVertex3f(0, 0, -l); glVertex3f(0, 0, l);
	glEnd();
}

};

boid::boid()
: position_(frand(-50, 50), frand(-50, 50), frand(-50, 50))
{ }

void
boid::update(float dt, const glm::vec3& target)
{
	// I HAVE NO IDEA WHAT I'M DOING

	// project target to this my coord system
	glm::vec3 inv_target = (glm::inverse(glm::translate(position_)*direction_)*glm::vec4(target, 1)).xyz();

	// rotate around y
	glm::vec2 v0 = glm::normalize(inv_target.xz());
	glm::mat4 m0 = glm::rotate(dt*ANG_SPEED*acosf(v0.y)*(v0.x < 0 ? -1 : 1), glm::vec3(0, -1, 0));

	// rotate around x
	glm::vec2 v1 = glm::normalize(inv_target.yz());
	glm::mat4 m1 = glm::rotate(dt*ANG_SPEED*acosf(v1.y)*(v1.x < 0 ? -1 : 1), glm::vec3(1, 0, 0));

	direction_ *= m0*m1;

	glm::vec3 dir = glm::normalize(direction_[2].xyz());
	position_ -= dt*SPEED*dir;
}

void
boid::draw() const
{
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(glm::translate(position_)*direction_));

	glColor4f(1, 1, 1, 1);
	draw_target();

	glPopMatrix();
}

boids::boids()
: prev_t_(0)
{
	const float TERRAIN_SIZE = 1000;
	const int COLUMN_ROWS = 40;
	const float COLUMN_RADIUS = .95f*.5f*TERRAIN_SIZE/COLUMN_ROWS;

	const float min = -.5f*TERRAIN_SIZE - COLUMN_RADIUS;
	const float max = .5f*TERRAIN_SIZE + COLUMN_RADIUS;

	terrain_root_ = make_quadtree_node(glm::vec2(min, min), glm::vec2(max, max));

	float y = -.5f*TERRAIN_SIZE + COLUMN_RADIUS;

	for (int i = 0; i < COLUMN_ROWS; i++) {
		float x = -.5f*TERRAIN_SIZE + COLUMN_RADIUS;

		for (int j = 0; j < COLUMN_ROWS; j++) {
			const glm::vec2 center(x, y + ((j & 1) ? COLUMN_RADIUS : 0));
			terrain_root_->insert(frob(center, frand(5, 100), COLUMN_RADIUS));
			x += 2*COLUMN_RADIUS;
		}

		y += 2*COLUMN_RADIUS;
	}
}

void
boids::draw(float t)
{
	// update boids

	glm::vec3 target(200.*sinf(1.2f*t), 0, 0);

	const float dt = t - prev_t_;

	for (auto& boid : boids_)
		boid.update(dt, glm::vec3(target));

	const float aspect = static_cast<float>(g_viewport_width)/g_viewport_height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, aspect, Z_NEAR, Z_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

#if 0
	glTranslatef(0, 0, -300);
	// glRotatef(t*30., 1, 0, 0);

	glPushMatrix();
	glTranslatef(target.x, target.y, target.z);
	
	glColor4f(1, 0, 0, 1);
	draw_target();

	glPopMatrix();

	for (const auto& boid : boids_)
		boid.draw();

	prev_t_ = t;
#else
	const frustum f(FOV, aspect, Z_NEAR, Z_FAR);
	extern int leaves_drawn;

	glm::vec3 eye = (glm::rotate(.1f*t, glm::vec3(0, 1, 0))*glm::vec4(0, 120 /* 500 + 400*sinf(.5f*t) */, -100, 1)).xyz();

	glm::mat4 mv = glm::lookAt(eye, glm::vec3(0, 20, 100), glm::vec3(0, 1, 0));

	leaves_drawn = 0;

	const ggl::program *prog = get_program(PROG_FAKEWIRE);
	
	prog->use();
	prog->set_uniform_f("color", 1, 1, 1);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	terrain_root_->draw(mv, f);

	// printf("%d\n", leaves_drawn);
#endif
}
