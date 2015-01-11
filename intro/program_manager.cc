#include "program_manager.h"

program_manager::program_manager()
{ }

program_manager &
program_manager::get_instance()
{
	static program_manager pm;
	return pm;
}

const ggl::program *
program_manager::get(const std::string& vert_shader, const std::string& frag_shader)
{
	auto key = std::make_pair(vert_shader, frag_shader);

	auto it = program_dict_.find(key);

	if (it == program_dict_.end()) {
		printf("loading %s,%s\n", vert_shader.c_str(), frag_shader.c_str());

		std::unique_ptr<ggl::program> program(new ggl::program);

		program->attach_vertex_shader(vert_shader.c_str());
		program->attach_fragment_shader(frag_shader.c_str());
		program->link();

		it = program_dict_.insert(it, dict_value_type(key, std::move(program)));
	}

	return it->second.get();
}
