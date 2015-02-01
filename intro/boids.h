#pragma once

#include <memory>

#include "fx.h"
#include "boid.h"

class quadtree_node;

class boids : public fx
{
public:
	boids();

	void draw(float t);

private:
	void init_terrain();

	static const int NUM_BOIDS = 256;
	boid boids_[NUM_BOIDS];
	std::unique_ptr<quadtree_node> terrain_root_;
	float prev_t_;
};
