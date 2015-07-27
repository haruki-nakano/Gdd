//
//  JSONPacker.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "JSONPacker.h"

#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "Constants.h"

using namespace cocos2d;

namespace JSONPacker {

GameState unpackGameStateJSON(std::string json) {
    rapidjson::Document document;
    document.Parse<0>(json.c_str());

    GameState gameState;

    gameState.name = document["name"].GetString();
    gameState.opponentMoveState = static_cast<MoveState>(document["moveState"].GetInt());
    gameState.event = static_cast<EventType>(document["eventType"].GetInt());
    gameState.playersHitCount = document["playersHitCount"].GetInt();
    gameState.opponentsHitCount = document["oponentsHitCount"].GetInt();
    gameState.playersHealCount = document["playersHealCount"].GetInt();
    gameState.opponentsHealCount = document["oponentsHealCount"].GetInt();

    rapidjson::Value &position = document["position"];
    float x = (float)position["x"].GetDouble();
    float y = (float)position["y"].GetDouble();
    gameState.opponentPosition = Vec2(x, y);

    if (document.HasMember("bullet")) {
        rapidjson::Value &bulletPosition = document["bullet"]["position"];
        rapidjson::Value &bulletDirection = document["bullet"]["direction"];
        Vec2 pos = Vec2(bulletPosition["x"].GetDouble(), bulletPosition["y"].GetDouble());
        Vec2 direction = Vec2(bulletDirection["x"].GetDouble(), bulletDirection["y"].GetDouble());

        Bullet *bullet = Bullet::create();
        bullet->setPosition(pos);
        bullet->setDirection(direction);
        bullet->setTag(TAG_OPPOPENT_BULLET);

        gameState.newBullet = bullet;
    } else {
        gameState.newBullet = nullptr;
    }

    rapidjson::Value &eggPosition = document["egg"]["position"];
    Vec2 pos = Vec2(eggPosition["x"].GetDouble(), eggPosition["y"].GetDouble());

    gameState.eggLifePoint = document["egg"]["lifePoint"].GetInt();
    gameState.eggPosition = pos;

    return gameState;
}

std::string packGameStateJSON(const GameState gameState) {
    rapidjson::Document document;
    document.SetObject();
    document.AddMember("name", gameState.name.c_str(), document.GetAllocator());
    document.AddMember("eventType", static_cast<int>(gameState.event), document.GetAllocator());
    document.AddMember("moveState", static_cast<int>(gameState.opponentMoveState), document.GetAllocator());
    document.AddMember("playersHitCount", gameState.playersHitCount, document.GetAllocator());
    document.AddMember("oponentsHitCount", gameState.opponentsHitCount, document.GetAllocator());
    document.AddMember("playersHealCount", gameState.playersHealCount, document.GetAllocator());
    document.AddMember("oponentsHealCount", gameState.opponentsHealCount, document.GetAllocator());

    rapidjson::Value positionJson(rapidjson::kObjectType);
    positionJson.AddMember("x", gameState.opponentPosition.x, document.GetAllocator());
    positionJson.AddMember("y", gameState.opponentPosition.y, document.GetAllocator());

    document.AddMember("position", positionJson, document.GetAllocator());

    // Bullet
    if (gameState.newBullet) {
        rapidjson::Value bulletJson(rapidjson::kObjectType);

        rapidjson::Value bulletPositionJson(rapidjson::kObjectType);
        bulletPositionJson.AddMember("x", gameState.newBullet->getPosition().x, document.GetAllocator());
        bulletPositionJson.AddMember("y", gameState.newBullet->getPosition().y, document.GetAllocator());

        rapidjson::Value bulletDirection(rapidjson::kObjectType);
        bulletDirection.AddMember("x", gameState.newBullet->getDirectionVec().x, document.GetAllocator());
        bulletDirection.AddMember("y", gameState.newBullet->getDirectionVec().y, document.GetAllocator());

        bulletJson.AddMember("position", bulletPositionJson, document.GetAllocator());
        bulletJson.AddMember("direction", bulletDirection, document.GetAllocator());

        document.AddMember("bullet", bulletJson, document.GetAllocator());
    }

    // Egg
    rapidjson::Value eggJson(rapidjson::kObjectType);

    rapidjson::Value eggPositionJson(rapidjson::kObjectType);
    eggPositionJson.AddMember("x", gameState.eggPosition.x, document.GetAllocator());
    eggPositionJson.AddMember("y", gameState.eggPosition.y, document.GetAllocator());

    eggJson.AddMember("position", eggPositionJson, document.GetAllocator());
    eggJson.AddMember("lifePoint", gameState.eggLifePoint, document.GetAllocator());

    document.AddMember("egg", eggJson, document.GetAllocator());

    // Flush
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string returnString(buffer.GetString(), buffer.Size());

    return returnString;
}
}