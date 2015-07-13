//
//  Bullet.cpp
//  Yuzu
//
//  Created by Haruki Nakano on 2015/07/13.
//
//

#include "Bullet.h"
#include "MathUtils.h"

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

    // Vec2 delta = Vec2::forAngle(CC_DEGREES_TO_RADIANS(90.0f - _angle)) * 10.0f;
    Vec2 delta = MathUtils::forDegreesAngle(_angle) * 10.0f;
    this->setPosition(currentPosition + delta);
}

void Bullet::setAngle(float angle) {
    _angle = angle;
}