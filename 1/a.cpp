#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

enum class ResourceType {
    IRON = 0,
    GOLD = 1,
    GEMS = 2,
    EXP = 3,
    COUNT = 4
};

static const std::array<std::string, static_cast<size_t>(ResourceType::COUNT)> ResourceNames = {
    "iron",
    "gold",
    "gems",
    "exp"
};

static const std::array<int, static_cast<size_t>(ResourceType::COUNT)> ResourceValue = {
    7, 11, 23, 1
};

struct Room {
    int id = -1;
    std::vector<int> neighbors;
    std::array<int, static_cast<size_t>(ResourceType::COUNT)> resources{};
    bool visited = false;
    //bool visible = false; - бесполезно для реализации, но можно интегрировать для другого алгоритма
    // построенного на том, что бот видит ресурсы в соседних комнатах, а не только в текущей или 
    // обратный путь ищется иначе 
};

struct DungeonInput {
    int N = 0;
    int M = 0;
    ResourceType target = ResourceType::IRON;
    std::vector<Room> rooms;
};

std::array<int, static_cast<size_t>(ResourceType::COUNT)> collected{};

//================================================= parsing

std::optional<ResourceType> parseResourceType(const std::string& str) {
    for (size_t i = 0; i < ResourceNames.size(); i++) {
        if (str == ResourceNames[i]) {
            return static_cast<ResourceType>(i);
        }
    }
    return std::nullopt;
}

bool parseInteger(const std::string& str, int& val) {
    if(str.empty()) {
        return false;
    }

    try {
        val = std::stoi(str);
    } catch (...) {
        return false;
    }
    return true;
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

bool parseRoomLine(const std::string& line, Room& room) {
    std::istringstream stream(line);
    std::string strId;
    std::string strNeighbors;
    std::string resources[static_cast<size_t>(ResourceType::COUNT)];

    if (!(stream >> strId)) {
        return false;
    }
    if (!(stream >> strNeighbors)) {
        return false;
    }
    for (size_t i = 0; i < static_cast<size_t>(ResourceType::COUNT); i++) {
        if (!(stream >> resources[i])) {
            return false;
        }
    }
    if (!stream.eof()) {
        std::string extra;
        if (stream >> extra) {
            return false;
        }
    }

    int id;
    if (!parseInteger(strId, id)) {
        return false;
    }
    if(id < 0) {
        return false;
    }
    room.id = id;

    if (strNeighbors.empty()) {
        return false;
    }

    auto neighborIds = splitString(strNeighbors, ',');
    for (const auto& neighbor : neighborIds) {
        int neighborId;
        if (!parseInteger(neighbor, neighborId)) {
            return false;
        }
        if (neighborId < 0) {
            return false;
        }
        room.neighbors.push_back(neighborId);
    }

    for (size_t i = 0; i < static_cast<size_t>(ResourceType::COUNT); i++) {
        int value;
        if (!parseInteger(resources[i], value)) {
            return false;
        }
        if (value < 0 || value > 255) {
            return false;
        }
        room.resources[i] = value;
    }

    return true;
}

bool parseDungeonInput(const std::string& inputPath, DungeonInput& data) {
    std::ifstream inputFile(inputPath);
    if(!inputFile.is_open()) {
        return false;
    }

    std::string line;
    if (!std::getline(inputFile, line)) {
        return false;
    }
    if (line.empty()) {
        return false;
    }
    int N;
    if (!parseInteger(line, N)) {
        return false;
    }
    if (N < 1 || N > 255) {
        return false;
    }
    data.N = N;
    data.rooms.reserve(N + 1);

    std::vector<bool> roomProcessed(N + 1, false);
    for (int i = 0; i <= N; i++) {
        if (!std::getline(inputFile, line)) {
            return false;
        }
        Room room;
        if (!parseRoomLine(line, room)) {
            return false;
        }
        if (room.id > N) {
            return false;
        }
        if (roomProcessed[room.id]) {
            return false;
        }
        roomProcessed[room.id] = true;
        data.rooms.push_back(std::move(room));
    }

    for (int i = 0; i <= N; i++) {
        if (!roomProcessed[i]) {
            return false;
        }
    }

    // ====================================================== M, resource type
    if (!std::getline(inputFile, line)) {
        return false;
    }
    std::istringstream lastLine(line);
    std::string strM;
    std::string strTarget;
    if (!(lastLine >> strM >> strTarget)) {
        return false;
    }
    int M;
    if (!parseInteger(strM, M)) {
        return false;
    }
    if (M < 2 || M > 255) {
        return false;
    }
    auto targetOpt = parseResourceType(strTarget);
    if (!targetOpt.has_value()) {
        return false;
    }
    data.M = M;
    data.target = targetOpt.value();

    std::string extra;
    if (lastLine >> extra) {
        return false;
    }

    inputFile.close();
    return true;
}

//================================================= output

void writeIncorrectData(std::ostream& out) {
    if(!out) {
        return;
    }
    out  << "incorrect data\n";
}

void writeGo(std::ostream& out, int roomId) {
    if(!out) {
        return;
    }
    out << "go " << roomId << "\n";
}

void writeCollect(std::ostream& out, ResourceType resource) {
    if(!out) {
        return;
    }
    out << "collect " << ResourceNames[static_cast<size_t>(resource)] << "\n";
}

const int extract = -1;

void writeState(std::ostream& out, Room& room) {
    if(!out) {
        return;
    }
    out << "state " << room.id << " ";
    for(auto i: room.resources) {
        if(i != -1) out << i << " ";
        else out << "_ ";
    }
    out << "\n";
}

void writeResult(std::ostream& out, int points) {
    if(!out) {
        return;
    }

    out << "result " 
    << collected[0] << " "
    << collected[1] << " "
    << collected[2] << " "
    << collected[3] << " "
    << points << "\n";
}

//================================================= algo

const int maxVal = 1e9;

std::tuple<int,int,int> collect(const Room& room, int targetResource = -1) {
    int bestType = -1;
    int bestValue = 0;
    int bestCount = 0;
    for (int i = 0; i < static_cast<int>(ResourceType::COUNT); i++) {
        if (room.resources[i] <= 0) continue;
        int unit = ResourceValue[i] * (i == targetResource ? 2 : 1);
        int total = room.resources[i] * unit;
        if (total > bestValue) {
            bestValue = total;
            bestType = i;
            bestCount = room.resources[i];
        }
    }
    return {bestType, bestCount, bestValue};
}

// std::pair<int, int> collect(const Room& room, int targetResource = -1) {
//     int optimal = 0;
//     int bestType = -1;
//     auto getResourceValue = [&](int type) {
//         if(type == targetResource) {
//             return ResourceValue[type] * 2;
//         }
//         return ResourceValue[type];
//     };

//     for(int i = 0; i < static_cast<int>(ResourceType::COUNT); i++) {
//         int tmpVal = room.resources[i] * getResourceValue(i);
//         if(tmpVal > optimal) {
//             optimal = tmpVal;
//             bestType = i;
//         }
//     }

//     if(optimal > 0 && bestType != -1) {
//         return {optimal, bestType};
//     } else {
//         return {0, -1};
//     }
// }

void moveBot(DungeonInput& data, int& currentRoom, int balance, int& points, std::ostream& out) {

    if(currentRoom != 0) {
        writeGo(out, currentRoom);
        writeState(out, data.rooms[currentRoom]);
    }

    data.rooms[currentRoom].visited = true;
    //data.rooms[currentRoom].visible = true;

    // ============= points calculation
    auto [bestType, count, totalValue] = collect(data.rooms[currentRoom], static_cast<int>(data.target));
    
    if(bestType != -1) {
        points += totalValue;
        data.rooms[currentRoom].resources[bestType] = -1;
        collected[bestType] += count;
        writeCollect(out, static_cast<ResourceType>(bestType));
        writeState(out, data.rooms[currentRoom]);
    }
    //===================

    int mn = maxVal;
    for(auto& neighbor: data.rooms[currentRoom].neighbors) {
        if(!data.rooms[neighbor].visited) {
            mn = std::min(mn, data.rooms[neighbor].id);
            // visited marking ??
        }
    }
    if(mn == maxVal) {
        return;
    }
    if(balance == 0) {
        return;
    }
    moveBot(data, mn, balance-1, points, out);
    if(currentRoom != 0) {
        writeGo(out, currentRoom);
        writeState(out, data.rooms[currentRoom]);
    }
    return;
}

std::vector<int> bfs(DungeonInput& data, int start) {
    std::vector<int> distances(data.rooms.size(), maxVal);
    std::vector<int> parents(data.rooms.size(), -1);
    std::vector<int> queue;

    distances[start] = 0;
    queue.push_back(start);

    while (!queue.empty()) {
        int current = queue.front();
        if(current == 0) {
            break;
        }
        queue.erase(queue.begin());

        for (int neighbor : data.rooms[current].neighbors) {
            if (data.rooms[neighbor].visited && distances[neighbor] == maxVal) {
                distances[neighbor] = distances[current] + 1;
                parents[neighbor] = current;
                queue.push_back(neighbor);
            }
        }
    }

    std::vector<int> path;
    for (int at = 0; at != -1; at = parents[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void returnBot(DungeonInput& data, int& currentRoom, int balance, int& points, std::ostream& out) {
    std::vector<int> optimalPath = bfs(data, currentRoom);
    balance -= (static_cast<int>(optimalPath.size()) - 1);
    for(size_t i = 0; i < optimalPath.size(); i++) {
        while(balance) {
            auto [bestType, count, totalValue] = collect(data.rooms[currentRoom], static_cast<int>(data.target)); 
            if(bestType == -1) {
                break;
            }
            points += totalValue;
            data.rooms[currentRoom].resources[bestType] = -1;
            collected[bestType] += count;
            writeCollect(out, static_cast<ResourceType>(bestType));
            writeState(out, data.rooms[currentRoom]);
            balance--;
        }

        int nextRoom = optimalPath[i];
        writeGo(out, nextRoom);
        if(nextRoom != 0) writeState(out, data.rooms[nextRoom]);
        
        currentRoom = nextRoom;
    }
}

void simulation(DungeonInput& data, std::ostream& out) {
    int balanceToGo = data.M / 2;
    int balanceToReturn = data.M - balanceToGo;

    int currentRoomId = 0;
    int points = 0;
    moveBot(data, currentRoomId, balanceToGo, points, out);
    returnBot(data, currentRoomId, balanceToReturn, points, out);
    writeResult(out, points);
}

//=================================================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>\n";
        return 1;
    }

    const std::string inputPath = argv[1];
    const std::string outputPath = "result.txt";
    std::ofstream out(outputPath);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << outputPath << "\n";
        return 1;
    }
    
    DungeonInput data;

    if (!parseDungeonInput(inputPath, data)) {
        writeIncorrectData(out);
        return 0;
    }

    simulation(data, out);
    //writeStubResult(outputPath);
    return 0;
}
