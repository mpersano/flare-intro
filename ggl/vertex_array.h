#pragma once

#include <GL/glew.h>

#include <vector>

namespace ggl {

template <int N>
struct vertex_flat
{
	GLfloat pos[N];
};

template <int N, int M>
struct vertex_texcoord
{
	GLfloat pos[N];
	GLfloat texcoord[M];
};

namespace detail {

// RAII <3 <3 <3

template <typename VertexType>
struct client_state;

template <int N>
struct client_state<vertex_flat<N>>
{
	client_state(const vertex_flat<N> *verts)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(N, GL_FLOAT, sizeof *verts, verts->pos);
	}

	~client_state()
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
};

template <int N, int M>
struct client_state<vertex_texcoord<N, M>>
{
	client_state(const vertex_texcoord<N, M> *verts)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(N, GL_FLOAT, sizeof *verts, verts->pos);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(M, GL_FLOAT, sizeof *verts, verts->texcoord);
	}

	~client_state()
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
};

} // detail

template <class Vertex>
class vertex_array
{
public:
	vertex_array() = default;

	vertex_array(int capacity)
	{ verts_.reserve(capacity); }

  	vertex_array(const vertex_array&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;

	void clear()
	{ verts_.clear(); }

	void add_vertex(const Vertex& v)
	{ verts_.push_back(v); }

	void draw(GLenum mode) const
	{
		detail::client_state<Vertex> state(&verts_[0]);
		glDrawArrays(mode, 0, verts_.size());
	}

	size_t get_num_verts() const
	{ return verts_.size(); }

private:
	std::vector<Vertex> verts_;
};

} // ggl
