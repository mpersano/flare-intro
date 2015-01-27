#pragma once

#include <ggl/texture.h>

#include "fx.h"

class text : public fx
{
public:
	text();

	void draw(float t) const;

private:
	class label {
	public:
		label(const char *source, int x, int y);

		void draw(float t) const;

	private:
		ggl::texture texture_;
		ggl::vertex_array<ggl::vertex_texcoord<GLint, 2, GLfloat, 2>> va_;
	};

	std::vector<std::unique_ptr<label>> labels_;
};
