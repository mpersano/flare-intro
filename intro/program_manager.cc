#include <cstdio>

#include <limits.h>

#include "program_manager.h"

namespace {

std::string
get_shader_path(const char *basename)
{
	std::string str("data/shaders/");
	str.append(basename);
	return str;
}

}

program_manager::program_manager()
{
	struct {
		const char *vertex_shader;
		const char *frag_shader;
	} program_sources[] = {
		{ "vert-decal.glsl", "frag-decal.glsl" },
		{ "vert-wiretri.glsl", "frag-wiretri.glsl" }
	};

	for (int i = 0; i < NumPrograms; i++) {
		const auto& p = program_sources[i];

		printf("loading %s,%s\n",
			get_shader_path(p.vertex_shader).c_str(),
			get_shader_path(p.frag_shader).c_str());

		std::unique_ptr<ggl::program> ptr(new ggl::program);

		ptr->attach_vertex_shader(get_shader_path(p.vertex_shader).c_str());
		ptr->attach_fragment_shader(get_shader_path(p.frag_shader).c_str());
		ptr->link();

		programs_[i] = std::move(ptr);
	}
}

program_manager &
program_manager::get_instance()
{
	static program_manager pm;
	return pm;
}

const ggl::program *
program_manager::get(program_id id)
{
	return programs_[id].get();
}
