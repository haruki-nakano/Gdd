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

void Player::setMoveState(const MoveState MoveState) {
    _moving = MoveState;
    float angle;
    switch (MoveState) {
        case MoveState::STOP:
            _direction = Vec2(0.0f, 0.0f);
            return;

        case MoveState::LEFT:
            _direction = Vec2(-1.0f, 0.5f);
            break;

        case MoveState::RIGHT:
            angle = 135.0f;
            _direction = Vec2(1.0f, -0.5f);
            break;

        case MoveState::UP:
            angle = 45.0f;
            _direction = Vec2(1.0f, 0.5f);
            break;

        case MoveState::DOWN:
            angle = -135.0f;
            _direction = Vec2(-1.0f, -0.5f);
            break;
    }
    angle = MathUtils::degreesAngle(_direction);
    this->setRotation(angle);
}

MoveState Player::getMoveState() {
    return _moving;
}

Bullet *Player::createBullet() {
    Bullet *bullet = Bullet::create();
    bullet->setPosition(this->getPosition());
    bullet->setAngle(this->getRotation());
    return bullet;
}