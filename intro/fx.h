#pragma once

struct ogg_player;

class fx
{
public:
	fx(int width, int height)
	: width_(width), height_(height), player_(nullptr)
	{ }

	void set_ogg_player(const ogg_player *player)
	{ player_ = player; }

	virtual void draw(float t) const = 0;

protected:
	int width_, height_;
	const ogg_player *player_;
};
