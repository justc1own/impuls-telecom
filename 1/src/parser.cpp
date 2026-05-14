#include "parser.h"

#include <fstream>
#include <optional>
#include <sstream>
#include <string>

std::optional<ResourceType> parseResourceType(const std::string& str) {
    for (size_t i = 0; i < ResourceNames.size(); i++) {
        if (str == ResourceNames[i]) {
            return static_cast<ResourceType>(i);
        }
    }
    return std::nullopt;
}

std::optional<std::string> parseInteger(const std::string& str, int& val) {
    if (str.empty()) {
        return "invalid integer";
    }

    try {
        val = std::stoi(str);
    } catch (...) {
        return "invalid integer: " + str;
    }
    return std::nullopt;
}

std::vector<std::string> splitString(const std::string& text, char del) {
    std::vector<std::string> parts;
    std::string element;
    std::istringstream stream(text);
    while (std::getline(stream, element, del)) {
        parts.push_back(element);
    }
    return parts;
}

std::optional<std::string> parseRoomLine(const std::string& line, Room& room) {
    std::istringstream stream(line);
    std::string strId;
    std::string strNeighbors;
    std::string resources[static_cast<size_t>(ResourceType::COUNT)];

    if (!(stream >> strId)) {
        return line;
    }
    if (!(stream >> strNeighbors)) {
        return line;
    }
    for (size_t i = 0; i < static_cast<size_t>(ResourceType::COUNT); i++) {
        if (!(stream >> resources[i])) {
            return line;
        }
    }
    if (!stream.eof()) {
        std::string extra;
        if (stream >> extra) {
            return line;
        }
    }

    int id;
    if (auto err = parseInteger(strId, id)) {
        return *err;
    }
    if (id < 0) {
        return "invalid room id" + line;
    }
    room.id = id;

    if (strNeighbors.empty()) {
        return "invalid room neighbors" + line;
    }

    auto neighborIds = splitString(strNeighbors, ',');
    for (const auto& neighbor : neighborIds) {
        int neighborId;
        if (auto err = parseInteger(neighbor, neighborId)) {
            return *err;
        }
        if (neighborId < 0) {
            return "invalid neighbor id" + line;
        }
        room.neighbors.push_back(neighborId);
    }

    for (size_t i = 0; i < static_cast<size_t>(ResourceType::COUNT); i++) {
        int value;
        if (auto err = parseInteger(resources[i], value)) {
            return *err;
        }
        if (value < 0 || value > 255) {
            return "invalid resource value" + line;
        }
        room.resources[i] = value;
    }

    return std::nullopt;
}

std::optional<std::string> parseDungeonInput(const std::string& inputPath, DungeonInput& data) {
    std::ifstream inputFile(inputPath);
    if (!inputFile.is_open()) {
        return "cannot open input file";
    }

    std::string line;
    if (!std::getline(inputFile, line)) {
        return "invalid input data";
    }
    if (line.empty()) {
        return "empty input data";
    }
    int N;
    if (auto err = parseInteger(line, N)) {
        return *err;
    }
    if (N < 1 || N > 255) {
        return "invalid N";
    }
    data.N = N;
    data.rooms.reserve(N + 1);

    std::vector<bool> roomProcessed(N + 1, false);
    for (int i = 0; i <= N; i++) {
        if (!std::getline(inputFile, line)) {
            return "invalid input data";
        }
        Room room;
        if (auto err = parseRoomLine(line, room)) {
            return *err;
        }
        if (room.id > N) {
            return "invalid room id" + line;
        }
        if (roomProcessed[room.id]) {
            return "duplicate room id" + line;
        }
        roomProcessed[room.id] = true;
        data.rooms.push_back(std::move(room));
    }

    for (int i = 0; i <= N; i++) {
        if (!roomProcessed[i]) {
            return "missing room " + std::to_string(i);
        }
    }

    if (!std::getline(inputFile, line)) {
        return "invalid input data";
    }
    std::istringstream lastLine(line);
    std::string strM;
    std::string strTarget;
    if (!(lastLine >> strM >> strTarget)) {
        return "invalid last line";
    }
    int M;
    if (auto err = parseInteger(strM, M)) {
        return *err;
    }
    if (M < 2 || M > 255) {
        return "invalid M";
    }
    auto targetOpt = parseResourceType(strTarget);
    if (!targetOpt.has_value()) {
        return "invalid resource type";
    }
    data.M = M;
    data.target = targetOpt.value();

    std::string extra;
    if (lastLine >> extra) {
        return "extra data";
    }

    inputFile.close();
    return std::nullopt;
}
