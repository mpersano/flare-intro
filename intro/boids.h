#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "fx.h"

struct boid
{
public:
	boid();

	void draw() const;
	void update(float dt, const glm::vec3& target);

private:
	glm::vec3 position_;
	glm::mat4 direction_;
};

class boids : public fx
{
public:
	boids();

	void draw(float t);

private:
	static const int NUM_BOIDS = 32;
	boid boids_[NUM_BOIDS];
	float prev_t_;
};
