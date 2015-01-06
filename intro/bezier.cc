#include "bezier.h"

glm::vec3
bezier::eval(float u) const
{
	const float w0 = (1 - u)*(1 - u);
	const float w1 = 2*u*(1 - u);
	const float w2 = u*u;

	return p0*w0 + p1*w1 + p2*w2;
}

glm::vec3
bezier::eval_dir(float u) const
{
	const float w0 = 2*(u - 1);
	const float w1 = 2*(1 - 2*u);
	const float w2 = 2*u;

	return p0*w0 + p1*w1 + p2*w2;
}
