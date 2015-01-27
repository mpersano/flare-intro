#pragma once

#include <memory>

#include <ggl/program.h>

enum program_id
{
	PROG_WHITE,
	PROG_DECAL,
	PROG_PORTALWIREFRAME,
	PROG_TEXTBLUR,
	NUM_PROGRAMS,
};

const ggl::program *
get_program(program_id id);
