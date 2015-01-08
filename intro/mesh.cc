#include "mesh.h"

void
mesh::initialize_bounding_box()
{
	for (const auto& v : verts)
		bbox += v;
}
