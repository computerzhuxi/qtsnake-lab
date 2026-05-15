#include "app/leaderboard/LeaderboardData.h"
#include "core/logging/Logger.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>

void LeaderboardData::addEntry(const ScoreEntry& entry) {
    entries_.push_back(entry);
    std::sort(entries_.begin(), entries_.end(),
              [](const ScoreEntry& a, const ScoreEntry& b) {
                  return a.score > b.score;
              });
    // Keep only top 100
    if (entries_.size() > 100) {
        entries_.resize(100);
    }
}

std::vector<ScoreEntry> LeaderboardData::topN(int n) const {
    size_t count = std::min(static_cast<size_t>(n), entries_.size());
    return std::vector<ScoreEntry>(entries_.begin(), entries_.begin() + count);
}

bool LeaderboardData::loadFromFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) return false;

    entries_.clear();
    for (const auto& val : doc.array()) {
        QJsonObject obj = val.toObject();
        ScoreEntry entry;
        entry.playerName = obj["name"].toString().toStdString();
        entry.score      = obj["score"].toInt();
        entry.gridWidth  = obj["gridW"].toInt();
        entry.gridHeight = obj["gridH"].toInt();
        entry.timestamp  = obj["time"].toVariant().toLongLong();
        entries_.push_back(entry);
    }
    std::sort(entries_.begin(), entries_.end(),
              [](const ScoreEntry& a, const ScoreEntry& b) {
                  return a.score > b.score;
              });
    return true;
}

bool LeaderboardData::saveToFile(const QString& path) {
    QJsonArray arr;
    for (const auto& e : entries_) {
        QJsonObject obj;
        obj["name"]   = QString::fromStdString(e.playerName);
        obj["score"]  = e.score;
        obj["gridW"]  = e.gridWidth;
        obj["gridH"]  = e.gridHeight;
        obj["time"]   = static_cast<qint64>(e.timestamp);
        arr.append(obj);
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(QJsonDocument(arr).toJson());
    return true;
}
