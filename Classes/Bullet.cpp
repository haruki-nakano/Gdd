//
//  Bullet.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Bullet.h"
#include "MathUtils.h"
#include "Constants.h"

using namespace cocos2d;

bool Bullet::init() {
    if (!Sprite::initWithFile("bullet.png")) {
        return false;
    }

    _angle = 0.0f;

    return true;
}

void Bullet::onEnter() {
    Sprite::onEnter();
}

void Bullet::step() {
    Vec2 currentPosition = this->getPosition();

    Vec2 delta = MathUtils::forDegreesAngle(_angle) * DEFAULT_BULLET_SPEED;
    this->setPosition(currentPosition + delta);
}

void Bullet::setAngle(float angle) {
    _angle = angle;
}