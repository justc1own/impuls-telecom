#pragma once

#include <ostream>
#include <string>
#include "dungeon_types.h"

void writeIncorrectData(std::ostream& out, const std::string& errorMessage);
void writeGo(std::ostream& out, int roomId);
void writeCollect(std::ostream& out, ResourceType resource);
void writeState(std::ostream& out, Room& room);
void writeResult(std::ostream& out, int points);
