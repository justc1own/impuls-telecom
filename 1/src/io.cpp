#include "io.h"

void writeIncorrectData(std::ostream& out, const std::string& errorMessage) {
    if (!out) {
        return;
    }
    out << errorMessage << "\n";
}

void writeGo(std::ostream& out, int roomId) {
    if (!out) {
        return;
    }
    out << "go " << roomId << "\n";
}

void writeCollect(std::ostream& out, ResourceType resource) {
    if (!out) {
        return;
    }
    out << "collect " << ResourceNames[static_cast<size_t>(resource)] << "\n";
}

const int extract = -1;

void writeState(std::ostream& out, Room& room) {
    if (!out) {
        return;
    }
    out << "state " << room.id << " ";
    for (auto i : room.resources) {
        if (i != -1)
            out << i << " ";
        else
            out << "_ ";
    }
    out << "\n";
}

void writeResult(std::ostream& out, int points) {
    if (!out) {
        return;
    }

    out << "result "
        << collected[0] << " "
        << collected[1] << " "
        << collected[2] << " "
        << collected[3] << " "
        << points << "\n";
}
