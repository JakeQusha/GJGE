#pragma once
#include "console.hpp"
#include "inspector.hpp"
#include "console_commands.hpp"
#include "componenets/flags.hpp"
#include "components/relations.hpp"

typedef ge::Console<ge::hi_command, ge::echo_command> Console_t;
typedef ge::Inspector<Alive,Dead> Inspector_t;