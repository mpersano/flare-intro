#pragma once

#include <memory>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "fx.h"

class quadtree_node;

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
	void init_terrain();

	static const int NUM_BOIDS = 128;
	boid boids_[NUM_BOIDS];
	std::unique_ptr<quadtree_node> terrain_root_;
	float prev_t_;
};
