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

namespace JSONPacker {
struct GameState {
    std::string name;
    int playersLifePoint;
    int opponentsLifePoint;
    cocos2d::Vec2 opponentPosition;
    MoveState opponentMoveState;
    Bullet *newBullet;
};

GameState unpackGameStateJSON(std::string json);
std::string packGameStateJSON(const GameState data);
}

#endif /* defined(__Yuzu__JSONPacker__) */
