#pragma once
#include "gui/console.hpp"
#include "assets/asset_manager.hpp"
#include "gui/inspector.hpp"
#include "gui/console_commands.hpp"
#include "components/relations.hpp"
#include "components/transform2D.hpp"
#include "components/sprite.hpp"
#include "components/flags.hpp"
#define RESOURCES_DIR resources

namespace conf{
    typedef ge::Console<ge::hi_command, ge::echo_command> Console_t;
    typedef ge::Inspector<Alive,Dead,ge::comp::Transform2D,ge::comp::Sprite> Inspector_t;
    typedef ge::AssetManager<ge::MultiTexture> AssetManager_t;
}
