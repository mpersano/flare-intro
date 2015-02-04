#pragma once

#include <GL/glew.h>

#include <vector>

namespace ggl {

template <typename VertexType, int VertexSize>
struct vertex_flat
{
	VertexType pos[VertexSize];
};

template <typename VertexType, int VertexSize, typename TexCoordType, int TexCoordSize>
struct vertex_texcoord
{
	VertexType pos[VertexSize];
	TexCoordType texcoord[TexCoordSize];
};

namespace detail {

template <typename GLType>
struct gltype_to_glenum;

template <>
struct gltype_to_glenum<GLbyte>
{
	static const GLenum type = GL_BYTE;
};

template <>
struct gltype_to_glenum<GLubyte>
{
	static const GLenum type = GL_UNSIGNED_BYTE;
};

template <>
struct gltype_to_glenum<GLshort>
{
	static const GLenum type = GL_SHORT;
};

template <>
struct gltype_to_glenum<GLushort>
{
	static const GLenum type = GL_UNSIGNED_SHORT;
};

template <>
struct gltype_to_glenum<GLint>
{
	static const GLenum type = GL_INT;
};

template <>
struct gltype_to_glenum<GLuint>
{
	static const GLenum type = GL_UNSIGNED_INT;
};

template <>
struct gltype_to_glenum<GLfloat>
{
	static const GLenum type = GL_FLOAT;
};

// RAII <3 <3 <3

template <typename Vertex>
struct client_state;

template <typename VertexType, int VertexSize>
struct client_state<vertex_flat<VertexType, VertexSize>>
{
	client_state(const vertex_flat<VertexType, VertexSize> *verts)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(VertexSize, gltype_to_glenum<VertexType>::type, sizeof *verts, verts->pos);
	}

	~client_state()
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
};

template <typename VertexType, int VertexSize, typename TexCoordType, int TexCoordSize>
struct client_state<vertex_texcoord<VertexType, VertexSize, TexCoordType, TexCoordSize>>
{
	client_state(const vertex_texcoord<VertexType, VertexSize, TexCoordType, TexCoordSize> *verts)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(VertexSize, gltype_to_glenum<VertexType>::type, sizeof *verts, verts->pos);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(TexCoordSize, gltype_to_glenum<TexCoordType>::type, sizeof *verts, verts->texcoord);
	}

	~client_state()
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
};

} // detail

// TODO: LEARN C++

template <typename VertexType>
class vertex_array_base
{
public:
	vertex_array_base() = default;

	void clear()
	{ verts_.clear(); }

	void add_vertex(const VertexType& v)
	{ verts_.push_back(v); }

	size_t get_num_verts() const
	{ return verts_.size(); }

protected:
	std::vector<VertexType> verts_;
};

template <typename IndexType, typename VertexType>
class indexed_vertex_array_base : public vertex_array_base<VertexType>
{
public:
	typedef vertex_array_base<VertexType> base_type;

	void clear()
	{
		base_type::clear();
		indices_.clear();
	}

	void add_index(const IndexType& index)
	{ indices_.push_back(index); }

	size_t get_num_indices() const
	{ return indices_.size(); }

protected:
	std::vector<IndexType> indices_;
};

template <typename VertexType>
class vertex_array : public vertex_array_base<VertexType>
{
public:
	void draw(GLenum mode) const
	{
		detail::client_state<VertexType> state(&this->verts_[0]);
		glDrawArrays(mode, 0, this->verts_.size());
	}
};

template <typename IndexType, typename VertexType>
class indexed_vertex_array : public indexed_vertex_array_base<IndexType, VertexType>
{
public:
	void draw(GLenum mode) const
	{
		detail::client_state<VertexType> state(&this->verts_[0]);
		glDrawElements(mode, this->indices_.size(), detail::gltype_to_glenum<IndexType>::type, &this->indices_[0]);
	}
};

// TODO: LEARN OPENGL

class gl_buffer
{
public:
	gl_buffer(GLenum target)
	: target_(target)
	{ glGenBuffers(1, &id_); }

	gl_buffer(const gl_buffer&) = delete;
	gl_buffer& operator=(const gl_buffer&) = delete;

	~gl_buffer()
	{ glDeleteBuffers(1, &id_); }

	void bind() const
	{ glBindBuffer(target_, id_); }

	void unbind() const
	{ glBindBuffer(target_, 0); }

	void buffer(GLsizeiptr size, const GLvoid *data, GLenum usage) const
	{
		bind();
		glBufferData(target_, size, data, usage);
		unbind();
	}

private:
	GLenum target_;
	GLuint id_;
};

template <typename VertexType>
class vbo : public vertex_array_base<VertexType>
{
public:
	vbo()
	: vertex_buffer_(GL_ARRAY_BUFFER)
	{ }

	// usage: GL_STATIC_DRAW or GL_DYNAMIC_DRAW
	void buffer(GLenum usage) const
	{
		vertex_buffer_.buffer(this->verts_.size()*sizeof(VertexType), &this->verts_[0], usage);
	}

	void draw(GLenum mode) const
	{
		vertex_buffer_.bind();

		detail::client_state<VertexType> state(0);
		glDrawArrays(mode, 0, this->verts_.size());

		vertex_buffer_.unbind();
	}

private:
	gl_buffer vertex_buffer_;
};

template <typename IndexType, typename VertexType>
class indexed_vbo : public indexed_vertex_array_base<IndexType, VertexType>
{
public:
	indexed_vbo()
	: vertex_buffer_(GL_ARRAY_BUFFER)
	, index_buffer_(GL_ELEMENT_ARRAY_BUFFER)
	{ }

	// usage: GL_STATIC_DRAW or GL_DYNAMIC_DRAW
	void buffer(GLenum usage) const
	{
		vertex_buffer_.buffer(this->verts_.size()*sizeof(VertexType), &this->verts_[0], usage);
		index_buffer_.buffer(this->indices_.size()*sizeof(IndexType), &this->indices_[0], usage);
	}

	void draw(GLenum mode) const
	{
		vertex_buffer_.bind();
		index_buffer_.bind();

		detail::client_state<VertexType> state(0);
		glDrawElements(mode, this->indices_.size(), detail::gltype_to_glenum<IndexType>::type, 0);

		index_buffer_.unbind();
		vertex_buffer_.unbind();
	}

private:
	gl_buffer vertex_buffer_, index_buffer_;
};


} // ggl
