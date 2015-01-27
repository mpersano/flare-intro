#pragma once

#include <ggl/texture.h>

#include <vector>

#include "fx.h"

class text : public fx
{
public:
	text();

	void draw(float t) const;

private:
	typedef std::vector<std::pair<float, float>> twitch_vector;

	class label {
	public:
		label(const char *source, int x, int y, float start_t, float duration, const twitch_vector& twitches);

		void draw(float t) const;

	private:
		float start_t_, duration_;
		twitch_vector twitches_;
		ggl::texture texture_;
		ggl::vertex_array<ggl::vertex_texcoord<GLint, 2, GLfloat, 2>> va_;
	};

	std::vector<std::unique_ptr<label>> labels_;
};
