#pragma once

#include <array>
#include <string>
#include <vector>

enum class ResourceType {
    IRON = 0,
    GOLD = 1,
    GEMS = 2,
    EXP = 3,
    COUNT = 4
};

inline const std::array<std::string, static_cast<size_t>(ResourceType::COUNT)> ResourceNames = {
    "iron",
    "gold",
    "gems",
    "exp"
};

inline const std::array<int, static_cast<size_t>(ResourceType::COUNT)> ResourceValue = {
    7, 11, 23, 1
};

struct Room {
    int id = -1;
    std::vector<int> neighbors;
    std::array<int, static_cast<size_t>(ResourceType::COUNT)> resources{};
    bool visited = false;
};

struct DungeonInput {
    int N = 0;
    int M = 0;
    ResourceType target = ResourceType::IRON;
    std::vector<Room> rooms;
};

inline std::array<int, static_cast<size_t>(ResourceType::COUNT)> collected{};
