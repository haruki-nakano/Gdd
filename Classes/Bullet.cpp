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

    _direction = Vec2(0.0f, 0.0f);

    PhysicsBody *bulletPhysics = PhysicsBody::createBox(Size(5.0f, 5.0f));
    bulletPhysics->setDynamic(true);
    bulletPhysics->setGravityEnable(false);
    bulletPhysics->setRotationEnable(false);
    bulletPhysics->setCategoryBitmask(CATEGORY_MASK_BULLET);
    bulletPhysics->setCollisionBitmask(CONTACT_MASK_BULLET);
    bulletPhysics->setVelocity(_direction * DEFAULT_BULLET_SPEED);
    bulletPhysics->setContactTestBitmask(0);

    this->setPhysicsBody(bulletPhysics);

    return true;
}

void Bullet::onEnter() {
    Sprite::onEnter();
}

void Bullet::setDirection(Vec2 v) {
    _direction = v;

    this->getPhysicsBody()->setVelocity(_direction * DEFAULT_BULLET_SPEED);
}

cocos2d::Vec2 Bullet::getDirectionVec() {
    return _direction;
}