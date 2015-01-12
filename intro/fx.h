#pragma once

struct ogg_player;

class fx
{
public:
	virtual ~fx() { }

	virtual void draw(float t) const = 0;
};
