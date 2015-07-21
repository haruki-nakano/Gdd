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
    _lifePoint = DEFAULT_BULLET_LIFE;

    return true;
}

void Bullet::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    PhysicsBody *bulletPhysics = PhysicsBody::createBox(this->getBoundingBox().size);
    bulletPhysics->setDynamic(true);
    bulletPhysics->setGravityEnable(false);
    bulletPhysics->setRotationEnable(false);
    bulletPhysics->setCategoryBitmask(CATEGORY_MASK_BULLET);
    bulletPhysics->setCollisionBitmask(COLLISION_MASK_BULLET);
    bulletPhysics->setContactTestBitmask(CONTACT_MASK_BULLET);
    bulletPhysics->setVelocity(_direction * DEFAULT_BULLET_SPEED);

    this->setTag(CATEGORY_MASK_BULLET);
    this->setPhysicsBody(bulletPhysics);
}

void Bullet::step(float dt) {
    _lifePoint--;
}

int Bullet::getLifePoint() {
    return _lifePoint;
}

void Bullet::setLifePoint(int lifePoint) {
    _lifePoint = lifePoint;
}

void Bullet::setDirection(Vec2 v) {
    _direction = v;

    auto physics = this->getPhysicsBody();
    if (physics) {
        physics->setVelocity(_direction * DEFAULT_BULLET_SPEED);
    }
}

cocos2d::Vec2 Bullet::getDirectionVec() {
    return _direction;
}