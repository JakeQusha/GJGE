#pragma once

#include "gui/console.hpp"
#include "gui/inspector.hpp"
#include "components/relations.hpp"
#include "components/transform2D.hpp"
#include "components/sprite.hpp"
#include "gui/console_commands.hpp"
#include "components/flags.hpp"

typedef ge::Console<ge::hi_command, ge::echo_command> Console_t;
typedef ge::Inspector<Alive, Dead, ge::comp::Transform2D, ge::comp::Sprite> Inspector_t;