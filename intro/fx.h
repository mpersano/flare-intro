#pragma once

class fx
{
public:
	virtual ~fx() { }

	virtual void draw(float t) = 0;
};
