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
#include "PhysicsConstants.h"

using namespace cocos2d;

namespace JSONPacker {

DataType dataTypeForData(const std::string &json) {
    rapidjson::Document document;
    document.Parse<0>(json.c_str());
    return static_cast<DataType>(document["dataType"].GetInt());
}

int unpackStageSelectJSON(const std::string &json) {
    rapidjson::Document document;
    document.Parse<0>(json.c_str());
    return document["stageId"].GetInt();
}

std::string packStageSelectJSON(const int data) {
    rapidjson::Document document;
    document.SetObject();
    document.AddMember("dataType", static_cast<int>(DataType::STAGE_SELECT), document.GetAllocator());
    document.AddMember("stageId", data, document.GetAllocator());

    // Flush
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string returnString(buffer.GetString(), buffer.Size());

    return returnString;
}

GameState unpackGameStateJSON(const std::string &json) {
    rapidjson::Document document;
    document.Parse<0>(json.c_str());

    GameState gameState;

    gameState.name = document["name"].GetString();
    gameState.opponentMoveState = static_cast<MoveState>(document["moveState"].GetInt());
    gameState.opponentDirection = static_cast<Direction>(document["direction"].GetInt());
    gameState.event = static_cast<EventType>(document["eventType"].GetInt());
    gameState.playersHitCount = document["playersHitCount"].GetInt();
    gameState.opponentsHitCount = document["oponentsHitCount"].GetInt();
    gameState.playersHealCount = document["playersHealCount"].GetInt();
    gameState.opponentsHealCount = document["oponentsHealCount"].GetInt();

    rapidjson::Value &position = document["position"];
    float x = (float)position["x"].GetDouble();
    float y = (float)position["y"].GetDouble();
    gameState.opponentPosition = Vec2(x, y);

    gameState.newBullets = std::vector<Bullet *>();
    rapidjson::Value &bullets = document["bulletArray"];
    auto size = bullets.Size();
    for (rapidjson::SizeType i = 0; i < size; i++) {
        rapidjson::Value &bulletJson = bullets[i];

        float lifePoint = bulletJson["lifePoint"].GetDouble();

        rapidjson::Value &bulletPosition = bulletJson["position"];
        rapidjson::Value &bulletDirection = bulletJson["direction"];
        Vec2 pos = Vec2(bulletPosition["x"].GetDouble(), bulletPosition["y"].GetDouble());
        Vec2 direction = Vec2(bulletDirection["x"].GetDouble(), bulletDirection["y"].GetDouble());

        Bullet *bullet = Bullet::create();
        bullet->setPosition(pos);
        bullet->setDirection(direction);
        bullet->setLifePoint(lifePoint);
        bullet->setTag(TAG_OPPOPENT_BULLET);

        gameState.newBullets.push_back(bullet);
    }

    rapidjson::Value &eggPosition = document["egg"]["position"];
    Vec2 pos = Vec2(eggPosition["x"].GetDouble(), eggPosition["y"].GetDouble());

    gameState.eggLifePoint = document["egg"]["lifePoint"].GetInt();
    gameState.eggItemType = static_cast<EggItemType>(document["egg"]["itemType"].GetInt());
    gameState.eggPosition = pos;

    return gameState;
}

std::string packGameStateJSON(const GameState gameState) {
    rapidjson::Document document;
    document.SetObject();

    document.AddMember("dataType", static_cast<int>(DataType::GAME_STATE), document.GetAllocator());

    document.AddMember("name", gameState.name.c_str(), document.GetAllocator());
    document.AddMember("eventType", static_cast<int>(gameState.event), document.GetAllocator());
    document.AddMember("moveState", static_cast<int>(gameState.opponentMoveState), document.GetAllocator());
    document.AddMember("direction", static_cast<int>(gameState.opponentDirection), document.GetAllocator());
    document.AddMember("playersHitCount", gameState.playersHitCount, document.GetAllocator());
    document.AddMember("oponentsHitCount", gameState.opponentsHitCount, document.GetAllocator());
    document.AddMember("playersHealCount", gameState.playersHealCount, document.GetAllocator());
    document.AddMember("oponentsHealCount", gameState.opponentsHealCount, document.GetAllocator());

    rapidjson::Value positionJson(rapidjson::kObjectType);
    positionJson.AddMember("x", gameState.opponentPosition.x, document.GetAllocator());
    positionJson.AddMember("y", gameState.opponentPosition.y, document.GetAllocator());

    document.AddMember("position", positionJson, document.GetAllocator());

    // Bullets
    rapidjson::Value bulletArray(rapidjson::kArrayType);
    for (Bullet *bullet : gameState.newBullets) {
        rapidjson::Value bulletJson(rapidjson::kObjectType);

        rapidjson::Value bulletPositionJson(rapidjson::kObjectType);
        bulletPositionJson.AddMember("x", bullet->getPosition().x, document.GetAllocator());
        bulletPositionJson.AddMember("y", bullet->getPosition().y, document.GetAllocator());

        rapidjson::Value bulletDirection(rapidjson::kObjectType);
        bulletDirection.AddMember("x", bullet->getDirectionVec().x, document.GetAllocator());
        bulletDirection.AddMember("y", bullet->getDirectionVec().y, document.GetAllocator());

        bulletJson.AddMember("position", bulletPositionJson, document.GetAllocator());
        bulletJson.AddMember("direction", bulletDirection, document.GetAllocator());

        bulletJson.AddMember("lifePoint", bullet->getLifePoint(), document.GetAllocator());

        bulletArray.PushBack(bulletJson, document.GetAllocator());
    }
    document.AddMember("bulletArray", bulletArray, document.GetAllocator());

    // Egg
    rapidjson::Value eggJson(rapidjson::kObjectType);

    rapidjson::Value eggPositionJson(rapidjson::kObjectType);
    eggPositionJson.AddMember("x", gameState.eggPosition.x, document.GetAllocator());
    eggPositionJson.AddMember("y", gameState.eggPosition.y, document.GetAllocator());

    eggJson.AddMember("position", eggPositionJson, document.GetAllocator());
    eggJson.AddMember("lifePoint", gameState.eggLifePoint, document.GetAllocator());
    eggJson.AddMember("itemType", static_cast<int>(gameState.eggItemType), document.GetAllocator());

    document.AddMember("egg", eggJson, document.GetAllocator());

    // Flush
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string returnString(buffer.GetString(), buffer.Size());

    return returnString;
}
}