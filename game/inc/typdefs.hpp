#pragma once
#include "console.hpp"
#include "inspector.hpp"
#include "console_commands.hpp"
#include "componenets/flags.hpp"
#include "components/relations.hpp"
#include "components/transform2D.hpp"
#include "components/sprite.hpp"

typedef ge::Console<ge::hi_command, ge::echo_command> Console_t;
typedef ge::Inspector<Alive,Dead,ge::comp::Transform2D,ge::comp::Sprite> Inspector_t;