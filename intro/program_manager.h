#pragma once

#include <memory>

#include <ggl/program.h>

class
program_manager
{
public:
	enum program_id
	{
		White,
		Decal,
		PortalWireframe,
		TextBlur,
		NumPrograms,
	};

	program_manager(const program_manager&) = delete;
	program_manager& operator=(const program_manager&) = delete;

	static program_manager& get_instance();

	const ggl::program *get(program_id id);

private:
	program_manager();

	std::unique_ptr<ggl::program> programs_[NumPrograms];
};

inline const ggl::program *
get_program(program_manager::program_id id)
{
	return program_manager::get_instance().get(id);
}
