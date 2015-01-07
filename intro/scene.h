#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "mesh.h"

namespace sg {

class node
{
public:
	virtual ~node() { }

	virtual void draw(const glm::mat4& mv) const = 0;
};

class group_node : public node
{
public:
	void draw(const glm::mat4& mv) const;

	void add_child(std::unique_ptr<node> child);

protected:
	std::vector<std::unique_ptr<node>> children_;
};

struct transform_node : public group_node
{
public:
	transform_node(const glm::mat4& mat);

	void draw(const glm::mat4& mv) const;

private:
	glm::mat4 mat_;
};

class leaf_node : public node
{
public:
	void draw(const glm::mat4& mv) const;

	virtual void render() const = 0;
};

class mesh_node : public leaf_node
{
public:
	mesh_node(mesh_ptr mesh);

protected:
	mesh_ptr mesh_;
};

class debug_mesh_node : public mesh_node
{
public:
	using mesh_node::mesh_node;

	void render() const;
};

} // sg
