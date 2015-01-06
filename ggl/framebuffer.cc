#include "texture.h"
#include "framebuffer.h"

namespace ggl {

framebuffer::framebuffer(int width, int height)
: texture_(width, height)
{
	glGenFramebuffers(1, &id_);

	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_.get_gl_id(), 0);
	unbind();
}

framebuffer::~framebuffer()
{
	glDeleteFramebuffers(1, &id_);
}

void
framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

void
framebuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
