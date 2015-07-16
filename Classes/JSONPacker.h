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

namespace JSONPacker {
struct GameState {
    std::string name;
    bool gameOver;
    cocos2d::Vec2 position;
    
};

GameState unpackGameStateJSON(std::string json);
std::string packGameStateJSON(const GameState data);
}

#endif /* defined(__Yuzu__JSONPacker__) */
