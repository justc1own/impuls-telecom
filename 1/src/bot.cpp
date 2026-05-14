#include "bot.h"
#include "io.h"

#include <algorithm>
#include <tuple>
#include <vector>

#include <iostream>

const int maxVal = 1e9;

std::tuple<int, int, int> collect(const Room& room, int targetResource = -1) {
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

void moveBot(DungeonInput& data, int& currentRoom, int balance, int& points, std::ostream& out) {
    if (currentRoom != 0) {
        writeGo(out, currentRoom);
        writeState(out, data.rooms[currentRoom]);
    }

    data.rooms[currentRoom].visited = true;

    auto [bestType, count, totalValue] = collect(data.rooms[currentRoom], static_cast<int>(data.target));

    if (bestType != -1) {
        points += totalValue;
        data.rooms[currentRoom].resources[bestType] = -1;
        collected[bestType] += count;
        writeCollect(out, static_cast<ResourceType>(bestType));
        writeState(out, data.rooms[currentRoom]);
    }

    int mn = maxVal;
    for (auto& neighbor : data.rooms[currentRoom].neighbors) {
        if (!data.rooms[neighbor].visited) {
            mn = std::min(mn, data.rooms[neighbor].id);
        }
    }
    if (mn == maxVal) {
        return;
    }
    if (balance == 0) {
        return;
    }
    int nextRoom = mn;
    moveBot(data, nextRoom, balance - 1, points, out);
    currentRoom = nextRoom;
    // if (currentRoom != 0) {
    //     writeGo(out, currentRoom);
    //     writeState(out, data.rooms[currentRoom]);
    // }
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
        if (current == 0) {
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
    for (size_t i = 1; i < optimalPath.size(); i++) {
        while (balance) {
            auto [bestType, count, totalValue] = collect(data.rooms[currentRoom], static_cast<int>(data.target));
            if (bestType == -1) {
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
        //std::cout << "next room: " << nextRoom << "\n";
        writeGo(out, nextRoom);
        if (nextRoom != 0) writeState(out, data.rooms[nextRoom]);

        currentRoom = nextRoom;
    }
}

void simulation(DungeonInput& data, std::ostream& out) {
    int balanceToGo = data.M / 2;
    int balanceToReturn = data.M - balanceToGo;

    int currentRoomId = 0;
    int points = 0;
    moveBot(data, currentRoomId, balanceToGo, points, out);
    //std::cout << "current room after going: " << currentRoomId << "\n";
    returnBot(data, currentRoomId, balanceToReturn, points, out);
    writeResult(out, points);
}
