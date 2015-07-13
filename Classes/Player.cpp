//
//  Player.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Player.h"

#include "Bullet.h"
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

    this->setPosition(currentPosition + _direction);
}

void Player::setDirection(const Vec2 direction) {
    _direction = direction;

    float angle = MathUtils::degreesAngle(direction);
    this->setRotation(angle);
}

void Player::stop() {
    _direction = Vec2(0.0f, 0.0f);
}

Bullet *Player::createBullet() {
    Bullet *bullet = Bullet::create();
    bullet->setPosition(this->getPosition());
    bullet->setAngle(this->getRotation());
    return bullet;
}