//
//  Player.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Player.h"

using namespace cocos2d;

bool Player::init() {
    if (!Sprite::initWithFile("player.png")) {
        return false;
    }

    _direction = Vec2(10.0f, 10.0f);

    return true;
}

void Player::onEnter() {
    Sprite::onEnter();
}

void Player::step() {
    Vec2 currentPosition = this->getPosition();

    this->setPosition(currentPosition + _direction);
}

void Player::setDirection(cocos2d::Vec2 direction) {
    _direction = direction;

    this->setRotation(atan2f(direction.x, direction.y) * 180.0f / M_PI);
}

void Player::stop() {
    _direction = Vec2(0.0f, 0.0f);
}