#pragma once

#include <map>
#include <string>
#include <memory>

#include <ggl/program.h>

class
program_manager
{
	using dict_type = std::map<std::pair<std::string, std::string>, std::unique_ptr<ggl::program>>;
	using dict_value_type = typename dict_type::value_type;

public:
	program_manager(const program_manager&) = delete;
	program_manager& operator=(const program_manager&) = delete;

	static program_manager& get_instance();

	const ggl::program *get(const std::string& vert_shader, const std::string& frag_shader);

private:
	program_manager();

	dict_type program_dict_;
};
