#include "app/network/Protocol.h"
#include <QJsonArray>

namespace net {

static const char* msgTypeName(MsgType t) {
    switch (t) {
    case MsgType::JoinRequest:  return "JoinRequest";
    case MsgType::JoinResponse: return "JoinResponse";
    case MsgType::Input:        return "Input";
    case MsgType::GameState:    return "GameState";
    case MsgType::Heartbeat:    return "Heartbeat";
    case MsgType::HeartbeatAck: return "HeartbeatAck";
    case MsgType::Disconnect:   return "Disconnect";
    }
    return "Unknown";
}

static const char* dirName(core::Direction d) {
    switch (d) {
    case core::Direction::Up:    return "Up";
    case core::Direction::Down:  return "Down";
    case core::Direction::Left:  return "Left";
    case core::Direction::Right: return "Right";
    }
    return "Right";
}

// ---- Create ----

QJsonObject makeJoinRequest(const QString& name) {
    QJsonObject obj;
    obj["type"] = msgTypeName(MsgType::JoinRequest);
    obj["playerName"] = name;
    return obj;
}

QJsonObject makeJoinResponse(bool accepted, int w, int h) {
    QJsonObject obj;
    obj["type"] = msgTypeName(MsgType::JoinResponse);
    obj["accepted"] = accepted;
    obj["gridW"] = w;
    obj["gridH"] = h;
    return obj;
}

QJsonObject makeInput(core::Direction d, uint64_t tick) {
    QJsonObject obj;
    obj["type"] = msgTypeName(MsgType::Input);
    obj["direction"] = dirName(d);
    obj["tick"] = static_cast<qint64>(tick);
    return obj;
}

QJsonObject makeGameState(const std::vector<core::Point>& snake,
                           core::Point food, int score, uint64_t tick) {
    QJsonObject obj;
    obj["type"] = msgTypeName(MsgType::GameState);

    QJsonArray snakeArr;
    for (const auto& p : snake) {
        QJsonArray seg;
        seg.append(p.x);
        seg.append(p.y);
        snakeArr.append(seg);
    }
    obj["snake"] = snakeArr;

    QJsonObject foodObj;
    foodObj["x"] = food.x;
    foodObj["y"] = food.y;
    obj["food"] = foodObj;
    obj["score"] = score;
    obj["tick"] = static_cast<qint64>(tick);
    return obj;
}

QJsonObject makeHeartbeat(int seq) {
    QJsonObject obj;
    obj["type"] = msgTypeName(MsgType::Heartbeat);
    obj["seq"] = seq;
    return obj;
}

QJsonObject makeHeartbeatAck(int seq) {
    QJsonObject obj;
    obj["type"] = msgTypeName(MsgType::HeartbeatAck);
    obj["seq"] = seq;
    return obj;
}

// ---- Parse ----

MsgType parseType(const QJsonObject& obj) {
    QString t = obj["type"].toString();
    if (t == "JoinRequest")  return MsgType::JoinRequest;
    if (t == "JoinResponse") return MsgType::JoinResponse;
    if (t == "Input")        return MsgType::Input;
    if (t == "GameState")    return MsgType::GameState;
    if (t == "Heartbeat")    return MsgType::Heartbeat;
    if (t == "HeartbeatAck") return MsgType::HeartbeatAck;
    if (t == "Disconnect")   return MsgType::Disconnect;
    return MsgType::Disconnect;
}

core::Direction parseDirection(const QJsonObject& obj) {
    QString d = obj["direction"].toString();
    if (d == "Up")    return core::Direction::Up;
    if (d == "Down")  return core::Direction::Down;
    if (d == "Left")  return core::Direction::Left;
    if (d == "Right") return core::Direction::Right;
    return core::Direction::Right;
}

QString parsePlayerName(const QJsonObject& obj) {
    return obj["playerName"].toString();
}

std::vector<core::Point> parseSnake(const QJsonObject& obj) {
    std::vector<core::Point> result;
    QJsonArray arr = obj["snake"].toArray();
    for (const auto& val : arr) {
        QJsonArray seg = val.toArray();
        if (seg.size() >= 2) {
            result.push_back({seg[0].toInt(), seg[1].toInt()});
        }
    }
    return result;
}

core::Point parseFood(const QJsonObject& obj) {
    QJsonObject f = obj["food"].toObject();
    return {f["x"].toInt(), f["y"].toInt()};
}

} // namespace net
