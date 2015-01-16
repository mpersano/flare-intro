#pragma once

#include <map>
#include <string>
#include <memory>

#include <ggl/program.h>

class
program_manager
{
	typedef std::map<std::pair<std::string, std::string>, std::unique_ptr<ggl::program>> dict_type;
	typedef typename dict_type::value_type dict_value_type;

public:
	program_manager(const program_manager&) = delete;
	program_manager& operator=(const program_manager&) = delete;

	static program_manager& get_instance();

	const ggl::program *get(const std::string& vert_shader, const std::string& frag_shader);

private:
	program_manager();

	dict_type program_dict_;
};
