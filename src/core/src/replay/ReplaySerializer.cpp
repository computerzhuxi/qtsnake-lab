#include "core/replay/ReplaySerializer.h"
#include <fstream>
#include <sstream>
#include <vector>

namespace core {

// Helper: parse "x,y" or "x y" into Point
static Point parsePoint(const std::string& s) {
    Point p;
    size_t comma = s.find(',');
    if (comma != std::string::npos) {
        p.x = std::stoi(s.substr(0, comma));
        p.y = std::stoi(s.substr(comma + 1));
    } else {
        size_t space = s.find(' ');
        if (space != std::string::npos) {
            p.x = std::stoi(s.substr(0, space));
            p.y = std::stoi(s.substr(space + 1));
        }
    }
    return p;
}

static Direction parseDirection(const std::string& s) {
    if (s == "Up")    return Direction::Up;
    if (s == "Down")  return Direction::Down;
    if (s == "Left")  return Direction::Left;
    if (s == "Right") return Direction::Right;
    return Direction::Right;
}

static const char* directionName(Direction d) {
    switch (d) {
    case Direction::Up:    return "Up";
    case Direction::Down:  return "Down";
    case Direction::Left:  return "Left";
    case Direction::Right: return "Right";
    }
    return "Right";
}

// Split "15,10 14,10 13,10" into deque of Points
static std::deque<Point> parseSnake(const std::string& s) {
    std::deque<Point> result;
    std::istringstream iss(s);
    std::string token;
    while (iss >> token) {
        result.push_back(parsePoint(token));
    }
    return result;
}

static std::string serializeSnake(const std::deque<Point>& body) {
    std::ostringstream oss;
    for (size_t i = 0; i < body.size(); ++i) {
        if (i > 0) oss << ' ';
        oss << body[i].x << ',' << body[i].y;
    }
    return oss.str();
}

bool saveReplayData(const ReplayData& data, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << "# QtSnake Lab Replay v2\n";

    // [config]
    file << "\n[config]\n";
    file << "grid_width=" << data.config.gridWidth << '\n';
    file << "grid_height=" << data.config.gridHeight << '\n';
    file << "initial_speed=" << data.config.initialSpeed << '\n';
    file << "speed_increment=" << data.config.speedIncrement << '\n';
    file << "min_speed=" << data.config.minSpeed << '\n';
    file << "initial_snake_length=" << data.config.initialSnakeLength << '\n';

    // [initial]
    file << "\n[initial]\n";
    file << "rng_seed=" << data.rngSeed << '\n';
    file << "direction=" << directionName(data.initialDirection) << '\n';
    file << "snake=" << serializeSnake(data.initialSnake) << '\n';
    file << "food=" << data.initialFood.x << ',' << data.initialFood.y << '\n';
    file << "score=" << data.initialScore << '\n';

    // [inputs]
    file << "\n[inputs]\n";
    for (const auto& [tick, dir] : data.directionChanges) {
        file << tick << '=' << directionName(dir) << '\n';
    }

    // [result]
    file << "\n[result]\n";
    file << "total_ticks=" << data.totalTicks << '\n';
    file << "final_score=" << data.finalScore << '\n';

    return file.good();
}

ReplayData loadReplayData(const std::string& filepath) {
    ReplayData data;
    std::ifstream file(filepath);
    if (!file.is_open()) return data;

    std::string line;
    std::string section;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        // Section header
        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        // Key=Value
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);

        if (section == "config") {
            if (key == "grid_width") data.config.gridWidth = std::stoi(val);
            else if (key == "grid_height") data.config.gridHeight = std::stoi(val);
            else if (key == "initial_speed") data.config.initialSpeed = std::stoi(val);
            else if (key == "speed_increment") data.config.speedIncrement = std::stoi(val);
            else if (key == "min_speed") data.config.minSpeed = std::stoi(val);
            else if (key == "initial_snake_length") data.config.initialSnakeLength = std::stoi(val);
        } else if (section == "initial") {
            if (key == "rng_seed") data.rngSeed = static_cast<unsigned int>(std::stoul(val));
            else if (key == "direction") data.initialDirection = parseDirection(val);
            else if (key == "snake") data.initialSnake = parseSnake(val);
            else if (key == "food") data.initialFood = parsePoint(val);
            else if (key == "score") data.initialScore = std::stoi(val);
        } else if (section == "inputs") {
            uint64_t tick = std::stoull(key);
            data.directionChanges[tick] = parseDirection(val);
        } else if (section == "result") {
            if (key == "total_ticks") data.totalTicks = std::stoull(val);
            else if (key == "final_score") data.finalScore = std::stoi(val);
        }
    }

    return data;
}

} // namespace core
