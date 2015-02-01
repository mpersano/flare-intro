#include <cmath>

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"
#include "boid.h"

namespace {

const float SPEED = 90.;
const float ANG_SPEED = .75;

}

boid::boid()
: position_(frand(-50, 50), frand(150, 200), frand(-50, 50))
{
	init_faces();
}

void
boid::init_faces()
{
	const float l = 10;

	const glm::vec3 verts[] = {
		{ -l, -l, -l },
		{ -l, l, -l },
		{ l, l, -l },
		{ l, -l, -l },
		{ -l, -l, l },
		{ -l, l, l },
		{ l, l, l },
		{ l, -l, l },
	};

	const int faces[NUM_FACES][4] = {
		{ 0, 1, 2, 3 },
		{ 6, 5, 4, 7 },
		{ 3, 2, 6, 7 },
		{ 1, 0, 4, 5 },
		{ 0, 3, 7, 4 },
		{ 1, 5, 6, 2 },
	};

	for (int i = 0; i < 6; i++) {
		const auto& face = faces[i];
		const glm::vec3& v0 = verts[face[0]];
		const glm::vec3& v1 = verts[face[1]];
		const glm::vec3& v2 = verts[face[2]];
		const glm::vec3& v3 = verts[face[3]];
		const glm::vec3 vm = .25f*(v0 + v1 + v2 + v3);

		auto& va = faces_[i];
		va.add_vertex({ { vm.x, vm.y, vm.z }, { 0 } });
		va.add_vertex({ { v0.x, v0.y, v0.z }, { 1 } });
		va.add_vertex({ { v1.x, v1.y, v1.z }, { 1 } });
		va.add_vertex({ { v2.x, v2.y, v2.z }, { 1 } });
		va.add_vertex({ { v3.x, v3.y, v3.z }, { 1 } });
		va.add_vertex({ { v0.x, v0.y, v0.z }, { 1 } });
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

	direction_ *= m0*m1;

	glm::vec3 dir = glm::normalize(direction_[2].xyz());
	position_ -= dt*SPEED*dir;
}

void
boid::draw() const
{
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(glm::translate(position_)*direction_));

	for (auto& va : faces_)
		va.draw(GL_TRIANGLE_FAN);

	glPopMatrix();
}
