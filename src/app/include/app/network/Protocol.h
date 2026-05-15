#ifndef APP_NETWORK_PROTOCOL_H
#define APP_NETWORK_PROTOCOL_H

#include <QJsonObject>
#include <QString>
#include <vector>

#include "core/engine/Direction.h"
#include "core/engine/Point.h"

namespace net {

enum class MsgType {
    JoinRequest,
    JoinResponse,
    Input,
    GameState,
    Heartbeat,
    HeartbeatAck,
    Disconnect
};

// Create messages
QJsonObject makeJoinRequest(const QString& playerName);
QJsonObject makeJoinResponse(bool accepted, int gridW, int gridH);
QJsonObject makeInput(core::Direction d, uint64_t tick);
QJsonObject makeGameState(const std::vector<core::Point>& snake,
                          core::Point food, int score, uint64_t tick);
QJsonObject makeHeartbeat(int seq);
QJsonObject makeHeartbeatAck(int seq);

// Parse helpers
MsgType parseType(const QJsonObject& obj);
core::Direction parseDirection(const QJsonObject& obj);
QString parsePlayerName(const QJsonObject& obj);
std::vector<core::Point> parseSnake(const QJsonObject& obj);
core::Point parseFood(const QJsonObject& obj);

} // namespace net

#endif // APP_NETWORK_PROTOCOL_H
