#include <cmath>

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"
#include "boid.h"

namespace {

const float SPEED = 90.;
const float ANG_SPEED = .7;

}

boid::boid()
: position_(frand(-50, 50), frand(150, 200), frand(-50, 50))
, direction_(glm::rotate(frand(0, 2.*M_PI), glm::vec3(0, 1, 0))*glm::rotate(frand(0, 2.*M_PI), glm::vec3(1, 0, 0)))
, ang_speed_(frand(1., 3.))
{
	init_faces();
}

void
boid::init_faces()
{
	const glm::vec3 verts[] = {
		{ 0, 0, 1 },
		{ 0, 0.942809, -0.33333 },
		{ -0.816497, -0.471405, -0.33333 },
		{ 0.816497, -0.471405, -0.33333 },
	};

	const int faces[][3] = {
		{ 0, 1, 2 },
		{ 0, 3, 1 },
		{ 0, 2, 3 },
		{ 1, 3, 2 },
	};

	for (const auto& face : faces) {
		const float l = 20;

		const glm::vec3& v0 = l*verts[face[0]].xzy();
		const glm::vec3& v1 = l*verts[face[1]].xzy();
		const glm::vec3& v2 = l*verts[face[2]].xzy();

		va_.add_vertex({ { v0.x, v0.y, v0.z }, { frand(0, .125), frand(0, .125), frand(0, .5) } });
		va_.add_vertex({ { v1.x, v1.y, v1.z }, { frand(0, .125), frand(0, .125), frand(0, .5) } });
		va_.add_vertex({ { v2.x, v2.y, v2.z }, { frand(0, .125), frand(0, .125), frand(0, .5) } });
	}
}

void
boid::update(float dt, const glm::vec3& target)
{
	const float MIN_ANGLE = .1f, MAX_ANGLE = 1.f;

	// I HAVE NO IDEA WHAT I'M DOING

	// project target to this my coord system
	glm::vec3 inv_target = (glm::inverse(glm::translate(position_)*direction_)*glm::vec4(target, 1)).xyz();

	// rotate around y
	glm::vec2 v0 = glm::normalize(inv_target.xz());

	float a0 = acosf(v0.y);

	if (a0 < MIN_ANGLE)
		a0 = MIN_ANGLE;
	else if (a0 > MAX_ANGLE)
		a0 = MAX_ANGLE;

	glm::mat4 m0 = glm::rotate(dt*ANG_SPEED*a0*(v0.x < 0 ? -1 : 1), glm::vec3(0, -1, 0));

	// rotate around x
	glm::vec2 v1 = glm::normalize(inv_target.yz());

	float a1 = acosf(v1.y);

	if (a1 < MIN_ANGLE)
		a1 = MIN_ANGLE;
	else if (a1 > MAX_ANGLE)
		a1 = MAX_ANGLE;

	glm::mat4 m1 = glm::rotate(dt*ANG_SPEED*a1*(v1.x < 0 ? -1 : 1), glm::vec3(1, 0, 0));

	glm::mat4 m2 = glm::rotate(ang_speed_*dt, glm::vec3(0, 0, 1));

	direction_ *= m0*m1*m2;

	glm::vec3 dir = glm::normalize(direction_[2].xyz());
	position_ -= dt*SPEED*dir;
}

void
boid::draw() const
{
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(glm::translate(position_)*direction_));

	va_.draw(GL_TRIANGLES);

	glPopMatrix();
}
