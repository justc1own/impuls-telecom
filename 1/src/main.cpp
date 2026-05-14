#include "bot.h"
#include "io.h"
#include "parser.h"

#include <fstream>
#include <iostream>
#include <string>

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
    auto error = parseDungeonInput(inputPath, data);
    if (error.has_value()) {
        writeIncorrectData(out, *error);
        return 0;
    }

    simulation(data, out);
    return 0;
}
