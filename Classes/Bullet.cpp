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
    _lifePoint = INITIAL_BULLET_LIFE;

    return true;
}

void Bullet::onEnter() {
    Sprite::onEnter();

    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    PhysicsBody *bulletPhysics = PhysicsBody::createBox(this->getBoundingBox().size * 0.5f);
    bulletPhysics->setDynamic(true);
    bulletPhysics->setGravityEnable(false);
    bulletPhysics->setRotationEnable(false);
    bulletPhysics->setCategoryBitmask(CATEGORY_MASK_BULLET);
    bulletPhysics->setCollisionBitmask(COLLISION_MASK_BULLET);
    bulletPhysics->setContactTestBitmask(CONTACT_MASK_BULLET);
    bulletPhysics->setVelocity(_direction * DEFAULT_BULLET_SPEED);

    this->setPhysicsBody(bulletPhysics);
}

void Bullet::step(const float dt) {
    _lifePoint -= dt;
}

float Bullet::getLifePoint() const {
    return _lifePoint;
}

void Bullet::setLifePoint(float lifePoint) {
    _lifePoint = lifePoint;
}

void Bullet::setDirection(const Vec2 v) {
    _direction = v;

    auto physics = this->getPhysicsBody();
    if (physics) {
        physics->setVelocity(_direction * DEFAULT_BULLET_SPEED);
    }
}

cocos2d::Vec2 Bullet::getDirectionVec() const {
    return _direction;
}