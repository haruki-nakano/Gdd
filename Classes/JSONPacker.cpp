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

using namespace cocos2d;

namespace JSONPacker {

GameState unpackGameStateJSON(std::string json) {
    rapidjson::Document document;
    document.Parse<0>(json.c_str());

    GameState gameState;

    gameState.name = document["name"].GetString();
    gameState.gameOver = document["gameOver"].GetBool();

    rapidjson::Value &position = document["position"];
    float x = (float)position["x"].GetDouble();
    float y = (float)position["y"].GetDouble();
    gameState.position = Vec2(x, y);

    return gameState;
}

std::string packGameStateJSON(const GameState gameState) {
    rapidjson::Document document;
    document.SetObject();
    document.AddMember("name", gameState.name.c_str(), document.GetAllocator());
    document.AddMember("gameOver", gameState.gameOver, document.GetAllocator());

    rapidjson::Value positionJson(rapidjson::kObjectType);
    positionJson.AddMember("x", gameState.position.x, document.GetAllocator());
    positionJson.AddMember("y", gameState.position.y, document.GetAllocator());

    document.AddMember("position", positionJson, document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string returnString(buffer.GetString(), buffer.Size());

    return returnString;
}
}