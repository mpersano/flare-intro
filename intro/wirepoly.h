#pragma once

#include <glm/vec3.hpp>

struct wirepoly
{
public:
	wirepoly() { }

	void add_vertex(const glm::vec3& v)
	{ verts_.push_back(v); }

	void draw() const;

private:
	std::vector<glm::vec3> verts_;
};
