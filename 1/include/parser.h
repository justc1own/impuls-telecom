#pragma once

#include <optional>
#include <string>
#include "dungeon_types.h"

std::optional<std::string> parseDungeonInput(const std::string& inputPath, DungeonInput& data);
