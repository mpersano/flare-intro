#pragma once

class fx
{
public:
	fx(int width, int height)
	: width_(width), height_(height)
	{ }

	virtual void draw(float t) const = 0;

protected:
	int width_, height_;
};
