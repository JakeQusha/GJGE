#pragma once

#include "scene.hpp"
#include "gui/console.hpp"
#include "gui/inspector.hpp"
#include "components/relations.hpp"
#include "components/transform2D.hpp"
#include "components/sprite.hpp"
#include "gui/console_commands.hpp"
#include "components/flags.hpp"
#include "components/collision2D.hpp"
using Console_t = ge::Console<ge::hi_command, ge::echo_command>;
using Inspector_t = ge::Inspector<Alive, Dead, ge::comp::Transform2D, ge::comp::Sprite, ge::comp::AABBCollider,ge::comp::PersistentEntity>;