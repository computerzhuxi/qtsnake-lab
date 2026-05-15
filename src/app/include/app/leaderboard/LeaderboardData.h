#ifndef APP_LEADERBOARD_LEADERBOARDDATA_H
#define APP_LEADERBOARD_LEADERBOARDDATA_H

#include <QString>
#include <cstdint>
#include <string>
#include <vector>

struct ScoreEntry {
    std::string playerName;
    int score = 0;
    int gridWidth = 0;
    int gridHeight = 0;
    int64_t timestamp = 0;
};

class LeaderboardData {
public:
    void addEntry(const ScoreEntry& entry);
    std::vector<ScoreEntry> topN(int n) const;

    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);

private:
    std::vector<ScoreEntry> entries_;
};

#endif // APP_LEADERBOARD_LEADERBOARDDATA_H
