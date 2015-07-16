//
//  Player.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Player.h"

#include "Bullet.h"
#include "Constants.h"
#include "MathUtils.h"

using namespace cocos2d;

bool Player::init() {
    if (!Sprite::initWithFile("player.png")) {
        return false;
    }

    return true;
}

void Player::onEnter() {
    Sprite::onEnter();
}

void Player::step() {
    Vec2 currentPosition = this->getPosition();

    this->setPosition(currentPosition + _direction * MOVE_FACTOR);
}

void Player::setMovingState(const MovingState movingState) {
    _moving = movingState;
    float angle;
    switch (movingState) {
        case MovingState::STOP:
            _direction = Vec2(0.0f, 0.0f);
            return;

        case MovingState::LEFT:
            angle = -45.0f;
            _direction = Vec2(-1.0f, 1.0f);
            break;

        case MovingState::RIGHT:
            angle = 135.0f;
            _direction = Vec2(1.0f, -1.0f);
            break;

        case MovingState::UP:
            angle = 45.0f;
            _direction = Vec2(1.0f, 1.0f);
            break;

        case MovingState::DOWN:
            angle = -135.0f;
            _direction = Vec2(-1.0f, -1.0f);
            break;
    }
    this->setRotation(angle);
}

MovingState Player::getMovingState() {
    return _moving;
}

Bullet *Player::createBullet() {
    Bullet *bullet = Bullet::create();
    bullet->setPosition(this->getPosition());
    bullet->setAngle(this->getRotation());
    return bullet;
}