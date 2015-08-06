//
//  JSONPacker.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#ifndef __Yuzu__JSONPacker__
#define __Yuzu__JSONPacker__

#include "cocos2d.h"
#include "json/document.h"

#include "Bullet.h"
#include "Constants.h"
#include "Egg.h"

namespace JSONPacker {
struct GameState {
    std::string name;
    int playersHitCount;
    int opponentsHitCount;
    int playersHealCount;
    int opponentsHealCount;
    int eggLifePoint;
    cocos2d::Vec2 eggPosition;
    EggItemType eggItemType;
    cocos2d::Vec2 opponentPosition;
    MoveState opponentMoveState;
    Direction opponentDirection;
    EventType event;

    // Optional data
    std::vector<Bullet *> newBullets;
};

enum class DataType {
    STAGE_SELECT = 0,
    GAME_STATE = 1,
};

DataType dataTypeForData(const std::string &json);

int unpackStageSelectJSON(const std::string &json);
std::string packStageSelectJSON(const int data);

GameState unpackGameStateJSON(const std::string &json);
std::string packGameStateJSON(const GameState data);
}

#endif /* defined(__Yuzu__JSONPacker__) */
