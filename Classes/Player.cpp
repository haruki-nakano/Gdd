//
//  Player.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Player.h"

bool Player::init() {
    if (!Sprite::initWithFile("player.png")) {
        return false;
    }
    return true;
}

void Player::onEnter() {
    Sprite::onEnter();
}